/*Press the button under the letter W to start a hardware test. When you first power up the system,
 * the neoPixel will start dancing. When you start a hardware test, the pixel will stop dancing and not
 * start dancing again until you return to mode 0, which happens after power cycling. If you are 
 * in any testing mode, the behavior of the pixel indicates the status of the test.
 * If there is a fixed light of some color - that indicates an error (color coded by error type).
 * If the pixel blinks 3 times in green, this means the test you just ran was successful.
 * If the pixel is off, it means it is waiting for you to run the next test.
 * If the pixel is dancing, the hardware tests have not been started yet. 
 * If the pixel blinks while changing color, that is the 20sec leak test.
 */

#include <Adafruit_TinyUSB.h> // for Serial
#include <FlowIO.h>
#define btnPin 7 //TODO: Place this as literal into the FlowIO library
#define batteryPin A6 //TODO: Place this as literal into the FlowIO library

FlowIO flowio;
//#############---Hardware Test Vars---#################
int testMode = 0;
int prevTestMode = 0;
bool dancingNeoPixel = true;
bool btnReading = 0;   // current digital reading of the button
bool btnState = 0;     // previous state of the button
unsigned long btnToggleTime = 0;
float p0 = 0; //starting pressure
float pinf=0; //inflation pressure
float pvac=0; //vacuum pressure
unsigned long pixelChangeTime = millis();
//#############---END: Hardware Test Vars---##############

void setup(){
  flowio = FlowIO(GENERAL); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  
  pinMode(btnPin,INPUT_PULLUP);   //This should go into the FlowIO library
  flowio.blueLED(HIGH);
}

void loop() {
  //#############---BEGIN: Hardware Test---#################
  if(dancingNeoPixel==true){
    if(millis() - pixelChangeTime > 300){
      flowio.pixel(random(0,6),random(0,6),random(0,6));
      pixelChangeTime = millis();          
    }
  }
  if(testMode != prevTestMode){ //Execute the following only if the testMode changes.
    //The code in each case below will execute ONLY ONCE, not continuously.
    switch(testMode){
      case 1: //SENSOR (Error = RED)
        dancingNeoPixel = false; //Note that this won't take effect until the NEXT iteration of the loop,
        flowio.pixel(0,0,0);      //hence I must also manually stop the pixel.
        sensorTest(); 
        break;
      case 2: //VALVES (Error = no sound)
        valveClickTest();
        break;
      case 3: //INFLATION (Error = YELLOW)
        inflationPumpTest();
        break;
      case 4: //VACUUM (Error = PURPLE)
        vacuumPumpTest();
        break;
      case 5: //BATTERY (Error = PINK)
        batteryConnectionTest();
        break;
      case 6: //PWM INFLATION (Error = sound)
        pwmInflationTest();
        break;
      case 7: //PWM VACUUM (Error = sound)
        pwmVacuumTest();
        break;
      case 8: //LEAK
        manualLeakTest();
        break;
      case 9: //POWER_OFF
        flowio.pixel(11,11,11);
        delay(100);
        flowio.powerOFF();
        break;
    }
    prevTestMode = testMode;
  }
  btnReading = digitalRead(btnPin);
  if(btnReading != btnState){ //if btnReading has changed.
    if(btnReading == LOW)  //and if it is now pressed.
      testMode += 1;
      if(testMode>9){ //this is only relevant in case the powerOFF() fails.
        testMode=0;
        dancingNeoPixel=true;
      }
      delay(50); //debounce
  }
  btnState = btnReading;  
  //#############---END: Hardware Test---#################
}

/* TODO: Integrate this code into the main firmware, and there is no need to force the user to remain in testing testMode to executing all tests.
 * If I press the button, I should enter the hardware testing loop and the control loop should only cycle between the hardware testing code. Any extraneous code should
 * not be part of the infinite loop. But maybe I shouldn't do that. If I want to do a hardware test, I may just want to run a one or two tests, and not having to be
 * forced to run the entire sequence of tests.
 */
