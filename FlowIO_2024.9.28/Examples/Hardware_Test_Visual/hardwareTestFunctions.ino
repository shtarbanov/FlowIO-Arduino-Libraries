//############################--Test Function Definitions--##################################
//######################################################################################
void sensorTest(){
  //To get the correct pressure reading, we will open and close two ports first
  flowio.openPorts(0b00000011);
  delay(100);
  flowio.stopAction(0xFF);
  while(flowio.activateSensor()==false){ //FAILED
    delay(10);
    showFailure(1,0,0); //RED
  }
  p0=flowio.getPressure(PSI);
  showSuccess(); //3 GREEN blinks
}
void valveClickTest(){
    flowio.pixel(0, 0, 0); // Turn off the pixel
    delay(500);
    //Outlet valve click test
    flowio.openOutletValve();
    flowio.pixel(0, 0, 5);
    delay(500);
    flowio.closeOutletValve();
    flowio.pixel(0, 0, 0);
    delay(1000);
    //Ports 1 to 5 click tests
    for (uint8_t i = 0; i < 5; i++) {
        flowio.setPorts(1 << i); // Set port to 1, 2, 4, 8, 16
        flowio.pixel(0, 0, 5);
        delay(500);
        flowio.setPorts(0b00000000);
        flowio.pixel(0, 0, 0);
        delay(1000);
    }
    //Inlet valve click test
    flowio.openInletValve();
    flowio.pixel(0, 0, 5);
    delay(500);
    flowio.closeInletValve();
    flowio.pixel(0, 0, 0);
    delay(1000);
}
void inflationPumpTest(){
  flowio.pixel(0,0,0);
  flowio.openInletValve();
  flowio.startPump(1);
  delay(100);
  pinf=flowio.getPressure(PSI);
  flowio.stopPump(1);
  //release the pressure and close the valves:
  flowio.openOutletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
  (pinf > (p0+0.4)) ? showSuccess() : showFailure(1,1,0); //YELLOW -> inflation pump failed
}
void vacuumPumpTest(){
  flowio.pixel(0,0,0);
  flowio.openOutletValve();
  flowio.startPump(2);
  delay(100);
  pvac=flowio.getPressure(PSI);
  flowio.stopPump(2);
  //release the pressure and close the valves
  flowio.openInletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
  (pvac < (p0-0.4)) ? showSuccess() : showFailure(1,0,1); //PURPLE -> vacuum pump failed
}
void batteryConnectionTest(){
  flowio.pixel(0,0,0);
  pinMode(batteryPin,INPUT);
  int vbat0 = analogRead(batteryPin);
  flowio.openInletValve();
  flowio.openOutletValve();
  delay(800);
  int vbat1 = analogRead(batteryPin);
  flowio.closeInletValve();
  flowio.closeOutletValve();
  (vbat1<vbat0) ? showSuccess() : showFailure(5,1,1); //PINK
}
void pwmInflationTest(){
  for(int i=255; i>100; i--){
    flowio.startInflation(0b00000001,i);
    delay(20);
  }
  flowio.stopAction(0b00000001);
}
void pwmVacuumTest(){
  for(int i=255; i>100; i--){
    flowio.startVacuum(0b00000001,i);
    delay(20);
  }
  flowio.stopAction(0b00000001);
}
void manualLeakTest(){
  flowio.pixel(0,0,0);
  flowio.openInletValve();
  //You have 20 seconds to check for leaks on the inlet valve.
  for(int i=20; i>=0; i--){
    flowio.pixel(0,0,0);
    delay(100);
    flowio.pixel(1,0,0);
    delay(100);
    flowio.pixel(0,0,0);
    delay(100);
    flowio.pixel(0,1,0);
    delay(100);
    flowio.pixel(0,0,0);
    delay(100);
    flowio.pixel(0,0,1);
    delay(100);
    flowio.pixel(0,0,0);
    delay(100);
    flowio.pixel(1,1,0);
    delay(100);
    flowio.pixel(0,0,0);
    delay(100);
    flowio.pixel(0,1,1);
    delay(100);
    flowio.pixel(0,0,0);
  }
  flowio.closeInletValve();
}
//############################--Status Function Definitions--##################################
//######################################################################################
void showSuccess(){
  //show a confimation of test sucess by blinking GREEEN three times.
  flowio.pixel(0,0,0);
  delay(200);
  flowio.pixel(0,5,0);
  delay(200);
  flowio.pixel(0,0,0);
  delay(200);
  flowio.pixel(0,5,0);
  delay(200);
  flowio.pixel(0,0,0);
  delay(200);
  flowio.pixel(0,5,0);
  delay(200); 
  flowio.pixel(0,0,0);
  delay(200); 
}
void showFailure(uint8_t r, uint8_t g, uint8_t b){
 //set the pixel to RED and don't allow dancing.
 flowio.pixel(r,g,b);
 dancingNeoPixel=false; 
}
