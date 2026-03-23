#include <FlowIO.h>
#define btnPin 7

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
bool activeFlag = 0;

FlowIO flowio;

void setup(){
  pinMode(btnPin,INPUT_PULLUP); 
  
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
//  Serial.begin(115200);
//  while(!Serial) delay(10);
}

void loop() {
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
//    Serial.println("button state changed");
    if(buttonState == HIGH)  //and if it is now RELEASED.
//    Serial.println("button state LOW");
      activeFlag = !activeFlag; //test has been started
//      Serial.println("activeFlag");
      if(activeFlag){
//        Serial.print("Active Flag: ");
//        Serial.println("activeFlag");
        valveClickTest();
      }
      else{
        closeAllValves();
        flowio.pixel(1,1,1);
      }
  }
  prevButtonState = buttonState; 
}

void openAllValves(){
  flowio.openInletValve();
  flowio.openOutletValve();
  flowio.setPorts(0b00011111);
}
void closeAllValves(){
  flowio.closeInletValve();
  flowio.closeOutletValve();
  flowio.setPorts(0b00000000);
}
void valveClickTest(){
  flowio.pixel(0,0,10);
  Serial.print("Valve Click Test in Progress....");
  Serial.print("out..");
  flowio.pixel(10,0,0);
  flowio.openOutletValve();
  delay(500);
  flowio.pixel(0,1,0);
  flowio.closeOutletValve();
  delay(1000);
  Serial.print("p1..");
  flowio.pixel(10,0,0);
  flowio.setPorts(0b00000001);
  delay(500);
  flowio.pixel(0,1,0);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p2..");
  flowio.pixel(10,0,0);
  flowio.setPorts(0b00000010);
  delay(500);
  flowio.pixel(0,1,0);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p3..");
  flowio.pixel(10,0,0);
  flowio.setPorts(0b00000100);
  delay(500);
  flowio.pixel(0,1,0);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p4..");
  flowio.pixel(10,0,0);
  flowio.setPorts(0b00001000);
  delay(500);
  flowio.pixel(0,1,0);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p5..");
  flowio.pixel(10,0,0);
  flowio.setPorts(0b00010000);
  delay(500);
  flowio.pixel(0,1,0);
  flowio.setPorts(0b00000000);
  delay(1000); 
  Serial.println("in..");
  flowio.pixel(10,0,0);
  flowio.openInletValve();
  delay(500);
  flowio.closeInletValve();
  flowio.pixel(0,1,0);
  delay(1000);
  flowio.pixel(1,1,1);
}
