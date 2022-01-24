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
    //------------------------------------------------------
    ports = ports & 0b00011111; //clear the upper 3 bits in case they are not 0. They must be 0 for
    //our comparison operations below to work as expected.
    //-------------------------------------------------------
    //If user invokes inflateP() while another one with differnt ports is already running, ignore it:
    /*This is a little more complicated than it seems. The main check will happen in the line where I check
    the _inflatePrunning && compare _hardwareState to ports. However, that one is not enough, because
    I can have a situation where I start the function, the _inflatePrunning becomes true, and then
    externally to this function I run the stopAction() command. At that point, the _inflatePrunning still
    remains true, and the _hardwareState does not match ports because the ports are off. And thus I would
    end up in a situation where I can't even reinvoke the same function with the same ports. Thefore, What
    I must do first is check for this special case and set the _inflatePrunning flag to false if the
    the ports are all off. First I must take care of this special case:*/
    if(_inflatePrunning && (_hardwareState & 0b00011111)==0) _inflatePrunning=false;
    //Now I can have the main check to ignore other functions of this type:
    if(_inflatePrunning && (_hardwareState & 0b00011111)!= ports){
      return 1; //indicates a task is in progress.
    }
    //-------------------------------------------------------
    //Now we can define the essence of our function
    //-------------------------------------------------------
    //If the task has already completed in a previous iteration, return 0 and don't do anything.
    if(inflatePcomplete==true){ //this flag is defined in the FLowIO.h file and is false by default.
      return 0; //Indicates that the task has completed.
    }else{
      //If we are not inflating OR the active ports do NOT match 'ports' argument, start the inflation:
      if(getHardwareStateOf(INLET)==0 || ((_hardwareState & 0b00011111) ^ ports)!=0){
        startInflation(ports, pwmValue);
        _inflatePrunning = true;
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
          _inflatePrunning = false;
          inflatePcomplete = true; //the inflation is now completed.
          return dt; //return the time to inflate.
        }
      }
    }
  }
  /*This function allows you to inflate multiple ports simultaneously to different pressures.
  When invoking the function, You must enter values for all 5 pressures, which are mapped to the 5 ports.
  However, if you set a value to 0 then that port will simply be ignored. This function will NOT
  return anything which will make its implementation much simpler.
  */
  void FlowIO::inflateP_diff(uint8_t ports, float pMax1, float pMax2, float pMax3, float pMax4, float pMax5, Unit pUnit, uint8_t pwmValue){
    ports = ports & 0b00011111; //clear the upper 3 bits in case they are not 0. They must be 0 for
    //our comparison operations below to work as expected.
    //-------------------------------------------------------
    //If user invokes inflateP() while another one with differnt ports is already running, ignore it:
    if(_inflatePrunning && (_hardwareState & 0b00011111)==0) _inflatePrunning=false;
    if(_inflatePrunning && (_hardwareState & 0b00011111)!= ports) return;
    //-------------------------------------------------------
    //If the task has already completed in a previous iteration, return 0 and don't do anything.
    if(inflatePcomplete==true){ //this flag is defined in the FLowIO.h file and is false by default.
      return;
    }else{
      //If we are not inflating OR the active ports do NOT match 'ports' argument, start the inflation:
      if(getHardwareStateOf(INLET)==0 || ((_hardwareState & 0b00011111) ^ ports)!=0){
        startInflation(ports, pwmValue);
        _inflatePrunning = true;
        return;
      }
      //Otherwise, if we are already inflating on the appropriate ports:
      else{
        //check if any pMax has been reached, and then close corresponding port
        float pVal = getPressure(pUnit);
        if(pVal>=pMax1 && getHardwareStateOf(PORT1))  closePorts(0b00000001);
        if(pVal>=pMax2 && getHardwareStateOf(PORT2))  closePorts(0b00000010);
        if(pVal>=pMax3 && getHardwareStateOf(PORT3))  closePorts(0b00000100);
        if(pVal>=pMax4 && getHardwareStateOf(PORT4))  closePorts(0b00001000);
        if(pVal>=pMax5 && getHardwareStateOf(PORT5))  closePorts(0b00010000);
        //Now check if we have closed all the ports, and if yes, then we are done.
        if((_hardwareState & 0b00011111)==0){
          stopAction(ports);
          _inflatePrunning = false;
          inflatePcomplete=true;
          return;
        }
      }
    }
  }
