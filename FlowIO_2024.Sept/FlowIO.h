/* Written by Ali Shtarbanov */

#ifndef FlowIO_h
#define FlowIO_h

#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_NeoPixel.h"

#define blueLEDpin 4
#define powerOFFpin 13
#define neoPixelPin 8

#define _BV(bit) (1<<bit) //bv = bit value
#define set_bit(bit,var) 	(var |=  _BV(bit))
#define clear_bit(bit,var) 	(var &= ~_BV(bit))
#define toggle_bit(bit,var)	(var ^=  _BV(bit))

enum Unit : uint8_t{
	PSI, ATM, MBAR, KPA, PA
};
enum Configuration : uint8_t{ //pneumatic configuration mode
	GENERAL, INFLATION_SERIES, VACUUM_SERIES, INFLATION_PARALLEL, VACUUM_PARALLEL
};

enum Component : uint8_t{
	PORT1, PORT2, PORT3, PORT4, PORT5, INLET, OUTLET, PUMP1, PUMP2, bit9, LEDBLUE, SENSOR 
   //bit0, bit1,  bit2,  bit3,  bit4,  bit5,  bit6,   bit7,  bit8,  bit9,   bit10,   bit11
};

class FlowIO{
private:
	uint8_t _pumpPins[2]={15,16};
	uint8_t _portValvePins[5]={12,11,10,9,6};	//left pin is port 1.
	uint8_t _inletValvePin=5; //(right) valve
	uint8_t _outletValvePin=14; //(left) valve
	//NOTE: It is tempting to consolidate the valve pins into a single array. But having things this way allows
	//me to change which pins I use for inlet and outet without having to change anything else in the code.
	//Another reason we want the ports to start from 0 is because we want the last 5 bits of the "ports"
	//argument variable to correspond to the 5 ports; thus bit 0 corresponds to index 0, to port 1.
	Configuration _config;
	void _setConfig(Configuration config);

	//Pressure Sensor
	TwoWire *_i2c;
	bool _i2cInitialized=false; //becomes true when the activateSensor() function has been executed.
  Unit _pressureUnit=PSI;
	uint16_t C[8];
	uint32_t D1_pres, D2_temp;
	int32_t TEMP;
	int32_t P;
	void calculate(); //Performs calculations per the sensor data sheet for conversion and second order compensation.
	uint8_t crc4(uint16_t n_prom[]);

	//Status indicators
	bool _powerOptimized[7]={0,0,0,0,0,0,0}; //holds 0 if a valve is not in power optmized state.
		//Holds 1 if a valve is in an optimized power ON state. Holds 0 in if a valve is off or if it is on
		//but not yet in an optimized power state. The values of this array are changed by the driver functions
		//and immediately after an analogWrite() function is called on any valve.
		//The indexing matches the hardwareState table: {p1,p2,p3,p4,p5,inlet,outlet}
		//TODO: You can change this to a byte instead of an array and store the information in the bits.
		//then you can simply use the set_bit() and clear_bit() function to change the desired bits only.
	uint32_t _startTimes[9]={0,0,0,0,0,0,0,0,0}; //holds the start time for each ACTIVE valve & pump.
		//The indexing matches the hardwareState table: {p1,p2,p3,p4,p5,inlet,outlet,pump1,pump2}
		//Each value is reset to 0 when the corresponding valve or pump is DEACTIVATED. Thus 0 means off. 
		//This array is used for power optimization.
	uint16_t _hardwareState = 0; /*this 16-bit variable will hold all info about what all the
  hardware is currently doing. Each bit corresponds to a hardware feature as follows:
   _____________________________________________________________________
  |bit| Component |'1' is 	|  		set by		| 		reset by		|
  |---|-----------|---------|-------------------|-----------------------|
  | 0 |  PORT1    | open 	| The 5 port bits are set / reset by any of	|
  | 1 |  PORT2    | onen 	| the three port functions in the driver:	|
  | 2 |  PORT3    | open 	| 				  .....						|
  | 3 |  PORT4    | open 	| 	setPorts(); openPorts(); closePorts();	|
  | 4 |  PORT5    | open 	| 				  .....						|
  | 5 |  INLET    | open 	| openInletValve();	| closeInletValve(); 	|
  | 6 |  OUTLET   | open 	| openOutletValve();| closeOutletValve();	|
  | 7 |  PUMP1    | Active	| startPump(); 		| stopPump();			|
  | 8 |  PUMP2    | Active	| startPump(); 		| stopPump();			|
  | 9 |  		  |			|					|						|
  | 10|  LEDBLUE  | ON 		| blueLED(1);	 	| blueLED(0);			| 
  | 11|  SENSOR   | Active	|	??????		  .....						|
  | 12|           |			|				  .....						|
  | 13|           |			|				  .....						|
  | 14|           |			|				  .....						|
  | 15| 		  |			|											| 
  |---------------------------------------------------------------------|
  */

	uint8_t _pwmHoldPrev = 170; //global variable that will hold the previous valve holding voltage PWM,
		//so that optimizePower() function can know if it has been reinvoked with a different pwmHold argument.
		//This is useful in case we want to change the holding current while the valve is still open by simply
		//reinvoking the optimizePower() function with a different argument, without having to close a valve first.
  	uint8_t _errorCode = 0; //this will change value if an error occurs.

	//Private Flags
	bool _inflatePrunning = false; 
	uint32_t _timeOfLastStateChange = millis();

public:
	//Literals
	float pressure_mbar=-1; //Pressure returned in mbar.
	float pressure_psi=-1;
	float pressure_atm=-1;
	float pressure_pa=-1; //pascal
	float pressure_kpa=-1; //kiloPascal
	float temp_c=-1; // Temperature returned in deg C

	//Public Flags
	bool inflatePcomplete=false;

	//CONSTRUCTORS
	FlowIO(); //TODO: Check if I can just use a single constructor FlowIO(Configuration config=GENERAL);
	FlowIO(Configuration config);
	void setConfig(Configuration config);
	Configuration getConfig();

	//PRESSURE SENSING
	bool activateSensor();
	void requestPressure();
	void setPressureUnit(Unit pUnit); //default is PSI.
	float getPressure();
	float getPressure(Unit pUnit);

	//PNEUMATICS: functions to perform actions involving more than one element.
	void startInflation(uint8_t ports, uint8_t pwmValue=255);
	void startInflationHalfCapacity(uint8_t ports, uint8_t pwmValue=255);
	void startVacuum(uint8_t ports, uint8_t pwmValue=255);
	void startVacuumHalfCapacity(uint8_t ports, uint8_t pwmValue=255);
	void startRelease(uint8_t ports);
	void stopAction(uint8_t ports);

	//DRIVER: functions to control individual pneumatic elements
	void stopPump(uint8_t pumpNumber); //sets the pwm value to 0.
	void startPump(uint8_t pumpNumber, uint8_t pwmValue=255); //sets the pumpt to a PWM value
	void openInletValve();
	void closeInletValve();
	void openOutletValve();
	void closeOutletValve();
	void setPorts(uint8_t ports); //Opens the ports with 1. Closes the ports with 0.
	void openPorts(uint8_t ports); //Opens the ports with 1. Leaves the ports with ports with 0 unchanged.
	void closePorts(uint8_t ports); //Closes ports with 1. Leaves the ports with 0 unchanged.
	void optimizePower(uint8_t holdPWM, uint16_t thresholdTime=500);

	//COMMAND CONTROL
	//TODO: The command based control can only return pressure, but has no ways of
	// returning the hardware status. May this function can simply return a pointer,
	//and depending on the request that pointer can point to an int, float, or null.
	float command(uint8_t action, uint8_t ports, uint8_t pwmValue=255);

	//INDICATORS
	uint16_t getHardwareState();
	bool getHardwareStateOf(uint8_t bitNumber);
	bool getHardwareStateOf(Component name);
	uint8_t readError();
	void raiseError(uint8_t errorCode);
	void blueLED(bool power);
	// void redLED(bool power);
	void pixel(uint8_t r, uint8_t g, uint8_t b);
	void powerOFF();
	uint32_t getStartTimeOf(Component name);
	uint32_t getTimeOfLastStateChange(); //the last time when a valve or pump was activated or deactivated.


	//FEEDFORWARD CONTROL: Time Dependent Methods. (Blocking)
	void inflateT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void inflate2xT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void vacuumT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void vacuum2xT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void releaseT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function

	//FEEDBACK CONTROL: Presure Dependent Methods (non-blocking)
	uint32_t inflateP(uint8_t ports, float pMax, Unit pUnit, uint8_t pwmValue=255);
	void inflateP_diff(uint8_t ports, float pMax1, float pMax2, float pMax3, float pMax4, float pMax5, Unit pUnit, uint8_t pwmValue=255);
};

#endif
