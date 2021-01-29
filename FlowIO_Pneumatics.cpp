#include "Arduino.h"
#include "FlowIO.h"

//PNEUMATICS API: functions to perform actions involving more than one element.
	void FlowIO::startInflation(uint8_t ports, uint8_t pwmValue){
		stopAction(ports);
		if(_config == GENERAL || _config == INFLATION_PARALLEL || _config == INFLATION_SERIES){
			if(ports<<3 != 0){ //if the ports are all 0, then don't do anything
				openInletValve(); 	//(If we don't have an inlet valve, it is ok b/c then this will act on pin "0")
				setPorts(ports);
				startPump(1, pwmValue);
				if(_config != GENERAL) startPump(2, pwmValue);
			}
		}
	}
	void FlowIO::startInflationHalfCapacity(uint8_t ports, uint8_t pwmValue){ //Only applicable for Inflation_Series and Inflation_Parallel configurations.
	  	stopAction(ports);
	  	if(_config == INFLATION_PARALLEL || _config == INFLATION_SERIES){
		  	if(ports<<3 != 0){
		  		openInletValve();
		  		setPorts(ports);
		  		startPump(1, pwmValue);
		  	}
	  	}
	}
	void FlowIO::startVacuum(uint8_t ports, uint8_t pwmValue){
		stopAction(ports);
		if(_config == GENERAL || _config == VACUUM_PARALLEL || _config == VACUUM_SERIES){
			if(ports<<3 != 0){
				openOutletValve();
				setPorts(ports);
				startPump(2,pwmValue);
				if(_config != GENERAL) startPump(1, pwmValue);
			}
		}
	}
	void FlowIO::startVacuumHalfCapacity(uint8_t ports, uint8_t pwmValue){ //Only applicable for Vacuum_Series and Vacuum_Parallel configurations.
		stopAction(ports);
		if(_config == VACUUM_SERIES || _config == VACUUM_PARALLEL){
			if(ports<<3 != 0){
				openOutletValve();
				setPorts(ports);
				startPump(1, pwmValue);
			}
		}
	}
	void FlowIO::startRelease(uint8_t ports){
		stopAction(ports);
		if(ports<<3 != 0){
			setPorts(ports);
			//We must open both valves. The outlet valve releases high pressure, the inlet valve releases low pressure.
			openOutletValve();
			openInletValve();
		}
	}
	void FlowIO::stopAction(uint8_t ports){
		closeInletValve();
		closeOutletValve();
		//If we are stopping the action on port 5, we naturally would type stopAction(0b00000001), which means that
		//we want to CLOSE port 5. BUT we don't want to change any of the other 4 ports. Thus we use closePorts rather than setPorts().
		closePorts(ports); //this only closes the ports indicated with bit value 1.
	  	stopPump(1);
		stopPump(2);
	}
