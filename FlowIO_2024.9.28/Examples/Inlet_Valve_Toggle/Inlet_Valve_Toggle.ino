/* Toggle the Inlet valve (with power saving) either 
  - by pressing the button
  - or by entering 0 or 1 in the serial monitor
On startup, the program starts in Mode 0 and activates the pressure sensor
The LED changes color to indicate the mode.
The pressure is reporter once every second when in mode 1.
If mode 1 has been active for 1 minute, it automatically switches to mode 0.
The device turns off after 4 minutes of inactivity
 */

#include <FlowIO.h>
#define btnPin 7

FlowIO flowio;

uint8_t countDisplayP = 0; //counts the number of times the pressure value was displayed (used only for visual purposes)

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
    while(flowio.activateSensor()==false){
      if(serialPortOpened) Serial.print(F("Sensor detection...FAILED...retrying...\n"));
      delay(10);
    }
    if(serialPortOpened) Serial.println(F("Sensor detection...SUCCESS"));
    Serial.println(F("Type 1 to openInletValve(), 0 to closeInletValve()"));
    Serial.println("------");
  }else if(serialPortOpened && !Serial){ //if the Serial port was active and then the cable got unplugged:
    serialPortOpened=false; //no longer open
  }
  //#############################################################

  static bool buttonState = 1;         // current state of the button
  static bool prevButtonState = 1;     // previous state of the button
  static int mode = 0;
  static int prevMode = -1; //making this different so we enter the state machine on first start.

  if(mode != prevMode){ //Only execute this code if the mode has changed.
    switch(mode){
      case 0: //we come here when we first start the system
        flowio.closeInletValve();
        if(serialPortOpened) Serial.println(F("\nSet to Mode 0")); //this will not print at startup, because it takes time for port to open
        flowio.pixel(1,1,1);
        break;
      case 1:
        flowio.openInletValve();
        if(serialPortOpened) Serial.println(F("Set to Mode 1"));
        countDisplayP = 0; //reset the pressure display counter
        flowio.pixel(1,0,0); //
        break;
    }
    prevMode = mode;
  }
  else if (mode == 1){ //show the pressure every second
    showPressureEvery(1);
    if(flowio.getHardwareStateOf(INLET)){
      if(millis()-flowio.getStartTimeOf(INLET) > 60*1000) mode=0;
    }
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
      if(mode == prevMode){
        if(mode==1) Serial.println();
        Serial.print(F("You're already in mode "));
        Serial.println(mode);
      }
    }
  }
  //###############################################
  
  static uint8_t _holdPWM=150; //valve holding voltage.
  static uint8_t AUTOOFFTIMER = 4;
  flowio.optimizePower(_holdPWM,200);
  autoPowerOff(AUTOOFFTIMER); //Although it's called every iteration, internally it runs once every 5 seconds.
}

void showPressureEvery(uint8_t T){
  static float remainingTimeP = 0;
  static uint32_t timelastcheckedP = millis();
  if(millis()-timelastcheckedP < T*1000) return;
  else timelastcheckedP = millis();
  
  Serial.print("\t");
  if(countDisplayP == 12){
    Serial.print("\n\t");
    countDisplayP=0;
  }
  countDisplayP++;
  Serial.print(flowio.getPressure(PSI));
  // Serial.println(flowio.getStartTimeOf(INLET));
}
