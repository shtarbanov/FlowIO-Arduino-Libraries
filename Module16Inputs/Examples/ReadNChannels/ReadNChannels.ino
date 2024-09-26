#include <bluefruit.h>
#include <Module16Inputs.h>

Module16Inputs module16inputs;
uint16_t adcValues[16];
uint8_t analogPin;
uint8_t ch = 1; //channel numbers are between 1 and 16
uint8_t n = ch;

uint8_t x = 26;
uint8_t s0 = 25;
uint8_t s1 = 24;

long requestedChannel = 1;

unsigned long timestamp;

void setup() {
    module16inputs = Module16Inputs();
    Serial.begin(115200);
}

void loop() {
    //Channels on pin A4: 3,4,7,8,11,12,15,16
    //Channels on pin A5: 1,2,5,6,9,10,13,14
//  ch=2;
//  module16inputs.setMuxesChannel(ch);
//  adcValues[ch-1] = analogRead(A5);
//
//
//  ch=5;
//  module16inputs.setMuxesChannel(ch);
//  adcValues[ch-1] = analogRead(A5);

    if (millis() - timestamp < 50) {
        return;
    } else {
        timestamp = millis();
    }

    if (Serial.available()) {
        long readResult = Serial.parseInt(SKIP_WHITESPACE);
        if (readResult > 0 && readResult <= 16) {
            requestedChannel = readResult;
            module16inputs.setMuxesChannel(requestedChannel);
        }
    }

    uint8_t n = requestedChannel;
    uint8_t analogPin = (n == 1 || n == 2 || n == 5 || n == 6 || n == 9 || n == 10 || n == 13 || n == 14) ? A5 : A4;
    uint16_t reading = analogRead(analogPin);
    Serial.print("channel ");
    Serial.print(requestedChannel);
    Serial.print(" = ");
    Serial.print(reading);
    Serial.println();

//    for (int i = 0; i < 16; i++) {
//        module16inputs.setMuxesChannel(i + 1);
//        uint8_t analogPin = ((i + 1) == 1 || (i + 1) == 2 || (i + 1) == 5 || (i + 1) == 6 || (i + 1) == 9 || (i + 1) == 10 || (i + 1) == 13 || (i + 1) == 14) ? A5 : A4;
//        uint16_t reading = analogRead(analogPin);
//        Serial.print(reading);
//        Serial.print("\t");
//    }
//    Serial.println();

    //To read a single channel, use the method .readChannel(#)
    //Reading the channels individually is more inefficient than reading
    //them all at once, because there are many more operations happening.
}
