/* FlowIO.h - a library for controlling the pneumatic circuit
	(I need to add this circuit board and the Tiny8SSR to my portfolio website)
   Created by Ali Shtarbanov, Sept. 2019
*/

#ifndef FlowIO_h
#define FlowIO_h

#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_NeoPixel.h"

#define MPRLS_POWERED        (0b01000000) ///< Status SPI powered bit
#define MPRLS_BUSY           (0b00100000) ///< Status busy bit
#define MPRLS_INTEGRITY_FAIL (0b00000100) ///< Status bit for integrity fail
#define MPRLS_SATURATION     (0b00000001) ///< Status bit for math saturation

#define blueLEDpin 4
#define powerOFFpin 13
#define neoPixelPin 8

#define _BV(bit) (1<<bit) //bv = bit value
#define set_bit(bit,var) 	(var |=  _BV(bit))
#define clear_bit(bit,var) 	(var &= ~_BV(bit))
#define toggle_bit(bit,var)	(var ^=  _BV(bit))

enum Unit : uint8_t{
	PSI, HPA, ATM
};
enum Configuration : uint8_t{ //pneumatic configuration mode
	GENERAL, INFLATION_SERIES, VACUUM_SERIES, INFLATION_PARALLEL, VACUUM_PARALLEL
};

enum Component : uint8_t{
	PORT1, PORT2, PORT3, PORT4, PORT5, INLET, OUTLET, PUMP1, PUMP2, LEDRED, LEDBLUE, SENSOR
   //bit0, bit1,  bit2,  bit3,  bit4,  bit5,  bit6,   bit7,  bit8,  bit8,   bit10,   bit11.
};

class FlowIO{
private:
	uint8_t _pumpPins[2]={15,16};
	uint8_t _portValvePins[5]={12,11,10,9,6};	//left pin is port 1.
	uint8_t _inletValvePin=5; //(right) valve
	uint8_t _outletValvePin=14; //(left) valve
	Configuration _config;
	bool _i2cInitialized=false; //becomes true when the activateSensor() function has been executed.
	void _setConfig(Configuration config);
	uint8_t _addr = 0x18;
  TwoWire *_i2c;
  uint8_t statusByte;
  uint8_t _getStatusByte(void);
  uint32_t _getRawPressure(void);
  float _getPressureHPA();
  float _getPressurePSI();
  float _getPressureATM();
  Unit _pressureUnit=PSI;

  uint16_t _hardwareState = 0; /*this 16-bit variable will hold all info about what all the
  hardware is currently doing. Each bit corresponds to a hardware feature as follows:
   _______________________
  |bit| Component |'1' is |
  |---|-----------|-------|
  | 0 |  PORT1    | open 	|
  | 1 |  PORT2    | onen 	|
  | 2 |  PORT3    | open 	|
  | 3 |  PORT4    | open 	|
  | 4 |  PORT5    | open 	|
  | 5 |  INLET    | open 	|
  | 6 |  OUTLET   | open 	|
  | 7 |  PUMP1    | Active|
  | 8 |  PUMP2    | Active|
  | 9 |  LEDRED   | ON 	|
  | 10|  LEDBLUE  | ON 	|
  | 11|  SENSOR   | Active|
  | 12|           |		|
  | 13|           |		|
  | 14|           |		|
  | 15|           |		|
  |---|-----------|-------|
  */

  uint8_t _errorCode = 0; //this will change value if an error occurs.

public:
	//CONSTRUCTORS
	FlowIO(); //TODO: Check if I can just use a single constructor FlowIO(Configuration config=GENERAL);
	FlowIO(Configuration config);
	void setConfig(Configuration config);
	Configuration getConfig();

	//PRESSURE SENSING
	bool activateSensor();
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


	//EXTRAS: Time Dependent Methods. (Blocking)
	void inflateT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void inflate2xT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void vacuumT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void vacuum2xT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function
	void releaseT(uint8_t ports, int millisec, uint8_t pwmValue=255); //blocking function

	//EXTRAS: Presure Dependent Methods
	void inflateP(uint8_t ports, float pMax, Unit pUnit, uint8_t pwmValue=255);

};

#endif
