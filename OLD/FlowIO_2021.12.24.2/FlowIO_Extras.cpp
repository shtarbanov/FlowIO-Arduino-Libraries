/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"

//EXTRAS APIs: functions to perform time-dependent actions (these are based on the above, and not on the driver directly)
//------NOTE: THESE FUNCTIONS ARE BLOCKING EXECUTION (they include delay() elements.)
  	void FlowIO::inflateT(uint8_t ports, int millisec, uint8_t pwmValue){ //this is a blocking function
		if(_config == GENERAL || _config == INFLATION_PARALLEL || _config == INFLATION_SERIES){
			if(millisec > 0 && ports<<3 != 0){
				startInflation(ports, pwmValue);
				delay(millisec);
				stopAction(ports);
			}
		}
	}
	void FlowIO::releaseT(uint8_t ports, int millisec, uint8_t pwmValue){ //this is a blocking function
		if(millisec > 0 && ports<<3 != 0){
			startRelease(ports);
			delay(millisec);
			stopAction(ports);
		}
	}
	void FlowIO::vacuumT(uint8_t ports, int millisec, uint8_t pwmValue){ //this is a blocking function
		if(_config == GENERAL || _config == VACUUM_PARALLEL || _config == VACUUM_SERIES){
			if(millisec > 0 && ports<<3 != 0){
				startVacuum(ports, pwmValue);
				delay(millisec);
				stopAction(ports);
			}
		}
	}

//EXTRAS API: Presure Dependent Methods (BLOCKING)

	//I will require pressure unit to be specified explicity, because otherwise user may forget what units
	//they had set and accidentally start working in different units, causing something to blow up. Moreover,
	//when units are specified explicity, this enables one to have different inflation operations happening in
	//different units, say if someone has inflatables that are rated at different pressure systems.
	void FlowIO::inflateP(uint8_t ports, float pMax, Unit pUnit, uint8_t pwmValue){ //the value of pMax is of unit as specified in the units function.
		if(getPressure(pUnit)<pMax) startInflation(ports, pwmValue);
		while(getPressure(pUnit)<pMax){
			delay(1);
		}
		stopAction(ports);
	}
