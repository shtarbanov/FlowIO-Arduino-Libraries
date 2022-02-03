/*Written by Ali Shtarbanov*/

//TODO: Test whether (_hardwareState & ports)==ports) indeed checks idf the hardwareState matches with the ports.
//the way to do this test is by having inflation for 2 seconds on the same port BEFORE I run the inflateP function
//in the main loop. Then see if I get those 2 seconds added to my total inflation time. They should be added if
//this is working fine.

#include "Arduino.h"
#include "FlowIO.h"

//FEEDBACK CONTROL: Presure Dependent Methods (non-blocking)
//I require the user to specify the pressure and the unit of that pressure explicity.

  /*************************************************************************************************
    Algorithm for inflateP():
    This function expects to be invoked multiple times, so it must check whether it has completed its
    taks on a previous iteration or not, for which we rely on the the external flag inflatePcomplete.
      If the inflatePcomplete flag is true, then don't do anything and return 0.
      Else:
        - If inflation on specified 'ports' is not running yet, start inflation and return 1.
        - else, If the task is already running, check if pressure>=pmax and if not, return 1
            otherwise stop the inflation and return the amount of time it took form when inlet was opened.

      NOTE: User must reset inflatePcomplete flag if the want to run this function again after it's complete.
      This applies even if they want to run this on a different port, b/c the flag is port agnostic.

      NOTE: Suppose I call this function after I have already been inflating the same port for 2 seconds. Then,
      it will recognize that the action is already happening, and after it is done, it will return the time
      relative to the start of the original inflation action when the inlet valve was opened. In other words,
      the time returned will include those 2 extra seconds.
  ***************************************************************************************************/
  uint32_t FlowIO::inflateP(uint8_t ports, float pMax, Unit pUnit, uint8_t pwmValue){ //(nonblocking)
    //If the task has already completed in a previous iteration, return 0 and don't do anything.
    if(inflatePcomplete==true){ //this flag is defined in the FLowIO.h file and is false by default.
      return 0; //Indicates that the task has completed.
    }else{
      //If we are not inflating OR the active ports do NOT match 'ports' argument, start the inflation:
      if(getHardwareStateOf(INLET)==0 || ((_hardwareState & 0b00011111) ^ ports)!=0){
        startInflation(ports, pwmValue);
        return 1; //indicates that the task is now running.
      }
      //Otherwise, if we are already inflating on the appropriate ports:
      else{
        if(getPressure(pUnit)<pMax){
          return 1; //indicates that the function is still running
        } else { //if pMax is reached, stop inflating and set task to done.
          //CRITICAL:I must save startTimeOf(INLET) before calling stopAction()
          //because that function will reset the start time to 0.
          uint32_t dt = millis() - getStartTimeOf(INLET);
          stopAction(ports); //resets the startTimes to 0.
          inflatePcomplete = true; //the inflation is now completed.
          return dt; //return the time to inflate.
        }
      }
    }
  }
