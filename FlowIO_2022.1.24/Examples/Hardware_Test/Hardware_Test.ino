/* TODO: Have test that autochecks all the I/O ports as well. You can open one port at a time and check if there is 
 * difference in the pressure when the port is opened and when closed. Then at the end, you can report exactly which 
 * port is problematic if any. You will not have to count clicks then, and then the full test will be automated.
 * 
 * TODO: Battery test should be done right after sensor test.
 * 
 * TODO: If any test fails, the subsequent tests should not be allowed.
 */

/*This is the hardware testng firmware used for ensuring that the device works as expected.
 * Open your serial monitor to read the instructions for how to run each test, and the 
 * outcomes and results. You need to have access to the UserSW button for this test. 
 * Follow the instructions on the Serial Monitor.
 */
 //TODO: Add a blockage test. Open both side valves and the pressure should not increase past a max value.
#include <FlowIO.h>
#define btnPin 7
#define batteryPin A6

FlowIO flowio;
int mode = 0;
int prevMode = -1; //making this different so we enter the state machine on first start.
bool repeatTest = false;


bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
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
  while(!Serial) delay(10); //"Serial" returns true whenever we press the serial monitor button.
  /*This means that we will be stull in this infinite loop UNTIL WE OPEN THE SERIAL MONITOR!!! */
  Serial.println("\n### --FlowIO Self Test Initialized - ###");
  Serial.println("------");
  Serial.println("0. Sensor Detection test");
  Serial.println("1. Valve Click test");
  Serial.println("2. Inflation test");
  Serial.println("3. Vacuum test");
  Serial.println("4. Battery test");
  Serial.println("5. Leak test");
  Serial.println("6. Power OFF");
  Serial.println("Type r after a test completes to repeat it.");
  Serial.println("------\n");
}

void loop() {
  flowio.pixel(1,1,1);
  if(repeatTest==true){
    prevMode=-1; //reset the prevMode variable to assume that the mode has changed
    repeatTest = false;
  }
  if(mode != prevMode){ //Only execute this code if the mode has changed.
    switch(mode){
      case 0: //we come here when we first start the system
        flowio.blueLED(1);
        sensorTest();
        Serial.println("------\nType 1 or press UserSW button for: 'Valve Click test'");
        Serial.println(" - Each valve will turn on and off sequentially. You should count 14 clicks.");
        break;
      case 1:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        valveClickTest();
        Serial.println("------\nType 2 or press UserSW button for: 'Inflation Pump Test'");
        //Serial.println(" - Run inflation pump for 100ms. Internal pressure should increase.");
        break;
      case 2:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        inflationPumpTest();
        Serial.println("------\nType 3 or press UserSW button for: 'Vacuum Pump Test'");
        //Serial.println(" - Run vacuum pump for 100ms. Internal pressure should decrease");
        break;
      case 3:
        flowio.blueLED(1);
        delay(500); //delay to allow enough time to release button
        vacuumPumpTest();
        Serial.println("------\nType 4 or press UserSW button for: 'Battery Connection Test'");
        //Serial.println(" - Compares battery voltage with valves off and on and looks for drop");
        break;
      case 4:
        flowio.blueLED(1);
        delay(500);
        batteryConnectionTest();
        Serial.println("------\nNext Test: 'Manually Check for sensor leaks'");
        Serial.println("Before running this test, disconnect the pump module and connect a syringe to the");
        Serial.println("inelt valve (rightmost). When the test starts, the inlet valve will open and stay");
        Serial.println("open for 20 seconds. During that time, try pushing air with the syringe into the");
        Serial.println("inlet valve to check if there are any leaks. Type 5 to begin this test."); 
        break;
      case 5:
        flowio.blueLED(1);
        delay(500);
        manualLeakTest();
        Serial.println("------\nType 6 or press UserSW button to: 'Power OFF'");
        break;
      case 6:
        Serial.println("Powering OFF......\n\n");
        flowio.powerOFF();
        break;
    }
    if(repeatTest) repeatTest=false;
    prevMode = mode;
  }
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW)  //and if it is now pressed.
      mode += 1;
      if(mode>5) mode=0;
      delay(50); //debounce
  }
  prevButtonState = buttonState; 
  //NOTE: If I enter a character into serial monitor and press enter, this will result in two bytes! The first 
  //byte will be the character entered, and the second will be LineFeed (Decimal 10). If I just click the send
  //button without entereing anything, the LineFeed will be received!
  while(Serial.available() > 0) {
    // read the incoming byte:
    char incomingByte = Serial.read();
    if(incomingByte=='r') repeatTest=true;
    else if(incomingByte-'0' >= 0 && incomingByte-'0'<= 6){ //if it is an allowed mode
      mode = incomingByte-'0'; //subtract ascii 0 to get the number the user wants to execute.
      Serial.println(mode);
      Serial.print("Starting test #");
      Serial.println(incomingByte - '0');
    }
  } 
}

//############################--Function Definitions--##################################
//######################################################################################

void batteryConnectionTest(){
  flowio.pixel(1,1,1);
  Serial.print("Battery Connection Test in Progress...");
  pinMode(batteryPin,INPUT);
  int vbat0 = analogRead(batteryPin);
  flowio.openInletValve();
  flowio.openOutletValve();
  delay(800);
  int vbat1 = analogRead(batteryPin);
  flowio.closeInletValve();
  flowio.closeOutletValve();
  if(vbat1<vbat0){
    Serial.println("...Success! (*_*) ");
    Serial.println("Battery test completed. Type r to repeat it.");
  }
  else{
    Serial.println("Fail");
  }
}
void manualLeakTest(){
  flowio.pixel(1,1,1);
  flowio.openInletValve();
  Serial.println("Manual Leak Test in Progress...");
  Serial.println("You have 20 seconds to check for leaks on the inlet valve.");
  Serial.print("Seconds remaining: ");
  for(int i=20; i>=0; i--){
    if(i%5==0) Serial.print(i);
    Serial.print(".");
    delay(1000);
  }
  flowio.closeInletValve();
  Serial.println("\nLeak Test completed. Type r to repeat it.");
  flowio.pixel(0,1,0);
}
void valveClickTest(){
  flowio.pixel(1,1,1);
  Serial.print("Valve Click Test in Progress....");
  Serial.print("out..");
  flowio.openOutletValve();
  delay(500);
  flowio.closeOutletValve();
  delay(1000);
  Serial.print("p1..");
  flowio.setPorts(0b00000001);
  delay(500);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p2..");
  flowio.setPorts(0b00000010);
  delay(500);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p3..");
  flowio.setPorts(0b00000100);
  delay(500);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p4..");
  flowio.setPorts(0b00001000);
  delay(500);
  flowio.setPorts(0b00000000);
  delay(1000);
  Serial.print("p5..");
  flowio.setPorts(0b00010000);
  delay(500);
  flowio.setPorts(0b00000000);
  delay(1000); 
  Serial.println("in..");
  flowio.openInletValve();
  delay(500);
  flowio.closeInletValve();
  delay(1000);
  Serial.println("Valve test completed. Type r to repeat it.");  
  //flowio.pixel(0,0,0);
}
void sensorTest(){
  flowio.pixel(1,1,1);
  //To get the correct pressure reading, we will open and close two ports first
  flowio.openPorts(0b00000011);
  delay(100);
  flowio.stopAction(0xFF);
  //Serial.println("Sensor Test in Progress....");
  while(flowio.activateSensor()==false){
    Serial.print("0. Sensor Detection Test......FAILED.");
    Serial.println(flowio.readError());
    flowio.pixel(1,0,0);
    delay(10);
  }
  flowio.pixel(1,1,1);
  Serial.print("0. Sensor Detection Test......Success! (*_*) ");
  Serial.print("P = ");
  p0=flowio.getPressure(PSI);
  Serial.println(p0);
  Serial.println("Type r to repeat the test.");  
  flowio.pixel(0,1,0);
}
void inflationPumpTest(){
  flowio.pixel(1,1,1);
  Serial.print("Inflation Pump Test in Progress....");
  flowio.openInletValve();
  flowio.startPump(1);
  delay(100);
  Serial.print("P = ");
  pinf=flowio.getPressure(PSI);
  Serial.print(pinf);
  if(pinf<=(p0+0.4)){
    Serial.println("...FAILED"); //pressure should be at least 0.4psi higher.
    flowio.pixel(1,0,0);
  }
  else{
    Serial.println("...Success! (*_*) ");
    flowio.pixel(0,1,0);
  }
  Serial.println("Type r to repeat the test.");  
  flowio.stopPump(1);
  //release the pressure and close the valves:
  flowio.openOutletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
}
void vacuumPumpTest(){
  flowio.pixel(1,1,1);
  Serial.print("Vacuum Pump Test in Progress....");
  flowio.openOutletValve();
  flowio.startPump(2);
  delay(100);
  Serial.print("P = ");
  pvac=flowio.getPressure(PSI);
  Serial.print(pvac);
  if(pvac>=(p0-0.4)){
    Serial.println("...FAILED"); //pressure should be at least 0.4psi lower.
    flowio.pixel(1,0,0);
  }
  else{
    Serial.println("...Success! (*_*) ");
    flowio.pixel(0,1,0);
  }
  Serial.println("Type r to repeat the test.");  
  flowio.stopPump(2);
  //release the pressure and close the valves
  flowio.openInletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
}
