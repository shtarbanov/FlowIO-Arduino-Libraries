#include <Module16Inputs.h>
#include <math.h>

Module16Inputs module16inputs;
uint16_t adcValues[16];
float pressureValues[16]; //kPa

void setup() {
  //analogReference(AR_INTERNAL_1_2);
  analogReadResolution(12); //0...4095. Can be up to 14-bit.
  module16inputs = Module16Inputs();
  Serial.begin(115200);
}

void loop() {
  module16inputs.read16ChannelsInto(adcValues);
//  module16inputs.read16ChannelsMovAvgInto(adcValues, 10);

  for(int i=0; i<16; i++){
    pressureValues[i] = 200 -75 * log(74.8 / (4.9 + 5728.6 * pow(0.9965,adcValues[i])) -1);
    Serial.print(adcValues[i]);
//    Serial.print(pressureValues[i]);
    Serial.print("\t");
  }
  Serial.println();

  //To read a single channel, use the method .readChannel(#)
  //Reading the channels individually is more inefficient than reading
  //them all at once, because there are many more operations happening.
}
