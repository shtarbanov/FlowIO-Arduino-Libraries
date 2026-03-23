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
  /*This means that we will be stuck in this infinite loop UNTIL WE OPEN THE SERIAL MONITOR!!! */
  
}

void loop() {
  flowio.pixel(1,1,1);
  flowio.startPump(1,255);
  delay(1000);
  flowio.startPump(1,50);
  delay(2000);
  flowio.stopPump(1);
  delay(5000);
  //Write some code here
}
