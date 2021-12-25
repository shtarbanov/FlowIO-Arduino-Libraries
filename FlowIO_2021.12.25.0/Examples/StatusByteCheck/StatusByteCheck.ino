/*This example is to help us see if the status byte is working correctly in the different configs.
 */

#include <FlowIO.h>
FlowIO flowio;

void setup(){
  flowio = FlowIO(INFLATION_PARALLEL); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
  Serial.begin(115200);
  while(!Serial) delay(10);
  Serial.println("\n### --FlowIO Status Byte Test Initialized - ###");
  Serial.println("------");
}

void loop() {
  flowio.startRelease(0x01);      
  delay(1500);
  Serial.println(flowio.getHardwareStateOf(INLET));
  flowio.stopAction(0x01);
  delay(1500);
  Serial.println(flowio.getHardwareStateOf(INLET));  
}
