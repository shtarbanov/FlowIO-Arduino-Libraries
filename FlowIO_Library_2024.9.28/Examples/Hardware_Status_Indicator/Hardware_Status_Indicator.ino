#include <Adafruit_TinyUSB.h> // for Serial
#include <FlowIO.h>
#define btnPin 7
#define batteryPin A6
#define AUTOOFFTIMER 5 //minutes of inactivity until auto-power-off.

FlowIO flowio;

void setup(){
  flowio = FlowIO(GENERAL);   
  pinMode(btnPin,INPUT_PULLUP);   
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
  Serial.begin(115200);
}

void loop() {
  //#############################################################
  //Recognize when USB cable is connected & disconnected
  static bool serialPortOpened=false;
  if(!serialPortOpened && Serial){ //If the serial port becomes active at any point after being inactive, reset the test and run init function.
    serialPortOpened=true;
  }else if(serialPortOpened && !Serial){ //if the Serial port was active and then the cable got unplugged:
    serialPortOpened=false; //no longer open
  }
  //#############################################################
  while(Serial.available() > 0) {
    char incomingByte = Serial.read();
    if(incomingByte=='H') flowio.blueLED(HIGH);
    else if(incomingByte=='L') flowio.blueLED(LOW);
    else if(incomingByte=='i') printDeviceInfo();
  } 
  //###############################################
  static uint8_t _holdPWM=150; //valve holding voltage.
  flowio.optimizePower(_holdPWM,200);
  autoPowerOff(AUTOOFFTIMER); //Although it's called every iteration, internally it runs once every 5 seconds. 
}