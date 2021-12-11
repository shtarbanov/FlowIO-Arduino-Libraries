/*
There is a slew rate or propagation delay problem, thus we must have a short delay after each
digitalWrite mux change that is preceding an analogRead.Even 1ms is perfectly sufficient.

The above fixes the issue with having 0 values when nothing is connected to the inputs. However, it
does not fix the other issue of cross-contamination between channels. To fix that one we must have
a dummy read followed by a short delay before each real read. The effect is even better if we have
2 dummy reads with a selay after each one.
*/

#include "Arduino.h"
#include "Module16Inputs.h"
#include <array>

//Constructor
Module16Inputs::Module16Inputs() : sampleIndex(0) {
    enableModule();
    reading.fill(0);
    for (auto &buffer: samples) {
        buffer.fill(0);
    }
    pinMode(x, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(s0, OUTPUT);
    pinMode(_A4, INPUT);
    pinMode(_A5, INPUT);
}

//Methods
void Module16Inputs::enableModule() {
    digitalWrite(_enPin, OUTPUT);
    pinMode(_enPin, HIGH);
}

void Module16Inputs::disableModule() {
    pinMode(_enPin, LOW);
}

void Module16Inputs::setDiffChannel(uint8_t channel) {
    if (channel <= 16) {
        diffChannel = channel;
    }
}

uint32_t Module16Inputs::setSettleTime(uint32_t microseconds) {
    uint32_t oldValue = settleTime;
    settleTime = microseconds;
    return oldValue;
}

void Module16Inputs::setSampleAveraging(SampleAveraging numSamples) {
    sampleAveraging = numSamples;
}

void Module16Inputs::setMuxesChannel(uint8_t n) {
    bool xVal = (n % 2 == 1);
    bool s1Val = !(n < 9);
    bool s0Val = (n == 5 || n == 6 || n == 7 || n == 8 || n == 13 || n == 14 || n == 15 || n == 16);
    digitalWrite(x, xVal);
    digitalWrite(s1, s1Val);
    digitalWrite(s0, s0Val);
    delayMicroseconds(settleTime); //required for the MCU signal to rise after switching.
}

uint16_t Module16Inputs::readChannel(uint8_t n) {
    if (n < 1 || n > 16) return -1;

    uint8_t analogPin = (n == 1 || n == 2 || n == 5 || n == 6 || n == 9 || n == 10 || n == 13 || n == 14) ? A5 : A4;
    setMuxesChannel(n);
    uint32_t sample = 0;
    // Sample once to draw access capacitance.
    analogRead(analogPin);
    // SampleAveraging can be either 1, 2 or 4
    for (int i = 0; i < (uint8_t)sampleAveraging; i++) {
        delayMicroseconds(settleTime);
        sample += analogRead(analogPin);
    }
    // If we have a difference channel set, sample that one also and diff the result.
    if (diffChannel > 0 && diffChannel <= 16 && diffChannel != n) {
        uint16_t diffSample = readChannel(diffChannel);
        // Subtract the diff from the sample, or if diff is greater than the sample zero the sample to avoid overflow.
        // TODO: Consider returning signed results to accommodate for diffing.
        sample -= min(diffSample, sample);
    }
    return (uint16_t)((sample) / sampleAveraging);
}

// Why this exists?
uint16_t Module16Inputs::readChannelMovAvg(uint8_t channel, uint8_t numSamples) {
    samples[sampleIndex][channel] = readChannel(channel);
    sampleIndex = (sampleIndex + 1) % BUFFER_SIZE;

    uint16_t tmp = 0;
    for (int i = 0; i < numSamples; i++) {
        const buffer_t &sample = samples[(sampleIndex - numSamples - 1 + i) % BUFFER_SIZE];
        // Sum a sample with the temporary buffer.
        tmp += sample[channel];
    }
    tmp /= numSamples;
    return tmp;
}

template<size_t N>
void sum(const std::array <uint16_t, N> &a, std::array <uint16_t, N> &into) {
    for (size_t i = 0; i < N; i++) {
        into[i] += a[i];
    }
}

template<size_t N>
void divideArray(uint16_t by, std::array <uint16_t, N> &array) {
    for (size_t i = 0; i < N; i++) {
        array[i] /= by;
    }
}

void Module16Inputs::read16ChannelsMovAvgInto(uint16_t out[16],
                                              uint8_t numSamples) { //argument is the array provided by user containing the averaged results only.
    // 1. Sample all inputs into the samples array.
    // 2. Increment cyclic index i
    // 3. Sum samples[((i - 1 - numSamples) .. (i - 1)) mod BUFFER_SIZE]
    // 4. return average.

    read16ChannelsInto(samples[sampleIndex]);
    sampleIndex = (sampleIndex + 1) % BUFFER_SIZE;

    std::array <uint16_t, INPUTS> tmp = {0};
    for (int i = 0; i < numSamples; i++) {
        const buffer_t &sample = samples[(sampleIndex - numSamples - 1 + i) % BUFFER_SIZE];
        // Sum a sample with the temporary buffer
        sum<INPUTS>(sample, tmp);
    }
    divideArray<INPUTS>(numSamples, tmp);
    for (int i = 0; i < INPUTS; i++) {
        out[i] = tmp[i];
    }
}

//It is strange that I cannot pass an array
void Module16Inputs::read16ChannelsInto(uint16_t out[16]) { //The results are stored into the array data16Channels
    //Read all 16 values and store them into the array passed in the argument. Index 0 is sensor 1.
    for (int i = 0; i < 4; i++) {
        out[i] = readChannel(i + 1);
        out[i + 4] = readChannel(i + 5);
        out[i + 8] = readChannel(i + 9);
        out[i + 12] = readChannel(i + 13);
    }
}

void Module16Inputs::read16ChannelsInto(buffer_t &out) {
    for (int i = 0; i < 4; i++) {
        out[i] = readChannel(i + 1);
        out[i + 4] = readChannel(i + 5);
        out[i + 8] = readChannel(i + 9);
        out[i + 12] = readChannel(i + 13);
    }
}
/*
------------------------
i  |  # | x s1 s0  pin
------------------------
0  |  1 | 1  0  0   A5 |
1  |  2 | 0  0  0   A5 |
2  |  3 | 1  0  0   A4 |
3  |  4 | 0  0  0   A4 |
4  |  5 | 1  0  1   A5 |
5  |  6 | 0  0  1   A5 |
6  |  7 | 1  0  1   A4 |
7  |  8 | 0  0  1   A4 |
8  |  9 | 1  0  1   A5 |
9  | 10 | 0  1  0   A5 |
10 | 11 | 1  1  0   A4 |
11 | 12 | 0  1  0   A4 |
12 | 13 | 1  1  0   A5 |
13 | 14 | 0  1  1   A5 |
14 | 15 | 1  1  1   A4 |
15 | 16 | 0  1  1   A4 |
*/
