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
	void FlowIO::openInletValve(){
		digitalWrite(_inletValvePin,HIGH);
		set_bit(5,_hardwareState);
	}
	void FlowIO::closeInletValve(){
		digitalWrite(_inletValvePin,LOW);
		clear_bit(5,_hardwareState);
	}	
	void FlowIO::openOutletValve(){
		digitalWrite(_outletValvePin,HIGH);
		set_bit(6,_hardwareState);
	}
	void FlowIO::closeOutletValve(){
		digitalWrite(_outletValvePin,LOW);
		clear_bit(6,_hardwareState);
	}
	void FlowIO::setPorts(uint8_t ports){
  		/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
		//char portsArray[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
		char portsArray[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
			for(int i=0; i<5; i++){
				digitalWrite(_portValvePins[i],portsArray[i]); //element 0 of the pins array is port 1.
				(portsArray[i]==1) ? set_bit(i,_hardwareState) : clear_bit(i,_hardwareState);
			}
  	}
  	void FlowIO::openPorts(uint8_t ports){ //opens only the ports having a 1 in their bits. Leaves the others unchanged.
  		/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
  		//char portsArray[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
  		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
  		char portsArray[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
		for(int i=0; i<5; i++){
			if(portsArray[i]==0x01){ //only act on the ports that are 1. We want to set them to 1.
				digitalWrite(_portValvePins[i], 1); //element 0 of _portValvePins is port 1.
				set_bit(i,_hardwareState);
			}	
		}
  	}
  	void FlowIO::closePorts(uint8_t ports){ //closes only the ports with a 1 in the arugment byte. Leaves the others unchanged.
  		/*Use the following line if you want bit 0 of ports to correspond to port# 5. */
  		//char portsArray[5] = {ports>>4 & 0x01, ports>>3 & 0x01, ports>>2 & 0x01, ports>>1 & 0x01, ports & 0x01};
  		/*Use the following line if you want bit 0 of ports to correspond to port# 1. */
  		char portsArray[5] = {ports & 0x01, ports>>1 & 0x01, ports>>2 & 0x01, ports>>3 & 0x01, ports>>4 & 0x01};
  			//element 0 of portsArray corresponds to the LSB of 'ports' which we now say is port 1
		for(int i=0; i<5; i++){
			if(portsArray[i]==0x01){ //only act on the ports that are 1. We want to set them to 0.
				digitalWrite(_portValvePins[i], 0); //element 0 of the pins array is port 1.
				clear_bit(i,_hardwareState);
			}	
		}
	}
