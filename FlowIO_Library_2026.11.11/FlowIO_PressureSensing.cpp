/*Written by Ali Shtarbanov*/

#include "Arduino.h"
#include "FlowIO.h"

#define MS5837_ADDR 			0x76 //I2C address of the sensor
#define MS5837_RESET			0x1E //I2C Command: Reset
#define MS5837_ADC_READ	 	0x00 //I2C Command: Read
#define MS5837_PROM_READ 	0xA0 //I2C Command: Read PROM
//The following are commands for getting D1 and D2 at different oversampling ratios (OCR).
//https://www.mouser.com/datasheet/2/418/5/NG_DS_MS5837-30BA_B1-1130109.pdf
//There are different computational times (delays) required for the different OCR Values
//which can be found in the datasheet. I am using the minimum value, for which dT=0.6ms,
//but the minimum delay in the code that is required is 2ms for some reason.
#define MS5837_CONVERT_D1_256 	0x40//I2C Command: Request D1 Conversion @ OCR=256 (min)
#define MS5837_CONVERT_D2_256 	0x50//I2C Command: Request D2 Conversion @ OCR=256 (min)
#define MS5837_CONVERT_D1_512 	0x42
#define MS5837_CONVERT_D2_512 	0x52
#define MS5837_CONVERT_D1_1024 	0x44
#define MS5837_CONVERT_D2_1024 	0x54
#define MS5837_CONVERT_D1_2048 	0x46
#define MS5837_CONVERT_D2_2048 	0x56
#define MS5837_CONVERT_D1_4096 	0x48
#define MS5837_CONVERT_D2_4096 	0x58
#define MS5837_CONVERT_D1_8192 	0x4A//I2C Command: Request D1 Conversion @ OCR=8192 (max)
#define MS5837_CONVERT_D2_8192 	0x5A//I2C Command: Request D2 Conversion @ OCR=8192 (max)

//PRESSURE SENSING APIs:
	bool FlowIO::activateSensor(){
		_i2c = &Wire;
		_i2c->begin();
		delay(10);

		// Reset the MS5837, per datasheet. And also check if device is present on the bus
		//by looking at the value returned from .endTransmission(). Note that nothing is sent
		//to device until we call endTransmission(), so we must wait after calling it.
		_i2c->beginTransmission(MS5837_ADDR);
		_i2c->write(MS5837_RESET);
		_errorCode = _i2c->endTransmission(); //_errorCode=2 if not connected
		delay(10);
		if(_errorCode!=0){
			_i2cInitialized=false;
			return false;
		}

		// Read calibration values and CRC. Writing 8 bytes, and reading back 2 after each.
		for ( uint8_t i = 0 ; i < 7 ; i++ ) {
			_i2c->beginTransmission(MS5837_ADDR);
			_i2c->write(MS5837_PROM_READ+i*2);
			_i2c->endTransmission();

			_i2c->requestFrom(MS5837_ADDR,2);
			C[i] = (_i2c->read() << 8) | _i2c->read();
		}
		// Verify that data is correct with Cyclic Redundancy Check (CRC)
		uint8_t crcRead = C[0] >> 12;
		uint8_t crcCalculated = crc4(C);

		if ( crcCalculated != crcRead ) {
			_i2cInitialized=false;
			if(_errorCode==0) _errorCode = 200; //Cyclic Redundancy Check failed.
			return false; // CRC fail
		}
		_i2cInitialized=true;
		return true;
	}

	void FlowIO::requestPressure(){
		//Display -1 for all values if we can't reach the calculate() method at the bottom.
		pressure_mbar=-1; //Pressure returned in mbar.
		pressure_psi=-1;
		pressure_pa=-1; //pascal
		pressure_kpa=-1; //kiloPascal
		pressure_atm=-1;
		temp_c=-1; // Temperature returned in deg C

		//Check if the the sensor has been initialized successfully
		if(_i2cInitialized==false){
			//If we are here and _errorCode is still 0, then the problem is user error.
			if(_errorCode==0) _errorCode = 201; //"You did not initilize sensor"
			return;
		}

		// Request D1 conversion
		_i2c->beginTransmission(MS5837_ADDR);
		_i2c->write(MS5837_CONVERT_D1_256);
		uint8_t txError = _i2c->endTransmission();
		if(txError){ //if not 0,
			_errorCode = 202; //device got disconnected (after being connected)
			return;
		}
		//If the deice got disconnected and reconnected, reset the errorCode back 0.
		if(_errorCode==202 && txError==0){
			_errorCode=0;
		}

		delay(2); // Time for conversion to compute. Values in datasheet

		_i2c->beginTransmission(MS5837_ADDR);
		_i2c->write(MS5837_ADC_READ);
		_i2c->endTransmission();

		//Request 3 bytes from the sensor, then read three bytes into D1_pres.
		_i2c->requestFrom(MS5837_ADDR,3);
		D1_pres = 0;
		D1_pres = _i2c->read();
		D1_pres = (D1_pres << 8) | _i2c->read();
		D1_pres = (D1_pres << 8) | _i2c->read();

		// Request D2 conversion
		_i2c->beginTransmission(MS5837_ADDR);
		_i2c->write(MS5837_CONVERT_D2_256);
		_i2c->endTransmission();

		delay(2); // Time for conversion to compute. Values in datasheet

		_i2c->beginTransmission(MS5837_ADDR);
		_i2c->write(MS5837_ADC_READ);
		_i2c->endTransmission();

		//Request 3 bytes from the sensor, then read three bytes into D2_temp.
		_i2c->requestFrom(MS5837_ADDR,3);
		D2_temp = 0;
		D2_temp = _i2c->read();
		D2_temp = (D2_temp << 8) | _i2c->read();
		D2_temp = (D2_temp << 8) | _i2c->read();

		if(_errorCode==0) calculate();
	}
	void FlowIO::calculate() {
		// Given C1-C6 and D1, D2, calculated TEMP and P
		// Do conversion first and then second order temp compensation

		int32_t dT = 0;
		int64_t SENS = 0;
		int64_t OFF = 0;
		int32_t SENSi = 0;
		int32_t OFFi = 0;
		int32_t Ti = 0;
		int64_t OFF2 = 0;
		int64_t SENS2 = 0;

		// Terms called
		dT = D2_temp-uint32_t(C[5])*256l;
		SENS = int64_t(C[1])*32768l+(int64_t(C[3])*dT)/256l;
		OFF = int64_t(C[2])*65536l+(int64_t(C[4])*dT)/128l;
		P = (D1_pres*SENS/(2097152l)-OFF)/(8192l);
		// Temp conversion
		TEMP = 2000l+int64_t(dT)*C[6]/8388608LL;

		//Second order compensation
		if((TEMP/100)<20){         //Low temp
			Ti = (3*int64_t(dT)*int64_t(dT))/(8589934592LL);
			OFFi = (3*(TEMP-2000)*(TEMP-2000))/2;
			SENSi = (5*(TEMP-2000)*(TEMP-2000))/8;
			if((TEMP/100)<-15){    //Very low temp
				OFFi = OFFi+7*(TEMP+1500l)*(TEMP+1500l);
				SENSi = SENSi+4*(TEMP+1500l)*(TEMP+1500l);
			}
		}
		else if((TEMP/100)>=20){    //High temp
			Ti = 2*(dT*dT)/(137438953472LL);
			OFFi = (1*(TEMP-2000)*(TEMP-2000))/16;
			SENSi = 0;
		}

		OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
		SENS2 = SENS-SENSi;
		TEMP = (TEMP-Ti);
		P = (((D1_pres*SENS2)/2097152l-OFF2)/8192l);
		temp_c = TEMP/100.0f; //unit: C

		pressure_mbar = P/10.0f;
		pressure_psi = pressure_mbar * 0.0145037738;
		pressure_pa = pressure_mbar * 100.0;
		pressure_kpa = pressure_pa / 1000.0;
		pressure_atm = pressure_mbar * 0.000986923;
	}

	uint8_t FlowIO::crc4(uint16_t n_prom[]) {
		uint16_t n_rem = 0;

		n_prom[0] = ((n_prom[0]) & 0x0FFF);
		n_prom[7] = 0;

		for ( uint8_t i = 0 ; i < 16; i++ ) {
			if ( i%2 == 1 ) {
				n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
			} else {
				n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
			}
			for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- ) {
				if ( n_rem & 0x8000 ) {
					n_rem = (n_rem << 1) ^ 0x3000;
				} else {
					n_rem = (n_rem << 1);
				}
			}
		}

		n_rem = ((n_rem >> 12) & 0x000F);
		uint8_t returnValue = n_rem ^ 0x00;
		return returnValue;
	}

	float FlowIO::getPressure(Unit pUnit){
		requestPressure();
		if(pUnit==PSI) return pressure_psi;
		else if(pUnit==ATM) return pressure_atm;
		else if(pUnit==KPA) return pressure_kpa;
		else if(pUnit==PA) return pressure_pa;
		else if(pUnit==MBAR) return pressure_mbar;
		else return pressure_psi;
	}
	float FlowIO::getPressure(){
		float returnValue = getPressure(_pressureUnit);
		return returnValue;
	}
	void FlowIO::setPressureUnit(Unit pUnit){ //unit is an enum defined in header file.
		_pressureUnit=pUnit;
	}

	//TODO: Implement a function getTemperature();
