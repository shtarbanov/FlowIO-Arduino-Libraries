#include <FlowIO.h>
#define btnPin 7

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
bool activeFlag = 0;

uint8_t _holdPWM=150; //valve holding voltage. We must maintain the holding voltage at 1.3V. But as the battery
  //voltage drops, so does the voltage at the valve. Thus, we must increase the PWM value to counteract the drop
  //in battery voltage. Hence, the battery service updates the value periodically. 
  //This variable is used in the argument of the .optimizePower() function below.

FlowIO flowio;

void setup(){
  pinMode(btnPin,INPUT_PULLUP); 
  
  flowio = FlowIO(GENERAL); 
  // flowio.pixel(1,1,1);
  Serial.begin(115200);
}

void loop() {
  // static int testMode = 0;
  // static int prevTestMode = -1;
  // static bool valveToggleTestActive = false;

  // //#############################################################
  // //The following if-else block is extermely profound, because it allows me to recognize when USB cable is connected & disconnected, as well as the ability to execute code on each of those events.
  // //This only works though if Arduino is open and configured to look for Serial connections from the FlowIO board, not otherwise. Nevertheless, this now serves as a whole new input method for 
  // //controlling FlowIO. NOTE: Even though I can execute code on both connect and disconnect, the serial monitor can only display the things executed on connect but not on disconnect.
  // static bool serialPortOpened=false;
  // static bool initTestDone = false;  
  // if(!serialPortOpened && Serial){ //If the serial port becomes active at any point after being inactive, reset the test and run init function.
  //   serialPortOpened=true;
  //   Serial.println(F("\n### --Nonblocking Valve Toggle - ###"));
  //   Serial.println(F("------------------------------"));
  //   Serial.println(F("---> Input commands: <---"));
  //   Serial.println(F("0 - Stop Valve Toggle."));
  //   Serial.println(F("1 - Start Valve Toggle."));
  // }else if(serialPortOpened && !Serial){ //if the Serial port was active and then the cable got unplugged:
  //   serialPortOpened=false; //no longer open
  // }
  // //#############################################################

  // if(serialPortOpened){
  //   if(Serial.available()){
  //     // read the incoming byte and set the test mode accordingly:
  //     char incomingByte = Serial.read();
  //     if(incomingByte-'0' >= 0 && incomingByte-'0'<= 1){ //if it is an allowed mode
  //       testMode = incomingByte-'0'; //subtract ascii 0 to get the number the user wants to execute.
  //       prevTestMode=-1; //reset the prevMode variable to assume that the mode has changed even if it's the same as the previous.
  //       Serial.print("Test: ");
  //       Serial.println(testMode);        
  //     }    
  //   }
  // }
  // if(testMode != prevTestMode){ //Only execute this code if the mode has changed.
  //   switch(testMode){
  //     case 0: //do this when you first start the system if testMode is initialized to 0.
  //       flowio.blueLED(1);
  //       valveToggleTestActive = false;
  //       break;
  //     case 1:
  //       flowio.blueLED(1);
  //       valveToggleTestActive = true;
  //       break;     
  //   }
  //   prevTestMode = testMode;
  // }

  // buttonState = digitalRead(btnPin);
  // if(buttonState != prevButtonState){ //if buttonstate has changed.
  //   if(buttonState == HIGH)  //and if it is now RELEASED.
  //     valveToggleTestActive = !valveToggleTestActive; //test has been started
  // }
  // prevButtonState = buttonState;

  // //If the flag valveToggleTestActive is TRUE, then then toggle the valves
  // if(valveToggleTestActive) valveToggle(3000); 
  valveToggle(3000); 
  flowio.optimizePower(_holdPWM,200); //second argument can be fine tuned better to be lower.

}