#include <Adafruit_TinyUSB.h> // for Serial
/* 

TODO: Add a small i2C OLED display module to show the results on the displa module directly. The display module can be connected to the expansion port, or I can create a simple 4-header cable that I solder 
  to the microcontroller pins on top and just have it be dangling of to the side. The latter won't require any new PCBs to be designed or modifications of the existing pcb, because the soldering will
  be to the top of the MCU board, not the driver board. Then I can use the OLED I already have from the current ranger to do the testing. 
TODO: I can write code to check if a USB power cable is connected by monitoring the battery voltage. There shold be different voltages when charging vs when discharging.
  And if the different is tiny, then I can exagerrate the difference by opening the valves to cause a voltage drop if only the battery is providing all the power. The analogRead(batteryPin)
  yields a very tiny difference between connected (596) and unconnected (594) and these values can fluctuate plus minus 3, so this alone is not a reliable indicator to tell that a cable
  has just gotten connected / disconnected. It would be easier to tell if a cable is currently connected or not than to detect the moment when that happens.
  With 1 valve actuated, the return voltage valuves for cable connected and disconnected are, respectively (595) and (584). There is a very clear difference even with just 1 open valve.
  However, these values are not entirely true, because I don't know yet how much time I need to wait before reading the voltage, so I must redo this test afer having done the time test.
  The code I used for this test is:
  if(!serialPortOpened && Serial){
    serialPortOpened=true;
    if(!initHwTestDone) {initHwTestDone=true; initHardwareTest();}
    Serial.print("Next test: #");
    Serial.println(testMode+1);
    flowio.openInletValve();
    delay(100);
    testData[0] = analogRead(batteryPin);
    flowio.closeInletValve();
    Serial.print(testData[0]);
    Serial.print(", ");
    Serial.print(testData[1]);
  }
  else if(serialPortOpened && !Serial){
    serialPortOpened=false; //no longer open
    flowio.openInletValve();
    delay(100);
    testData[1] = analogRead(batteryPin);
    flowio.closeInletValve();
  }

TODO: Use the USB serial cable connect/disconnect event listner in the if-else block below as my input method for running an automated SelfTest. The only thing that a user would
  have to do is simply connect and disconnect the USB cable when the instructions tell him to do that from the Serial monitor. They won't have to type or press any buttons.

TODO: valveDetectionTest() - fully automated test to check whethre valves are connected. Thus must be run with the USB cable disconnected to check for voltage drops when valves are on.
  This function is already implemented. Only some user prompts are needed to tell the user that they need to disconnect the USB cable and reconnect it afterwards.

    TODO: First, investigate via millis() and analogRead() how long it takes for the battery voltage to drop after a valve is open, and to recover after a valve is closed.
    This is simply going to be helpful to determine how much delay to provide between the operations and the minimum time needed to run the valve detection test.

TODO: valveIntegrityTest() - after detecting the presense of the valves, the next test would be to check if they are opening and closing properly. This can be done using the 
  pressure sensor and the two pumps. Open one port at a time and check if there is difference in the pressure when the port is opened and when closed. Then at the end, report which 
  port is problematic if any. You will not have to count clicks then, and then the full test will be automated.You would have to already know that your pumps are working. 
  And to check that your pumps are working you have to have working ports inlet and outlet. So you could do your pump tests first, which will also affirm that your inlet and outlet valves are ok. 
  And then you will only be left with testing the 5 ports afterwards.

TODO: The battery tetection test is flawed and should be done while the cable is disconnected. Change the test prompts to ask the user to disconnect the cable before running the test.
  and to reconnect it afterwards. When the cable is reconnected, the test result (success) should be displayed on the serial monitor.
  
TODO: If sensor detection test failed or is not executed, other tests that require the sensor should not be allowed.

TODO: Add a blockage test. Open both side valves and the pressure should not increase past a max value.

TODO: Add a configuration detection test. This can even be done in the FlowIO library itself.

 */

/*This is the hardware testng firmware used for ensuring that the device works as expected.
 * Open your serial monitor to read the instructions for how to run each test, and the 
 * outcomes and results. You need to have access to the UserSW button for this test. 
 * Follow the instructions on the Serial Monitor.
 */
#include <FlowIO.h>
#define btnPin 7
#define batteryPin A6

//#######################################

#define AUTOOFFTIMER 5 //minutes of inactivity until auto-power-off.

//#######################################

FlowIO flowio;
int testMode = -1; //this will be set to 0
int prevTestMode = -1; //making this different so we enter the state machine on first start.
bool repeatTest = false;



//The following variables are just for the printTestResult() function. They don't have real importance.
//Saving the results is useful in the case of the valve detection function where we have to disconnect the USB cable during the test
//but we may wish to view the test result after reconnecting the cable.
int testData[8] = {0}; //this stores values such as pressures or voltages
bool testResults[8] = {0}; //this stores success or failure results

float p0 = 0;
float pinf=0;
float pvac=0;

void setup(){
  flowio = FlowIO(GENERAL); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  
  pinMode(btnPin,INPUT_PULLUP);   
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
  Serial.begin(115200);
  // while(!Serial) delay(10); //"Serial" returns true whenever we press the serial monitor button.
  /*This means that we will be stuck in this infinite loop UNTIL WE OPEN THE SERIAL MONITOR!!! If usb cable is already connected, it still takes 1.7seconds*/
}

void loop() {
  //#############################################################
  //The following if-else block is extermely profound, because it allows me to recognize when USB cable is connected & disconnected, as well as the ability to execute code on each of those events.
  //This only works though if Arduino is open and configured to look for Serial connections from the FlowIO board, not otherwise. Nevertheless, this now serves as a whole new input method for 
  //controlling FlowIO. NOTE: Even though I can execute code on both connect and disconnect, the serial monitor can only display the things executed on connect but not on disconnect.
  static bool serialPortOpened=false;
  static bool initHwTestDone = false;  
  if(!serialPortOpened && Serial){ //If the serial port becomes active at any point after being inactive, reset the test and run init function.
    serialPortOpened=true;
    if(!initHwTestDone) {initHwTestDone=true; initHardwareTest();}
    Serial.print("Next test: #");
    Serial.println(testMode+1);
  }else if(serialPortOpened && !Serial){ //if the Serial port was active and then the cable got unplugged:
    serialPortOpened=false; //no longer open
  }
  //#############################################################

  if(repeatTest==true){
    prevTestMode=-1; //reset the prevMode variable to assume that the mode has changed
    repeatTest = false;
  }
  if(testMode != prevTestMode){ //Only execute this code if the mode has changed.
    switch(testMode){
      //0. Sensor test
      case 0:
        flowio.blueLED(1);
        sensorTest();
        break;
      //1. Valve Click Test
      case 1:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        valveClickTest();
        break;
      //2. Battery test
      case 2:
        flowio.blueLED(1);
        delay(500);
        batteryConnectionTest();
        break;
      //3. Inflation Pump test
      case 3:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        inflationPumpTest();
        break;
      //4. Vacuum pump test
      case 4:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        vacuumPumpTest();
        break;

      //5. PWM Inflation test
      case 5:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        pwmInflationTest();
        break;
      //6. PWM Vacuum test
      case 6:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        pwmVacuumTest();
        Serial.println("------\nNext Test: 'Manually Check for sensor leaks'");
        Serial.println("Before running this test, disconnect the pump module and connect a syringe to the");
        Serial.println("inelt valve (rightmost). When the test starts, the inlet valve will open and stay");
        Serial.println("open for 20 seconds. During that time, try pushing air with the syringe into the");
        Serial.println("inlet valve to check if there are any leaks. Type 5 to begin this test."); 
        break;
      //7. Leak test
      case 7:
        flowio.blueLED(1);
        delay(500);
        manualLeakTest();
        break;
      //8. Power off test
      case 8:
        Serial.println("8. Powering OFF......\t\t\tdone\n\n");
        flowio.powerOFF();
        break;
    }
    if(repeatTest) repeatTest=false;
    prevTestMode = testMode;
  }
  static bool btnState = 0;         // current state of the button
  static bool prevbtnState = 0;     // previous state of the button
  btnState = digitalRead(btnPin);
  if(btnState != prevbtnState){ //if btnState has changed.
    if(btnState == LOW)  //and if it is now pressed.
      testMode += 1;
      if(testMode>7) testMode=0;
      delay(50); //debounce
  }
  prevbtnState = btnState; 
  //NOTE: If I enter a character into serial monitor and press enter, this will result in two bytes! The first 
  //byte will be the character entered, and the second will be LineFeed (Decimal 10). If I just click the send
  //button without entereing anything, the LineFeed will be received!
  while(Serial.available() > 0) {
    // read the incoming byte and set the test mode accordingly:
    char incomingByte = Serial.read();
    if(incomingByte=='r') repeatTest=true;
    else if(incomingByte=='n') testMode++;
    else if(incomingByte=='p') printTestResult();
    else if(incomingByte-'0' >= 0 && incomingByte-'0'<= 8){ //if it is an allowed mode
      testMode = incomingByte-'0'; //subtract ascii 0 to get the number the user wants to execute.
      prevTestMode=-1; //reset the prevMode variable to assume that the mode has changed even if it's the same as the previous.
    }
    else if(incomingByte=='i'){ //show information
      printDeviceInfo();
    }
  } 
  autoPowerOff(AUTOOFFTIMER); //Although it's called every iteration, internally it runs once every 5 seconds.
}