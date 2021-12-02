/* Module16Inputs.h - a library for the 16 input module

I can implement this as part of the FlowIO main library and then invoke the functions here
on the flowio instance.

Alternatively, I can invoke them as flowio.Module16Inputs.functionName()

I can also have my FlowIO library inherit this library. See https://www.tutorialspoint.com/cplusplus/cpp_inheritance.htm
*/

#ifndef Module16Inputs_h
#define Module16Inputs_h

#include "Arduino.h"
#include <array>

using std::array;

const size_t INPUTS = 16;
const size_t BUFFER_SIZE = 10;

typedef array <uint16_t, INPUTS> buffer_t;

enum SampleAveraging {
    SINGLE_SAMPLING = 1,
    DOUBLE_SAMPLING = 2,
    QUADRUPLE_SAMPLING = 4
};

class Module16Inputs {
private:
    uint8_t _enPin = 2;
    uint8_t x = 26;
    uint8_t s0 = 25;
    uint8_t s1 = 24;

    uint8_t _A4 = A4;
    uint8_t _A5 = A5;
    uint32_t settleTime = 750;
    SampleAveraging sampleAveraging = DOUBLE_SAMPLING;

    //The following arrays are for the moving average calculations.
    //The arrays must be initialized, otherwise I get garbage initially.
    //TODO: initialize the arrays with the first value read rather than 0 if possible.

    buffer_t reading;
    array <buffer_t, BUFFER_SIZE> samples;
    size_t sampleIndex;

    uint8_t diffChannel;

public:
    Module16Inputs(); //Constructor
    void setMuxesChannel(uint8_t n);

    void enableModule();

    void disableModule();

    uint16_t readChannel(uint8_t n); //rename to readInput
    void read16ChannelsInto(uint16_t out[16]); //argument is the array that will store the values.
    void read16ChannelsInto(buffer_t &out); //argument is the array that will store the values.

    //Moving average functions
    uint16_t readChannelMovAvg(uint8_t n, uint8_t samples);

    void read16ChannelsMovAvgInto(uint16_t out[16], uint8_t samples);

    /**
     * Select a channel whose readings will be subtracted from all other readings
     * @param channel
     */
    void setDiffChannel(uint8_t channel);

    /**
     * Change the wait time between switching mux and sampling, in microseconds.
     * Default is 1microsecond, which should be enough for current multiplexers that have 600ns max settle time.
     * @param microseconds
     * @return the previous value in microseconds
     */
    uint32_t setSettleTime(uint32_t microseconds);

    /**
     * Set the number consecutive samples to be taken.
     * This is different from moving average sampling. In sample averaging, multiple samples are taken consecutively on
     * the same input (without changing channels), with {settleTime} microseconds apart between sampling, and the result
     * is averaged.
     * In moving average sampling, samples (which may be averaged!) are saved in a buffer, to allow averaging over
     * repeated calls.
     * @param numSamples number of samples to take repeatedly and average. Default is 2, should be kept low.
     */
    void setSampleAveraging(SampleAveraging numSamples);
};

#endif
