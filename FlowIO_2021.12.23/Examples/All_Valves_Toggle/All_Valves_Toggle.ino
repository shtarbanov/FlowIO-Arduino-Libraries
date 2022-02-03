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
}

void loop() {
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW)  //and if it is now pressed.
      activeFlag = !activeFlag;
      if(activeFlag){
        openAllValves();
        flowio.pixel(10,0,0);
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
