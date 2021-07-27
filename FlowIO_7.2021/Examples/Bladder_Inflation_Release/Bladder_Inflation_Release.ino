/*
 * This firmware has 3 active modes and 1 idle mode. You can switch between the modes using
 * the Button on the microcontroller. The active modes use BOTH pumps. I will be using not the 
 * API but the lowest driver-level functions for this firmware, where I manually control everything.
 * Connect the inflation pump to the inlet valve as you would normally do.
 * Connect the bladder to ports 2 and 4.
 * 
 * Mode 0: On startup, start in the idle mode where nothing is happening.
 * Mode 1: startInflation(ports 2,4) with both pumps on 
 * Mode 2: Another idle state stopAction
 * Mode 3: startDeflation(ports 2,4). Deflate throug the outlet + ports 1,3,5.
 * 
 * The modes should have a different LED color. 
 * The Idle modes should be pixel(1,1,1).
 * The inflation mode should be pixel(10,0,0)
 * The deflation mode should be pixel(10,0,10);
 * 
 * 
 * 
 * Alternative options:
 * Inflate the blader directly, without going through the valves.
 * 
 * NOTES: The A valves have the higher flow rate than the D valves. So deflation should be happening through A
 */


#include <FlowIO.h>
#define btnPin 7

FlowIO flowio;
int mode = 0;
int prevMode = -1; //making this different so we enter the state machine on first start.

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button

void setup(){
  //flowio = FlowIO(GENERAL);
  flowio = FlowIO(INFLATION_PARALLEL); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  
  pinMode(btnPin,INPUT_PULLUP);   
  flowio.blueLED(HIGH);
  flowio.pixel(1,1,1);
}

void loop() {
  if(mode != prevMode){ //Only execute this code if the mode has changed.
    switch(mode){
      case 0: //idle. we come here when we first start the system
        flowio.stopAction(0xFF);
        flowio.pixel(1,1,1);
        break;
      case 1: //start inflating ports 2 and 4
//        delay(500); //delay to allow enough time to release button
        flowio.openInletValve();
        flowio.openOutletValve();
        flowio.setPorts(0b00001010);
        flowio.pixel(10,0,0);
        break;
      case 2:
        flowio.stopAction(0xFF);
        flowio.pixel(1,1,1);
        break;
      case 3:
//        delay(500); //delay to allow enough time to release button
        flowio.startRelease(0b00011111);
        flowio.pixel(10,0,5);
        break;
    }
    prevMode = mode;
  }
  buttonState = digitalRead(btnPin);
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == LOW)  //and if it is now pressed.
      mode += 1;
      if(mode>3) mode=0;
      delay(50); //debounce
  }
  prevButtonState = buttonState; 
}
