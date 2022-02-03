#include <FlowIO.h>
#define btnPin 7

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
bool activeFlag = 0;

FlowIO flowio;
int mode = 0;
int prevMode = -1; //making this different so we enter the state machine on first start.

void setup(){
  pinMode(btnPin,INPUT_PULLUP); 
  
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  Serial.begin(115200);
  while(!Serial) delay(10); //"Serial" returns true whenever we press the serial monitor button.
  //At all other times, this is false because the port is closed.
  //we can use if(Serial) anywhere in our code to check if the serial monitor is open and do an action when it is.
  Serial.println("\n### --FlowIO Sensor Leak Test - ###");
  Serial.println("------");
  Serial.println("Type 1 to openInletValve(), 0 to closeInletValve()");
  Serial.println("------");
  flowio.pixel(1,1,1);

}

void loop() {
  if(mode != prevMode){ //Only execute this code if the mode has changed.
    switch(mode){
      case 0: //we come here when we first start the system
        flowio.blueLED(1);
        flowio.closeInletValve();
        break;
      case 1:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        flowio.openInletValve();
        break;
    }
    prevMode = mode;
  }
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW)  //and if it is now pressed.
      activeFlag = !activeFlag;
      if(activeFlag){
        flowio.openInletValve();
        flowio.pixel(10,0,0);
      }
      else{
        flowio.closeInletValve();
        flowio.pixel(1,1,1);
      }
  }
  prevButtonState = buttonState; 
  while(Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();
    if(incomingByte-'0' >= 0 && incomingByte-'0'<= 1){ //if it is an allowed mode
      mode = incomingByte-'0'; //subtract ascii 0 to get the number the user wants to execute.
      Serial.println(mode);
      Serial.print("Inlet Valve is: ");
      Serial.println(incomingByte - '0');
    }
  }
}
