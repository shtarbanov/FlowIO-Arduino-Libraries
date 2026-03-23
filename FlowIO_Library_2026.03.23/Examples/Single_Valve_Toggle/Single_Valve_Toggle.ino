/*This example toggles the inlet valve state. When you start the program the system is idle.
 *When you press the UserSW button, the inlet valve starts toggling at the value in T_TOGGLE.
 *
 * 
 */

#include <FlowIO.h>
#define btnPin 7
#define T_TOGGLE 1000 //toggle time (ms)

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
int lastTime = millis();
bool activeFlag = false;
bool isValveOff = true;

FlowIO flowio;

void setup(){
  pinMode(btnPin,INPUT_PULLUP);   
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
  Serial.begin(115200);
}

void loop() {
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW){  //and if it is now pressed.
      activeFlag = !activeFlag; //toggle the flag status;
    }
  }
  prevButtonState = buttonState; 
  
  if(activeFlag){
    toggleInletValveEvery(T_TOGGLE);
  }
  else{
    if(isValveOff==false){
      flowio.closeInletValve();
      flowio.pixel(1,1,1);
      isValveOff = true;  
      Serial.println("Close");
    }
  }
}

void toggleInletValveEvery(int interval){
  if(activeFlag){ //if we are in the toggle regime.
    if(millis() - lastTime > interval){ //we wiil check the battery only once per 5 seconds. This reduces power consumption greatly.
        Serial.print("toggle: ");
        lastTime = millis();
        if(isValveOff==true){
          flowio.openInletValve();
          flowio.pixel(10,0,0);
          isValveOff=false;
          Serial.println("Open");
        }
        else{
          flowio.closeInletValve();
          flowio.pixel(1,1,1);
          isValveOff = true;  
          Serial.println("Close");
        }
    }
  }
}
