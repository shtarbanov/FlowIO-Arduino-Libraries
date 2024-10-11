void initHardwareTest(){
  Serial.println(F("\n### --FlowIO Self Test Initialized - ###"));
  Serial.println(F("------------------------------"));
  Serial.println(F("---> Input commands: <---"));
  Serial.println(F("0 - Pressure Sense test\t ---> Detect sensor and measure the ambient pressure."));
  Serial.println(F("1 - Valve Click test\t ---> Each valve will turn on / off sequentially. Listen for 14 clicks."));
  Serial.println(F("2 - Battery test\t ---> Detect if battery is present"));
  Serial.println(F("3 - Inflation pump test\t ---> Run pump1 for 100ms to increase internal pressure, then release."));
  Serial.println(F("4 - Vacuum pump test\t ---> Run pum2 for 100ms to decrease internal pressure, then release."));
  Serial.println(F("5 - Variable Inf test\t ---> Inflate with dropping PWM. Listen for decreasing sound"));
  Serial.println(F("6 - Variable Vac test\t ---> Vacuum with dropping PWM. Listen for decreasing sound"));  
  Serial.println(F("7 - Manual Leak test\t ---> Manually check for leask with a syringe connected to the inlet."));
  Serial.println(F("8 - Power OFF test\t ---> Shut down the device\n"));
  Serial.println(F("n - next test."));
  Serial.println(F("r - repeat last test."));
  Serial.println(F("i - Device information"));
  Serial.println(F("btn - next test (press the hidden button to run). \n------------------------------"));
  Serial.println(F("---> Physical Indicators: <---"));
  Serial.println(F("BLUE Pulsing\t- test running"));
  Serial.println(F("GREEN Blinking\t- test succeeded"));
  Serial.println(F("RED Blinking\t- test failed \n------------------------------------"));
}
//############################--Test Functions--##################################
void valveDetectionTest(){ //This test must be done with the USB cable disconnected, because no battery voltage drop would happen otherwise.
    //Serial.println("The following test requires you to disconnect FlowIO from the USB cable and rely on the LED indicator for the results. GREEEN bliks indicated success, RED blinks indicate failure. Each valve's result is provided independently via the LED.
    //Once the test completes, you can reconnect the USB cable. When you are ready to begin, disconnect the USB cable and press the hidden button under the letter W on the enclosure.");
    flowio.pixel(0, 0, 0); // Turn off the pixel
    delay(500);  
    pinMode(batteryPin,INPUT);
    int vbat[8]; //index [0]=default, [1]=out, [2]=p1,...,[6]=p5, [7]=in
   
   //no load [0]
    vbat[0] = analogRead(batteryPin);

  //Outlet valve test, vbat[1]
    Serial.print("out-->");
    flowio.openOutletValve();
    flowio.pixel(0, 0, 5);
    delay(500);
    vbat[1] = analogRead(batteryPin);
    flowio.closeOutletValve();
    flowio.pixel(0, 0, 0);
    (vbat[1]<vbat[0]) ? Serial.print("OK,..") : Serial.print("FAIL,..");
    (vbat[1]<vbat[0]) ? showSuccess() : showFailure();
    delay(1000);

  //Ports 1 to 5 tests
    for (uint8_t i = 0; i < 5; i++) {
        Serial.print("p");
        Serial.print(i+1);
        Serial.print("-->");
        flowio.setPorts(1 << i); // Set port to 1, 2, 4, 8, 16
        flowio.pixel(0, 0, 5);
        delay(500);
        vbat[i+2]= analogRead(batteryPin);
        flowio.setPorts(0b00000000);
        flowio.pixel(0, 0, 0);
        (vbat[i+2]<vbat[0]) ? Serial.print("OK,..") : Serial.print("FAIL,..");
        (vbat[i+2]<vbat[0]) ? showSuccess() : showFailure();
        delay(1000);

    }

    //Inlet valve  test
    Serial.print("in-->");
    flowio.openInletValve();
    flowio.pixel(0, 0, 5);
    delay(500);
    vbat[7]= analogRead(batteryPin);
    flowio.closeInletValve();
    flowio.pixel(0, 0, 0);
    (vbat[7]<vbat[0]) ? Serial.print("OK....") : Serial.print("FAIL....");
    (vbat[7]<vbat[0]) ? showSuccess() : showFailure();
    delay(1000);


  // (vbat1<vbat0) ? showSuccess() : showFailure();
  Serial.println("done");
  for(int i=0; i<8; i++){
    Serial.println(vbat[i]);
  }
}

//TEST 0
void sensorTest(){
  flowio.pixel(0,0,0);
  //To get the correct pressure reading, we will open and close two ports first
  flowio.openPorts(0b00000011);
  delay(100);
  flowio.stopAction(0xFF);
  Serial.print("0. Sensor Test in Progress............");
  while(flowio.activateSensor()==false){
    showFailure();
    Serial.println(flowio.readError());
    flowio.pixel(1,0,0);
    delay(10);
  }
  flowio.pixel(1,1,1);
  Serial.print("P = ");
  p0=flowio.getPressure(PSI);
  Serial.print(p0);
  showSuccess();
  flowio.pixel(0,1,0);
  Serial.println("\tdone");
}

//TEST 1
void valveClickTest(){
    flowio.pixel(0, 0, 0); // Turn off the pixel
    Serial.print("1. Valve Click Test in Progress....");
    delay(500);
    //Outlet valve click test
    Serial.print("out..");
    flowio.openOutletValve();
    flowio.pixel(0, 0, 5);
    delay(500);
    flowio.closeOutletValve();
    flowio.pixel(0, 0, 0);
    delay(1000);
    //Ports 1 to 5 click tests
    for (uint8_t i = 0; i < 5; i++) {
        Serial.print("p");
        Serial.print(i+1);
        Serial.print("..");
        flowio.setPorts(1 << i); // Set port to 1, 2, 4, 8, 16
        flowio.pixel(0, 0, 5);
        delay(500);
        flowio.setPorts(0b00000000);
        flowio.pixel(0, 0, 0);
        delay(1000);
    }
    //Inlet valve click test
    Serial.print("in.");
    flowio.openInletValve();
    flowio.pixel(0, 0, 5);
    delay(500);
    flowio.closeInletValve();
    flowio.pixel(0, 0, 0);
    delay(1000);
    Serial.println("\tdone");
    flowio.pixel(1,1,1);
}

//TEST 2
void batteryConnectionTest(){
  flowio.pixel(0,0,0);
  Serial.print("2. Battery Connection Test in Progress...");
  pinMode(batteryPin,INPUT);
  int vbat0 = analogRead(batteryPin);
  flowio.openInletValve();
  flowio.openOutletValve();
  flowio.setPorts(0b00011111);
  delay(800);
  int vbat1 = analogRead(batteryPin);
  flowio.setPorts(0b00000000);
  flowio.closeInletValve();
  flowio.closeOutletValve();
  (vbat1<vbat0) ? showSuccess() : showFailure();
  Serial.println("\tdone");
}

//TEST 3
void inflationPumpTest(){
  flowio.pixel(0,0,0);
  Serial.print("3. Inflation Pump Test in Progress....");
  flowio.openInletValve();
  flowio.startPump(1);
  delay(100);
  pinf=flowio.getPressure(PSI);
  Serial.print("P = ");
  Serial.print(pinf);
  flowio.stopPump(1);
  //release the pressure and close the valves:
  flowio.openOutletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
  if (pinf > (p0+0.4)){ showSuccess();}
  else                { flowio.raiseError(0); showFailure();} //BUG: Raising a code other than 0 causes next pressure reading to be -1.
  // (pinf > (p0+0.4)) ? showSuccess() : showFailure();
  Serial.println("\tdone");
}


//TEST 4
void vacuumPumpTest(){
  flowio.pixel(0,0,0);
  Serial.print("4. Vacuum Pump Test in Progress.......");
  flowio.openOutletValve();
  flowio.startPump(2);
  delay(100);
  pvac=flowio.getPressure(PSI);
  Serial.print("P = ");
  Serial.print(pvac);
  flowio.stopPump(2);
  //release the pressure and close the valves
  flowio.openInletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
  // (pvac < (p0-0.4)) ? : flowio.raiseError(2);
  (pvac < (p0-0.4)) ? showSuccess() : showFailure();
  Serial.println("\tdone");
}

//TEST 5
void pwmInflationTest(){
  flowio.pixel(0,0,5);
  Serial.print("5. Variable Inflation Test in progress.........");
  for(int i=255; i>45; i--){
    flowio.startInflation(0b00000001,i);
    delay(20);
  }
  flowio.stopAction(0b00000001);
  Serial.println("\t\t\tdone");
  flowio.pixel(1,1,1);
}

//TEST 6
void pwmVacuumTest(){
  flowio.pixel(0,0,5);
  Serial.print("6. Variable Vacuum Test in progress............");
  for(int i=255; i>45; i--){
    flowio.startVacuum(0b00000001,i);
    delay(20);
  }
  flowio.stopAction(0b00000001);
  Serial.println("\t\t\tdone");
  flowio.pixel(1,1,1);
}

//TEST 7
void manualLeakTest(){
  flowio.pixel(0,0,0);
  Serial.println("7. Manual Leak Test in Progress...");
  flowio.openInletValve();
  flowio.setPorts(0b00000000);
  Serial.println("You have 20 seconds to check for leaks via the inlet valve.");
  for(int i=20; i>=0; i--){   
    //Output i & pressure every second
    Serial.print(i);
    Serial.print(": P = ");
    pinf=flowio.getPressure(PSI);
    Serial.println(pinf);
    delay(1000);
    flowio.pixel(0,0,1);
    (i%2==0) ? flowio.pixel(0,0,1) : flowio.pixel(0,0,0);;
  }
  flowio.closeInletValve();
  Serial.println("\tdone");
  flowio.pixel(1,1,1);
}

// //############################--Helper Functions--##################################
// //##################################################################################
void showSuccess(){
  //show a confimation of test sucess by blinking GREEEN FIVE times.
  Serial.print("\tSuccess! (*_*).");
  for(int i=0; i<5; i++){
    flowio.pixel(0,0,0);
    delay(50);
    flowio.pixel(0,5,0);
    delay(75);
  }
  flowio.pixel(0,1,0);
  delay(100); 
}
void showFailure(){
  Serial.print("\tError = ");
  Serial.print(flowio.readError());
  //blink RED 10 times
  for(int i=0; i<10; i++){
    flowio.pixel(0,0,0);
    delay(50);
    flowio.pixel(5,0,0);
    delay(75);
  }
  flowio.pixel(1,0,0);
  //don't allow pixel dancing.
  // dancingNeoPixel=false; 
}

void printTestResult(){
  //prints the result from the last test
  Serial.print("Test: ");
  Serial.print(testMode);
  Serial.print(", Results: [");
  for(int i=0; i<sizeof(testResults); i++){
    Serial.print(testResults[i]);
    (i+1)==sizeof(testResults) ? Serial.print("]") : Serial.print(", ");
  }
}

//#####################################################################################
void printDeviceInfo(){
  Serial.print("\n-----------INFO---------------\nMinutes till power off: ");
  Serial.println(autoPowerOff(AUTOOFFTIMER));
  Serial.print("FlowIO System ID: ");
  Serial.println(getMcuUniqueID());
  Serial.print("Hardware state: ");
  Serial.println(flowio.getHardwareState(),BIN);
  Serial.print("------------------------------\n");
}