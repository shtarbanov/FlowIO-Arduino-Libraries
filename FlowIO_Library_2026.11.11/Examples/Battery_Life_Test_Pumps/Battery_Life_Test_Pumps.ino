#include <FlowIO.h>

FlowIO flowio;

void setup(){
  flowio = FlowIO(GENERAL); 
  flowio.pixel(1,1,1);
  Serial.begin(115200);
  // flowio.startPump(2,255); //this puts pump1 out of phase with pump2.
  // flowio.startPump(2,255);
  // flowio.startPump(1,255);
}

void loop() {
  pumpToggle(3000); 
}