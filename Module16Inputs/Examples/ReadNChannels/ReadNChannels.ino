#include <Module16Inputs.h>
Module16Inputs module16inputs;
uint16_t adcValues[16];
uint8_t analogPin;
uint8_t ch=1; //channel numbers are between 1 and 16
uint8_t n=ch;

uint8_t x=26;
uint8_t s0=25;
uint8_t s1=24;

void setup() {
  module16inputs = Module16Inputs();
  Serial.begin(115200);
}

void loop() {
  //Channels on pin A4: 3,4,7,8,11,12,15,16
  //Channels on pin A5: 1,2,5,6,9,10,13,14
  ch=2;
  module16inputs.setMuxesChannel(ch);
  adcValues[ch-1] = analogRead(A5);

  
  ch=5;
  module16inputs.setMuxesChannel(ch);
  adcValues[ch-1] = analogRead(A5);
  
  

  
  for(int i=0; i<16; i++){
    Serial.print(adcValues[i]);
    Serial.print("\t");
  }
  Serial.println();

  //To read a single channel, use the method .readChannel(#)
  //Reading the channels individually is more inefficient than reading
  //them all at once, because there are many more operations happening.
}
