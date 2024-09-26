#include "bluefruit.h"
#include <Module16Inputs.h>

Module16Inputs module16inputs;
uint16_t adcValues[16];

unsigned long timestamp;
unsigned int diffChannel = 0;
SampleAveraging sampling_average = SINGLE_SAMPLING;

void setup() {
    module16inputs = Module16Inputs();
    Serial.begin(115200);
}

void readSettings() {
    switch (Serial.peek()) {
        case 's': // Fall-through
        case 'S':
            sampling_average = SINGLE_SAMPLING;
            module16inputs.setSampleAveraging(sampling_average);
            Serial.println("Setting single sample mode");
            Serial.read();
            return;
        case 'd':
        case 'D':
            sampling_average = DOUBLE_SAMPLING;
            module16inputs.setSampleAveraging(sampling_average);
            Serial.println("Setting double sample mode");
            Serial.read();
            return;
        case 'q':
        case 'Q':
            sampling_average = QUADRUPLE_SAMPLING;
            module16inputs.setSampleAveraging(sampling_average);
            Serial.println("Setting quadruple sample mode");
            Serial.read();
            return;
    }
    long readResult = Serial.parseInt(SKIP_WHITESPACE);
    if (readResult > 0) {
        diffChannel = readResult > 16 ? 0 : readResult;
        module16inputs.setDiffChannel(diffChannel);
        Serial.print("Setting difference channel to ");
        Serial.println(diffChannel);
    }
}

void loop() {
    while (!Serial) {
        delay(10);
    }
    if (millis() - timestamp < 50) {
        return;
    } else {
        timestamp = millis();
    }

    if (Serial.available()) {
        readSettings();
    }

    module16inputs.read16ChannelsMovAvgInto(adcValues, 10);
    for (int i = 0; i < 16; i++) {
        Serial.print(adcValues[i]);
        Serial.print("\t");
    }
    Serial.println();

    //To read a single channel, use the method .readChannel(#)
    //Reading the channels individually is more inefficient than reading
    //them all at once, because there are many more operations happening.
}
