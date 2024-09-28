/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"
#include "Adafruit_NeoPixel.h"

//INDICATORS APIs
  void FlowIO::blueLED(bool power){
		digitalWrite(blueLEDpin,power);
		(power) ? set_bit(10,_hardwareState) : clear_bit(10,_hardwareState);
	}
  void FlowIO::pixel(uint8_t r, uint8_t g, uint8_t b){
		Adafruit_NeoPixel _thePixel(1,neoPixelPin); //1 pixel on pin 8
    _thePixel.begin();
    // _thePixel.setPixelColor(0, _thePixel.Color(0,0,0));
    _thePixel.show(); //If we don't have this show() we get a bad result the first time pixel() is invoked.
      //We can put this either here or after the next line, but we must have 2 of the .show() functions.
    _thePixel.setPixelColor(0, _thePixel.Color(r,g,b));
    _thePixel.show(); //Having the second show() overrides the bad result with the correct color.
	}
//Deprecated due to conflit with the Power button
// void FlowIO::redLED(bool power){
// 	digitalWrite(redLEDpin,power);
// 	(power) ? set_bit(9,_hardwareState) : clear_bit(9,_hardwareState);
// }
	void FlowIO::powerOFF(){
		pinMode(powerOFFpin,OUTPUT);
		digitalWrite(powerOFFpin,HIGH);
	}
	uint16_t FlowIO::getHardwareState(){
		return _hardwareState;
	}
	bool FlowIO::getHardwareStateOf(uint8_t bitNumber){
		//Parse the nth bit and return true or false
		uint16_t one = 1; //0x0001.
    	bool returnValue = (_hardwareState>>bitNumber & one);
		return returnValue;
	}
	bool FlowIO::getHardwareStateOf(Component name){
		uint16_t one = 1; //0x0001.
		//PORT1 corresponds to 0, PORT2 to 1, and so on as defined by the
		//order of the items in the enum definition. Hence, we don't need
		//to check for each individual name. Instead of having:
		//if (name==PORT1) return (_hardwareState>>0 & one);
		//we can simply have
		//return (_hardwareState>>name & one);
		//We must test if this actually works

		// if	  (name==PORT1)	return (_hardwareState>>0 & one);
		// else if(name==PORT2)	return (_hardwareState>>1 & one);
		// else if(name==PORT3)	return (_hardwareState>>2 & one);
		// else if(name==PORT4)	return (_hardwareState>>3 & one);
		// else if(name==PORT5)	return (_hardwareState>>4 & one);
		// else if(name==INLET)	return (_hardwareState>>5 & one);
		// else if(name==OUTLET)	return (_hardwareState>>6 & one);
		// else if(name==PUMP1)	return (_hardwareState>>7 & one);
		// else if(name==PUMP2)	return (_hardwareState>>8 & one);
		// else if(name==LEDRED)	return (_hardwareState>>9 & one);
		// else if(name==LEDBLUE)	return (_hardwareState>>10 & one);
		// else if(name==SENSOR)	return (_hardwareState>>11 & one);

		bool returnValue = (_hardwareState>>name & one);
		return returnValue;
	}
	uint32_t FlowIO::getTimeOfLastStateChange(){
		return _timeOfLastStateChange;
	}
	uint8_t FlowIO::readError(){
		return _errorCode;
	}
	void FlowIO::raiseError(uint8_t errorCode){
		_errorCode = errorCode;
	}
	uint32_t FlowIO::getStartTimeOf(Component name){
		if	   (name==PORT1)	return _startTimes[0];
		else if(name==PORT2)	return _startTimes[1];
		else if(name==PORT3)	return _startTimes[2];
		else if(name==PORT4)	return _startTimes[3];
		else if(name==PORT5)	return _startTimes[4];
		else if(name==INLET)	return _startTimes[5];
		else if(name==OUTLET)	return _startTimes[6];
		else if(name==PUMP1)	return _startTimes[7];
		else if(name==PUMP2)	return _startTimes[8];
		else return 0;
	}
