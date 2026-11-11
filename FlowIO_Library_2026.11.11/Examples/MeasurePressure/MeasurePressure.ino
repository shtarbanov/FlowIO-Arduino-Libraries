/*
Open the inlet valve and start inflation to raise the internal pressure. Keep the inflation going while the pressure is increasing. 
Once the pressure stops increasing, release the pressure through a port and then stop all activities. Report the pressure every 5ms.
What should my condition be for monitoring for pressure increases. I should incrementally increase the pressure, measure it, compare it with the last one, then incrase more by the same or lower amount.
If the new difference is less than the previous difference, I should increase pressure by a lesser amount. I can control both time and PWM of the pump to manage the increase.

But at first, I should just output everything for say 2 seconds and not store anything anywhere. 
*/
//This is the barebones minimum code you need for use of FlowIO. Use this as your starting template

#include <Adafruit_TinyUSB.h> // for Serial
#include <FlowIO.h>
FlowIO flowio;

int startTime = millis();

void setup(){
  flowio = FlowIO(GENERAL); //INFLATION_SERIES, INFLATION_PARALLEL, VACUUM_SERIES, VACUUM_PARALLEL
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
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
    sensorTest();
    onCableReconnect();
  }else if(serialPortOpened && !Serial){ //if the Serial port was active and then the cable got unplugged:
    serialPortOpened=false; //no longer open
    //anything here executes without showing anything to the screen.
  }
  //#############################################################
  
  
}

void onCableReconnect(){
  Serial.println("\n----------Reconnected----------");
  Serial.println("---INFLATION PRESSURE---");
  flowio.openInletValve();
  flowio.startPump(1,255);
  float p = 0;
  for(int i=0; i<40; i++){
    p = flowio.getPressure(PSI);
    delay(10);
    Serial.println(p);
  }
  flowio.startInflation(0x01);
  delay(50);
  flowio.stopAction(0x01);

  Serial.println("\n---VACUUM PRESSURE---");
  flowio.openOutletValve();
  flowio.startPump(2,255);
  for(int i=0; i<40; i++){
    p = flowio.getPressure(PSI);
    delay(10);
    Serial.println(p);
  }
  flowio.startVacuum(0x01);
  delay(50);
  flowio.stopAction(0x01);
}

void sensorTest(){
  flowio.pixel(0,0,0);
  //To get the correct pressure reading, we will open and close two ports first
  flowio.openPorts(0b00000011);
  delay(100);
  flowio.stopAction(0xFF);
  Serial.print("0. Sensor Test in Progress............");
  while(flowio.activateSensor()==false){
    Serial.println(flowio.readError());
    flowio.pixel(1,0,0);
    delay(10);
  }
  flowio.pixel(1,1,1);
  Serial.print("P = ");
  float p0=flowio.getPressure(PSI);
  Serial.print(p0);
  flowio.pixel(0,1,0);
  Serial.println("\tdone");
}
