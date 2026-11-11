/*Working Example:
 *This example demonstrates how to properly read the pressure, and how to activate the sensor 
 *in such a way that the device still works after you press the reset button.
 */
#include <FlowIO.h>
FlowIO flowio;

void setup() {
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  Serial.begin(115200);
  Serial.println("Starting");
  flowio.pixel(1,1,1);
  while(flowio.activateSensor()==false){
    flowio.pixel(1,0,0);
    Serial.print("An Error: ");
    Serial.println(flowio.readError());
    delay(10);
  }
  flowio.pixel(1,1,1);
}

void loop() {
  if(flowio.readError()!=0){
    Serial.print("An Error: ");
    Serial.println(flowio.readError());
    delay(500);
  }
  else{
    Serial.println(flowio.getPressure(PSI));
    delay(500);
  }

}
