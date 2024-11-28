#include <Module16Inputs.h>
Module16Inputs module16inputs;
uint16_t adcValues[16];
uint8_t channelNumber=1; //between 1 and 16

void setup() {
  analogReference(AR_INTERNAL_3_0); //https://learn.adafruit.com/bluefruit-nrf52-feather-learning-guide/nrf52-adc
  analogReadResolution(12);   // Set the resolution to 12-bit (0..4095). Default is 10-bit. Can be 8, 10, 12 or 14
  module16inputs = Module16Inputs();
  Serial.begin(115200);
}

void loop() {
  adcValues[channelNumber-1] = module16inputs.readChannel(channelNumber);
  for(int i=0; i<16; i++){
    Serial.print(adcValues[i]);
    Serial.print("\t");
  }
  Serial.println();

  //To read a single channel, use the method .readChannel(#)
  //Reading the channels individually is more inefficient than reading
  //them all at once, because there are many more operations happening.
}
