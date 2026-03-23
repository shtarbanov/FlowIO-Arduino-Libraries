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
  //while(!Serial) delay(10); //"Serial" returns true whenever we press the serial monitor button.
  /*This means that we will be stull in this infinite loop UNTIL WE OPEN THE SERIAL MONITOR!!! */
  
}

void loop() {
  flowio.pixel(1,1,1);
  flowio.startInflation(0x01);
  while(flowio.getPressure(PSI)<20){
    //keep going
  }
  flowio.stopAction(0x01);
  flowio.startVacuum(0x01);
  while(flowio.getPressure(PSI)>10){
    //keep vacuum
  }
  //Write some code here
}
