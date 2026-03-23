/*
 * In this example, we will inflate port 1 for 3 seconds and then stop. And we will do it in a
 * nonblocking way by takind advantage of the built-in start action timers.
 */
#include <FlowIO.h>
FlowIO flowio;

uint8_t counter = 1;

void setup(){
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  Serial.begin(115200);
  flowio.pixel(1,1,1);
  flowio.startInflation(0x01);
}

void loop() {
  if(millis() - flowio.getStartTimeOf(INLET) > 3000){
    flowio.stopAction(0b000000100);
  }
}
