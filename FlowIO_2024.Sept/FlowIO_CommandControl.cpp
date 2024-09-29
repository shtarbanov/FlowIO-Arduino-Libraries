/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"

//COMMAND CONTROL API: 3-byte based control. This is an implementation of our communication protocol at the library level. The "command()" method
	//allows you to control just about any feature of the FlowIO device, based on what arguments you provide. The first argument 'action' is
	//describing what to do. The second argument is describing on which ports that action is to be performed based on the position and
	//value of the individual bits, namely the first 5 bits. The third parameter is the PWM value if applicable.

	//Implementation of the control state machine (TUTORIAL ON STATE MACHINES: https://hackingmajenkoblog.wordpress.com/2016/02/01/the-finite-state-machine/)
	float FlowIO::command(uint8_t action, uint8_t ports, uint8_t pwmValue){ //returns the internal pressure
		//Here I am going to extract information from the port parameter's bits.
		float pressure = 999.9f;
		switch(action){
			case '!':
				stopAction(ports);
				break;
			case '+':
				startInflation(ports, pwmValue); //if the config state is not right, this won't do anything, in which case we also don't want to read pressure.
				break;
			case '-':
				startVacuum(ports, pwmValue);
				break;
			case 'p': //inflate2x
				startInflationHalfCapacity(ports, pwmValue);
				break;
			case 'n': //vacuum2x
				startVacuumHalfCapacity(ports, pwmValue);
				break;
			case '^':
				startRelease(ports);
				break;
			case 'o':
				openPorts(ports);
				break;
			case 'c':
				closePorts(ports);
				break;
			case 'f':
				if(ports=='f') powerOFF(); //power off when 'ff' is sent.
				break;
			case 'r':
				// redLED(ports); //if ports>0 led=on, if port==0 led=off.
				if(ports>0) pixel(10,0,0);
				break;
			case 'b':
				blueLED(ports);
				break;
			case '?':
				if(ports=='?'){ //Only the command '??' requests a new pressure reading.
					pressure = getPressure();
					//The ways to normally use this is to start some action, then immediately start calling
					//'??' to get continous pressure readings as the action is undergoing.
				}else{		 //'?#' would open port '#' and after 10ms measure the pressure on that port only.
					closeInletValve();
					closeOutletValve();
					stopPump(1);
					stopPump(2);
					setPorts(ports);
					delay(10); //8ms is too little. 9ms is maybe Ok. 10ms is safe.
					pressure = getPressure();
				}
				break;
			default:
				// This is where you can handle what happens if "action" is invalid.
				break;
		}
		//We want to get the pressure only in some states and NOT in others. And we want to return 999.9
		//for pressure if a command like 'r' is sent. We return a valid presure only for commands
		//that make sense to return a pressure reading.
		//There is still a bit of an issue because no matter what, I will always get a pressure value out.
		//If not valid commands are passed, the returned pressure will be the default value of 999.9f.
		return pressure;
	}
