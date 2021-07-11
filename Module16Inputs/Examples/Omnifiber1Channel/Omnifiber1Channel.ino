#include <Module16Inputs.h>
#include <math.h>
Module16Inputs module16inputs;
uint16_t adcValues[16];
uint8_t channelNumber=1; //between 1 and 16
float pressureValues[16]; //kPa


void setup() {
  //analogReference(AR_INTERNAL_1_2);
  analogReadResolution(12); //0...4095. Can be up to 14-bit.
  module16inputs = Module16Inputs();
  Serial.begin(115200);
}

void loop() {
  adcValues[0] = module16inputs.readChannel(channelNumber);
//  adcValues[0] = module16inputs.readChannelMovAvg(channelNumber,10);
  
  pressureValues[0] = 200 -75 * log(74.8 / (4.9 + 5728.6 * pow(0.9965,adcValues[0])) -1);

  Serial.print(adcValues[0]);
  Serial.print("\t");
  Serial.print(pressureValues[0]);
  Serial.println();

  //To read a single channel, use the method .readChannel(#)
  //Reading the channels individually is more inefficient than reading
  //them all at once, because there are many more operations happening.
}
