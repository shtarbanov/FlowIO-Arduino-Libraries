/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"
#include "Adafruit_NeoPixel.h"

//CONSTRUCTOR: Assigns the pins as outputs.
	//NOTE(1): The parameter variables have a scope limited to the consturctor. If we want to use some of those parameter
	//values that are passed by the user in a function other than the constructor, then we must save those values into
	//global variables (which we define with _underscore by convention) which have a scope not limited to the consturctor.
	//NOTE(2): There is a much more fundamental reason we need the these global member variables, and that has to do with
	//how they are stored in memory. When we create a member variable, a pointer is created that points to that variable.

	FlowIO::FlowIO(){	//If no paramter is specified in the constructor, default to GENERAL.
		_setConfig(GENERAL);
	}
	FlowIO::FlowIO(Configuration mode){
		_setConfig(mode);
	}

	//Hardware Configuration Mode
	void FlowIO::_setConfig(Configuration config){
		_config = config; //set global variable to the parameter value.
		pinMode(_inletValvePin, OUTPUT);
		analogWrite(_inletValvePin,0);
		pinMode(_outletValvePin, OUTPUT);
		analogWrite(_outletValvePin,0);
		for(int i=0; i < 2; i++){
			pinMode(_pumpPins[i], OUTPUT);
			analogWrite(_pumpPins[i],0);
		}
		for(int i=0; i < 5; i++){
			pinMode(_portValvePins[i], OUTPUT);
			analogWrite(_portValvePins[i],0);
		}
		pinMode(blueLEDpin,OUTPUT);
		// pinMode(redLEDpin,OUTPUT);
	}
	void FlowIO::setConfig(Configuration config){ //Notice that this is different from "_setConfig"
		_config = config;
	}
	Configuration FlowIO::getConfig(){
		return _config;
	}
