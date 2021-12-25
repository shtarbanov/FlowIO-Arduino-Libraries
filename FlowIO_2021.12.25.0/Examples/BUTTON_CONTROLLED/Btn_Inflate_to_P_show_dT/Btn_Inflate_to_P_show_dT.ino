/*Working Example
 * Press the button once to inflate port 3 to 30psi absolute pressure. Inflation will stop automatically  
 * when the pressure is reached. The serial monitor will display the time it took to reach this pressure
 * and the exact pressure value at which the inflation stopped.  
 * 
 * If you press the button again while inflating, the inflation will stop and the start timer will reset to 0
 * And then if you press it once more, the inflation will start again. The amount of time displayed for inflation
 * will include only the last inflation cycle and won't include inflation cycles that you stopped with the button.
 * 
 * NOTE: This uses the inflateP() function which needs to be called repeatedly until the pressure is reached.
 */
#include <FlowIO.h>
#define btnPin 7

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button
bool activeFlag = 0;

uint32_t inflationTime = 0;
uint8_t ports = 0b00000010; //port 2.

bool serialOneTimeFlag=false; 

FlowIO flowio;

void setup(){
  pinMode(btnPin,INPUT_PULLUP); 
  
  flowio = FlowIO(GENERAL); 
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
  Serial.begin(115200);
  while(flowio.activateSensor()==false){
    flowio.pixel(1,0,0);
    delay(10);
  }
  flowio.pixel(1,1,1);
}

void loop() {
  if(Serial && serialOneTimeFlag==false){
    serialOneTimeFlag=true;
    Serial.println("Inflate to P and show dT");
  }
  while(flowio.readError()!=0){
    Serial.print("Error: ");
    Serial.println(flowio.readError());
    delay(1000);
  }
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW)  //and if it is now pressed.
      activeFlag = !activeFlag;
      if(activeFlag){
        flowio.pixel(10,0,0);
      }
      else{
        flowio.pixel(1,1,1);
        flowio.stopAction(ports);
        //if inflation is complete, reset the flag
        if(flowio.inflatePcomplete) flowio.inflatePcomplete=false;
      }
  }
  prevButtonState = buttonState; 
  flowio.optimizePower(150,1000);

  //This will get invoked on each iteration of the loop:
  if(activeFlag){
    inflationTime = flowio.inflateP(ports,30,PSI); //returns 0 when complete, 1 when running, time when stopping.
    if(inflationTime>1){
      Serial.print("T(ms) = ");
      Serial.println(inflationTime);
      Serial.print("P(psi) = ");
      Serial.println(flowio.getPressure(PSI));
    }
  }
}
