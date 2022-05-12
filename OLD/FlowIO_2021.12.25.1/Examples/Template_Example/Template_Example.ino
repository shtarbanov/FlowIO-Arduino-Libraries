#include <FlowIO.h>
FlowIO flowio;

void setup() {
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  Serial.begin(115200);
  Serial.println("Starting");
  flowio.pixel(1,1,1);
}

void loop() {
  // put your main code here, to run repeatedly:

}
