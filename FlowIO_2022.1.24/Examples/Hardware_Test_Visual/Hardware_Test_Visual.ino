#include <Adafruit_TinyUSB.h> // for Serial
#include <FlowIO.h>
#define btnPin 7
#define batteryPin A6

FlowIO flowio;
int mode = 0;
int prevMode = -1; //making this different so we enter the state machine on first start.
bool dancingPixel = false;
bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
float p0 = 0;
float pinf=0;
float pvac=0;

unsigned long prevTime = millis();

void setup(){
  flowio = FlowIO(GENERAL); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  
  pinMode(btnPin,INPUT_PULLUP);   
  flowio.blueLED(HIGH);
}

void loop() {
  if(dancingPixel==true){
    if(millis() - prevTime > 300){
      flowio.pixel(random(0,6),random(0,6),random(0,6));
      prevTime = millis();          
    }
  }
  if(mode != prevMode){ //Execute the following only if the mode changes.
    //The code in each case below will execute ONLY ONCE, not continuously.
    switch(mode){
      case 0: //we come here when we first start the system
        dancingPixel = true;
        break;
      case 1: //SENSOR (Error = RED)
        delay(500); //delay to allow enough time to remove finger from the button
        sensorTest(); //this function does NOT change the dancingPixel variable, but stop the dance while running.
        //Hence, the dance will automatically restat when we exit the sensorTest() and our main loop gets invoked again. 
        break;
      case 2: //VALVES (Error = no sound)
        delay(500); //delay to allow enough time to remove finger from the button
        valveClickTest();
        break;
      case 3: //INFLATION (Error = YELLOW)
        delay(500); //delay to allow enough time to remove finger from the button
        inflationPumpTest();
        break;
      case 4: //VACUUM (Error = PURPLE)
        delay(500); //delay to allow enough time to remove finger from the button
        vacuumPumpTest();
        break;
      case 5: //BATTERY (Error = PINK)
        delay(500);
        batteryConnectionTest();
        break;
      case 6:
        delay(500); //LEAK
        manualLeakTest();
        break;
      case 7: //POWER_OFF
        flowio.powerOFF();
        break;
    }
    prevMode = mode;
  }
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW)  //and if it is now pressed.
      mode += 1;
      if(mode>7) mode=0;
      delay(50); //debounce
  }
  prevButtonState = buttonState; 

}

//############################--Function Definitions--##################################
//######################################################################################
void sensorTest(){
  flowio.pixel(0,0,0); //stop the pixel from dancing
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
  flowio.pixel(0,0,0); //stop the dancing pixel
  delay(100);
  flowio.openOutletValve();
  flowio.pixel(0,0,5);
  delay(500);
  flowio.closeOutletValve();
  flowio.pixel(0,0,0);
  delay(1000);
  flowio.setPorts(0b00000001);
  flowio.pixel(0,0,5);
  delay(500);
  flowio.setPorts(0b00000000);
  flowio.pixel(0,0,0);
  delay(1000);
  flowio.setPorts(0b00000010);
  flowio.pixel(0,0,5);
  delay(500);
  flowio.setPorts(0b00000000);
  flowio.pixel(0,0,0);
  delay(1000);
  flowio.setPorts(0b00000100);
  flowio.pixel(0,0,5);
  delay(500);
  flowio.setPorts(0b00000000);
  flowio.pixel(0,0,0);
  delay(1000);
  flowio.setPorts(0b00001000);
  flowio.pixel(0,0,5);
  delay(500);
  flowio.setPorts(0b00000000);
  flowio.pixel(0,0,0);
  delay(1000);
  flowio.setPorts(0b00010000);
  flowio.pixel(0,0,5);
  delay(500);
  flowio.setPorts(0b00000000);
  flowio.pixel(0,0,0);
  delay(1000); 
  flowio.openInletValve();
  flowio.pixel(0,0,5);
  delay(500);
  flowio.closeInletValve();
  flowio.pixel(0,0,0);
  delay(1000);
}
void inflationPumpTest(){
  flowio.pixel(0,0,0);
  Serial.print("Inflation Pump Test in Progress....");
  flowio.openInletValve();
  flowio.startPump(1);
  delay(100);
  pinf=flowio.getPressure(PSI);
  if(pinf<=(p0+0.4)){
    showFailure(1,1,0); //YELLOW -> inflation pump failed
  }
  else{
    dancingPixel = true; //this line is not needed, b/c this is the default state.
  }
  flowio.stopPump(1);
  //release the pressure and close the valves:
  flowio.openOutletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
}
void vacuumPumpTest(){
  flowio.pixel(0,0,0);
  flowio.openOutletValve();
  flowio.startPump(2);
  delay(100);
  pvac=flowio.getPressure(PSI);
  if(pvac>=(p0-0.4)){
    showFailure(1,0,1); //PURPLE -> vacuum pump failed
  }
  else{
    dancingPixel = true; //this line is not needed, b/c this is the default state.
  }
  flowio.stopPump(2);
  //release the pressure and close the valves
  flowio.openInletValve();
  delay(300);
  flowio.closeOutletValve();
  flowio.closeInletValve();
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
  if(vbat1<vbat0){
    showSuccess();
  }
  else{
    showFailure(5,1,1); //PINK
  }
}
void manualLeakTest(){
  flowio.pixel(0,0,0);
  flowio.openInletValve();
  //You have 60 seconds to check for leaks on the inlet valve.
  for(int i=60; i>=0; i--){
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
    
  }
  flowio.closeInletValve();
}
//############################--Function Definitions--##################################
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
  dancingPixel=true; 
}
void showFailure(uint8_t r, uint8_t g, uint8_t b){
 //set the pixel to RED and don't allow dancing.
 flowio.pixel(r,g,b);
 dancingPixel=false; 
}
