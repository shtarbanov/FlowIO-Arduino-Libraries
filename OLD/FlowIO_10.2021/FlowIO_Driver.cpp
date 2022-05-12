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
		_valveOpenedAt[5]=millis();
		set_bit(5,_hardwareState);
	}
	void FlowIO::closeInletValve(){
		analogWrite(_inletValvePin,0);
		clear_bit(5,_hardwareState);
		_valveOpenedAt[5]=0;
	}

	void FlowIO::openOutletValve(){
		analogWrite(_outletValvePin,255);
		set_bit(6,_hardwareState);
		_valveOpenedAt[6]=millis();
	}
	void FlowIO::closeOutletValve(){
		analogWrite(_outletValvePin,0);
		clear_bit(6,_hardwareState);
		_valveOpenedAt[6]=0;
	}
	void FlowIO::setPorts(uint8_t ports){
  	/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
		//char portsValue[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
		char portsValue[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
		for(int i=0; i<5; i++){
			analogWrite(_portValvePins[i],portsValue[i]*255); //element 0 of the pins array is port 1.
			(portsValue[i]==1) ? set_bit(i,_hardwareState) : clear_bit(i,_hardwareState);
			(portsValue[i]==1) ? _valveOpenedAt[i]=millis() : _valveOpenedAt[i]=0;
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
				set_bit(i,_hardwareState);
				_valveOpenedAt[i]=millis();
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
				clear_bit(i,_hardwareState);
				_valveOpenedAt[i]=0;
			}
		}
	}
	void FlowIO::optimizePower(uint8_t pwmHold, uint16_t thresholdTime){
		//if there are any open valves that have been open for at least 2x(switching time),
		//reduce the current to the holding current of holdPWM. The time is set to 500ms
		//by default, which should be more than plenty.

		//I must implement this function in such a way, that if reinvoked with a new
		//pwmHold value, it will reexecute the AnalogWrite() for all valves that are
		//above the time threshold AND for the valves that are already in the hold state.

		int timeNow = millis();
		for(int i=0; i<7; i++){
			if(pwmHold!=_pwmHoldPrev){ //if there is a change in the "pwmHold" argument,
				if(_valveOpenedAt[i]==-1){ //and if a valve is already in the hold state, then apply the new pwm value:
					if(i<5) analogWrite(_portValvePins[i], pwmHold);
					else if(i==5) analogWrite(_inletValvePin, pwmHold);
					else if(i==6) analogWrite(_outletValvePin, pwmHold);
				}
			}
			//Now we just check for the case of newly opened valves that are not in the hold state yet:
			if(_valveOpenedAt[i]>0){ //only consider valves which are recently opened.
				if(timeNow-_valveOpenedAt[i] > thresholdTime){
					if(i<5) analogWrite(_portValvePins[i], pwmHold);
					else if(i==5) analogWrite(_inletValvePin, pwmHold);
					else if(i==6) analogWrite(_outletValvePin, pwmHold);
					_valveOpenedAt[i]=-1; //flag the valve as being in the optimized hold state.
				}
			}
		}

		_pwmHoldPrev = pwmHold;
	}
