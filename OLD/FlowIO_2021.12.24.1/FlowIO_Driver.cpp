/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"

//DRIVER APIs: functions to control individual elements
	//It is critical to ABSOLUTELY NEVER access a negative array element, because
	//we will get content in the previous memory location right before the array!
	//If pumpNumber=0 and we subtract 1, that would be bad, and could lead to physical damage because
	//if that memory happens to hold a valid IO pin number, that could be disastrous!
	//This bug is very difficult to notice becase it may cause things to behave badly only sometimes.

	void FlowIO::startPump(uint8_t pumpNumber, uint8_t pwmValue){
		if(pumpNumber==1 || pumpNumber==2){
			analogWrite(_pumpPins[pumpNumber-1],pwmValue);
			//update the _hardwareState variable
			if(pwmValue>0)  set_bit(6+pumpNumber,_hardwareState); //bit 7 is pump1, bit8 is pump2
			else			clear_bit(6+pumpNumber,_hardwareState); //bit 7 is pump1, bit8 is pump2
		}
	}
	void FlowIO::stopPump(uint8_t pumpNumber){
	  	if(pumpNumber==1 || pumpNumber==2){
	  		analogWrite(_pumpPins[pumpNumber-1],0);
			//update the _hardwareState variable
	  		clear_bit(6+pumpNumber,_hardwareState); //bit 7 is pump1, bit8 is pump2
		}
	}

	//NOTE: Don't use digitalWrite for the below functions, because though it will work initially,
	//it will fail after you enter the analogWrite power saving mode and you want to return to the
	//digitalWrite on the next cycle. That is, unless the firmware has been now fixed by Adafruit
	//to not have that bug.

	void FlowIO::openInletValve(){
		analogWrite(_inletValvePin,255);
		_powerOptimized[5]=false; //reset the _powerOptimized tracker.
		_startTimes[5]=millis();
		set_bit(5,_hardwareState);
	}
	void FlowIO::closeInletValve(){
		analogWrite(_inletValvePin,0);
		_powerOptimized[5]=false; //reset the _powerOptimized tracker.
		clear_bit(5,_hardwareState);
		_startTimes[5]=0;
	}

	void FlowIO::openOutletValve(){
		analogWrite(_outletValvePin,255);
		_powerOptimized[6]=false; //reset the _powerOptimized tracker.
		set_bit(6,_hardwareState);
		_startTimes[6]=millis();
	}
	void FlowIO::closeOutletValve(){
		analogWrite(_outletValvePin,0);
		_powerOptimized[6]=false; //reset the _powerOptimized tracker.
		clear_bit(6,_hardwareState);
		_startTimes[6]=0;
	}
	void FlowIO::setPorts(uint8_t ports){
  	/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
		//char portsValue[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
		char portsValue[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
		for(int i=0; i<5; i++){
			analogWrite(_portValvePins[i],portsValue[i]*255); //element 0 of the pins array is port 1
			_powerOptimized[i]=false; //reset the _powerOptimized tracker.
			(portsValue[i]==1) ? set_bit(i,_hardwareState) : clear_bit(i,_hardwareState);
			(portsValue[i]==1) ? _startTimes[i]=millis() : _startTimes[i]=0;
		}
	}
	void FlowIO::openPorts(uint8_t ports){ //opens only the ports having a 1 in their bits. Leaves the others unchanged.
		/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
		//char portsValue[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
		char portsValue[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
		for(int i=0; i<5; i++){
			if(portsValue[i]==0x01){ //only act on the ports that are 1. We want to set them to 1.
				analogWrite(_portValvePins[i], 255); //element 0 of _portValvePins is port 1.
				_powerOptimized[i]=false; //reset the _powerOptimized tracker.
				set_bit(i,_hardwareState);
				_startTimes[i]=millis();
			}
		}
	}
	void FlowIO::closePorts(uint8_t ports){ //closes only the ports with a 1 in the arugment byte. Leaves the others unchanged.
		/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
		//char portsValue[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
		char portsValue[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
			//element 0 of portsValue corresponds to the LSB of 'ports' which we now say is port 1
		for(int i=0; i<5; i++){
			if(portsValue[i]==0x01){ //only act on the ports that are 1. We want to set them to 0.
				analogWrite(_portValvePins[i], 0); //element 0 of the pins array is port 1.
				_powerOptimized[i]=false; //reset the _powerOptimized tracker.
				clear_bit(i,_hardwareState);
				_startTimes[i]=0;
			}
		}
	}
	void FlowIO::optimizePower(uint8_t pwmHold, uint16_t thresholdTime){
		//if there are any open valves that have been open for at least thresholdTime,
		//reduce the current to the holding current, holdPWM. The second is set to 500ms
		//by default, which would be more than enough for even very high pressures.
		int timeNow = millis(); //TODO: Remove this by using millis() in the if block below directly.
		for(int i=0; i<7; i++){
			//First we check if the user wants to change the holding current at runtime or between iterations:
			//And if some of our valves are already at an optimized power ON state, and the user changes the hold
			//current, then we want to act even on those already-opened valves as well:
			if(pwmHold!=_pwmHoldPrev){ //if there is a change in the "pwmHold" argument,
				if(_powerOptimized[i]==true){ //if a valve is already in an optimized ON state, apply the new pwm value:
					if(i<5) analogWrite(_portValvePins[i], pwmHold);
					else if(i==5) analogWrite(_inletValvePin, pwmHold);
					else if(i==6) analogWrite(_outletValvePin, pwmHold);
				}
			}
			//Now we just check for the case of newly opened valves that are not in the hold state yet:
			if(_startTimes[i]>0 && _powerOptimized[i]==false){ //if a valve is open but not in an optimized state:
				if(timeNow-_startTimes[i] > thresholdTime){
					if(i<5) analogWrite(_portValvePins[i], pwmHold);
					else if(i==5) analogWrite(_inletValvePin, pwmHold);
					else if(i==6) analogWrite(_outletValvePin, pwmHold);
					_powerOptimized[i]=true; //flag the valve as being in the optimized hold state.
				}
			}
		}
		_pwmHoldPrev = pwmHold;
		//NOTE: _powerOptimized[] get reset to 0 by the driver action functions.
	}
