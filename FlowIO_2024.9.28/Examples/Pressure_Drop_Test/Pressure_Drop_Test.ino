#include <Adafruit_TinyUSB.h> // for Serial

/*This is testing how the internal pressure of the device drops due to leaks from the port valves. 
This test is based on the Leak Test, and it outputs "Time" & "Pressure" every second for 3 minutes.
To run the test, connect a syringe on the Inlet port and start the test to open the inlet valve only.

Commands available:
0 - pressure sensor test
1 - Device leakake / pressure drop test
a - abort the pressure drop test

Button actions (User SW btn):
press - go to the next test; 
long press - abort current test

 */

//#######################################

#define AUTOOFFTIMER 2 //minutes of inactivity until auto-power-off.

//#######################################

#include <FlowIO.h>
#define btnPin 7

FlowIO flowio;
int test = 0;
int prev_test = -1; //making this different so we enter the state machine on first start.

bool btnState = 0;         // current state of the button
bool prev_btnState = 0;     // previous state of the button
float p0 = 0;
float pinf=0;
float pvac=0;
uint32_t t0;
uint32_t t;

void setup(){
  flowio = FlowIO(GENERAL); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  
  pinMode(btnPin,INPUT_PULLUP);   
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
  Serial.begin(115200);
  while(!Serial) delay(10); //"Serial" returns true whenever we press the serial monitor button.
  /*This means that we will be stuck in this infinite loop UNTIL WE OPEN THE SERIAL MONITOR!!! */
  //TODO: See the SelfTest2024 for how to eliminate and fix this infinite loop problem.

  Serial.println("\n### --FlowIO Pressure Drop Test Initialized - ###");
  Serial.println("------");
  Serial.println("0. Sensor Detection test");
  Serial.println("1. Pressure Drop test");
  flowio.pixel(1,1,1);
}

void loop() {
  if(test != prev_test){ //Only execute this code if the test has changed.
    switch(test){
      case 0:
        flowio.blueLED(1);
        sensorTest();
        Serial.println("------\nType 1 or press UserSW button for: 'Pressure Drop test'");
        break;
      case 1:
        flowio.blueLED(1);
        delay(500);
        manualLeakTest();
        break;
    }
    prev_test = test;
  }
  //NOTE: If I enter a character into serial monitor and press enter, this will result in two bytes! The first 
  //byte will be the character entered, and the second will be LineFeed (Decimal 10). If I just click the send
  //button without entereing anything, the LineFeed will still be received!
  while(Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();
    if(incomingByte-'0' >= 0 && incomingByte-'0'<= 1){ //if it is allowed test number
      test = incomingByte-'0'; //subtract ascii 0 to get the number the user wants to execute.
      Serial.println(test);
      Serial.print("Starting test #");
      Serial.println(incomingByte - '0');
      if(prev_test == test) prev_test = 99; //this allows us to repeat the test by typing the same test tumber.
    }
  } 
  //################---Button Control---##########################
  btnState = digitalRead(btnPin);
  if(btnState != prev_btnState){ //if btnState has changed.
    if(btnState == LOW)  //and if it is now pressed.
      test += 1;
      if(test>1) test=0;
      delay(50); //debounce
  }
  prev_btnState = btnState; 
  //################---Button Control End---##########################
  autoPowerOff(AUTOOFFTIMER); //Although it's called every iteration, internally it runs once every 5 seconds.
}

//############################--Function Definitions--##################################
//######################################################################################
void sensorTest(){
  flowio.pixel(0,1,0);
  //To get the correct pressure reading, we will open and close two ports first
  flowio.openPorts(0b00000011);
  delay(100);
  flowio.stopAction(0xFF);
  //Serial.println("Sensor Test in Progress....");
  while(flowio.activateSensor()==false){
    Serial.print("\n--->  0. Sensor Detection Test......FAILED. Error = ");
    Serial.println(flowio.readError());
    flowio.pixel(1,0,0);
    delay(10);
  }
  flowio.pixel(1,1,1);
  Serial.print("\n--->  0. Sensor Detection Test......Success! (*_*) ");
  Serial.print("P = ");
  p0=flowio.getPressure(PSI);
  Serial.println(p0);
  flowio.pixel(1,1,1);
}

void manualLeakTest(){
  flowio.pixel(0,1,0);
  flowio.openInletValve();
  flowio.setPorts(0b00000000);
  Serial.println("Manual Leak Test in Progress...");
  Serial.println("You have 60 seconds to check for leaks on the inlet valve.");
  Serial.print("Seconds remaining: ");
  t0 = millis();
  uint32_t testDuration = 3*60*1000; //2 minutes in milliseconds
  while(millis()-t0 < testDuration){
    Serial.print(millis()-t0);
    Serial.print(",\t");
    pinf=flowio.getPressure(PSI);
    Serial.println(pinf);
    testDelay(1000);
    //---------------------------- 
    //Check for Abort 
    //via Serial:
    while(Serial.available() > 0) {
      char incomingByte = Serial.read();
      if(incomingByte == 'a'){ //abort
        Serial.println("Aborting");
        testDuration = 0; //this will break out of the test
      }
    }
    //via Button:
    btnState = digitalRead(btnPin);
    if(btnState == LOW){ //if buttonstate has changed AND is now pressed.
      Serial.println("Aborting");
      testDuration = 0; //this will break out of the test
    }
    // prevButtonState = buttonState; 
    //---------------------------- 
  }
  flowio.closeInletValve();
  Serial.println("\nLeak Test completed.");
  flowio.pixel(1,1,1);
}

void testDelay(int time){
    flowio.pixel(0,0,1);
    delay(0.9*time);
    flowio.pixel(0,1,0);
    delay(0.1*time);
}
