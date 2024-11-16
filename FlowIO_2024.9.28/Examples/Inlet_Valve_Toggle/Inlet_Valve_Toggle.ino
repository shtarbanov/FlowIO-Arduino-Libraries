/* Toggle the Inlet valve either 
  - by pressing the button
  - or by entering 0 or 1 in the serial monitor
On startup, the program starts in Mode 0.
The LED changes color to indicate the mode. */

#include <FlowIO.h>
#define btnPin 7

FlowIO flowio;
void setup(){
  pinMode(btnPin,INPUT_PULLUP); 
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  Serial.begin(115200);
}
void loop() {
  //#############################################################
  //The following if-else block is extermely profound, because it allows me to recognize when USB cable is connected & disconnected, as well as the ability to execute code on each of those events.
  //This only works though if Arduino is open and configured to look for Serial connections from the FlowIO board, not otherwise. Nevertheless, this now serves as a whole new input method for 
  //controlling FlowIO. NOTE: Even though I can execute code on both connect and disconnect, the serial monitor can only display the things executed on connect but not on disconnect.
  static bool serialPortOpened=false;
  if(!serialPortOpened && Serial){ //If the serial port becomes active at any point after being inactive, reset the test and run init function.
    serialPortOpened=true;
    Serial.println("Type 1 to openInletValve(), 0 to closeInletValve()");
    Serial.println("------");
  }else if(serialPortOpened && !Serial){ //if the Serial port was active and then the cable got unplugged:
    serialPortOpened=false; //no longer open
    Serial.println("Port closed");
  }
  //#############################################################

  static bool buttonState = 1;         // current state of the button
  static bool prevButtonState = 1;     // previous state of the button
  static int mode = 0;
  static int prevMode = -1; //making this different so we enter the state machine on first start.

  if(mode != prevMode){ //Only execute this code if the mode has changed.
    switch(mode){
      case 0: //we come here when we first start the system
        flowio.blueLED(1);
        flowio.closeInletValve();
        if(serialPortOpened) Serial.println("Set to Mode 0");
        flowio.pixel(1,1,1);
        break;
      case 1:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        flowio.openInletValve();
        if(serialPortOpened) Serial.println("Set to Mode 1");
        flowio.pixel(1,0,0); //
        break;
    }
    prevMode = mode;
  }

  //#########---Change mode from Button---############
  buttonState = digitalRead(btnPin); //this return 1 when not pressed.
  if(buttonState==0){ //if the button is pressed
    if(buttonState != prevButtonState){ //and if is just became pressed (rather than being held down)
      if(mode==1) mode = 0;
      else mode=1;
    }
  }
  prevButtonState = buttonState;  

  //#########---Change mode from Serial---############
  while(Serial.available() > 0) {
    char incomingByte = Serial.read();
    if(incomingByte-'0' >= 0 && incomingByte-'0'<= 1){ //if it is an allowed mode
      mode = incomingByte-'0'; //subtract ascii 0 to get the number the user wants to execute.
      Serial.print("You entered: ");
      Serial.println(mode);
    }
  }
  //###############################################
}
