/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"

//FEEDFORWARD CONTROL: Time Dependent Methods. (Blocking)
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
