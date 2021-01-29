#include "Arduino.h"
#include "FlowIO.h"

//PRESSURE SENSING APIs:
	void FlowIO::setPressureUnit(Unit pUnit){ //unit is an enum defined in header file.
		_pressureUnit=pUnit;
	}
	bool FlowIO::activateSensor(){
		_i2c = &Wire; //_i2c is a pointer that points to &Wire.
		_i2c->begin();
		_i2cInitialized=true; //Once this is set to true, there is nothing to set it back to false.
			//this is the only problem with this, apparent only if someone calls the i2c .end() funtion
			//and then tries to call the .getPressure() function without calling activateSensor() beforehand.
			//In that case, the system will hang, but a user would typically never need to ever call .end().
			//But once we start having more I2C devices used, users are more likely to invoke .end(), so this
			//should be fixed.
		delay(10);
		statusByte = _getStatusByte();
		if(statusByte == 0b01000000){
			set_bit(11,_hardwareState);
			return true;
		}
		else{
			_errorCode = 222; //throw an error '222' if sensor is not found.
			return false;
		}
	}
	uint8_t FlowIO::_getStatusByte(){
		//TODO: Check if the i2c is initialilzed before making a requst. Return 0 if not set.
		_i2c->requestFrom(_addr, (uint8_t)1);
		return _i2c->read();
	}
	uint32_t FlowIO::_getRawPressure(){
		//Request a new pressure reading to be performed.
		//TODO: Check if the i2c is initialilzed before making a requst.
		_i2c->beginTransmission(_addr);
		_i2c->write(0xAA);   // command to read pressure
		_i2c->write((byte)0x0);
		_i2c->write((byte)0x0);
		_i2c->endTransmission();

		//At this point, the sensor has been asked to do a pressure reading. We can now send as many read requests as we
		//want and we will keep getting the same  pressure value. The sensor will keep spitting the same value until a
		//a new pressure reading request is made. Now, we don't want to send a request to read all 4 bytes, because the
		//sensor may still be buzy, so we want to just read the status byte first. And then after we have confirmed that
		//it is not buzy, then we will send a request to read 4 bytes. Thus, the minimum number of bytes we will actually
		//read will be 5. This takes advantage of the fact that the sensor stops sending data when it sees the stop signal,
		//and then it starts sending data from byte 1 again, when a new read request is made.
		//(An alternative approach would be to simply request 4 bytes from the start, and if the sensor is busy send another
		//request for 4 bytes. In the case when sensor is not buzy, this saves us from reading status byte twice, but in the
		//case when the sensor is buzy, we are reading the last 3 bytes when we know they are invalid. Thus this approach
		//would be more wasteful.)

		//Request to read the status byte only. Keep reading it until it is not busy.:
		while(statusByte=_getStatusByte() & MPRLS_BUSY){
			delay(2); //computation will will not take more than 5ms.
		}

		//Request to read all 4 bytes, including status byte of course.
		_i2c->requestFrom(_addr, (uint8_t)4);

		statusByte = _i2c->read();
		if (statusByte & MPRLS_SATURATION) return 0xFFFFFFFF;
		if (statusByte & MPRLS_INTEGRITY_FAIL)  return 0xFFFFFFFF;

		uint32_t rawData; //only the lower 24 bits will store our data. Upper 8 bits will be 0.
		rawData = _i2c->read();
		rawData <<= 8;
		rawData |= _i2c->read();
		rawData <<= 8;
		rawData |= _i2c->read();
		return rawData;
	}
	float FlowIO::getPressure(){
		if(_i2cInitialized==false){
			_errorCode = 223;
			return 888.8f;
		}
		if(_pressureUnit==HPA) 		return _getPressureHPA();
		if (_pressureUnit==ATM) 	return _getPressureATM();
		if (_pressureUnit==PSI)	return _getPressurePSI();
	}
	float FlowIO::getPressure(Unit pUnit){
		if(_i2cInitialized==false){
			_errorCode = 223;
			return 888.8f;
		}
		if(pUnit==PSI) return _getPressurePSI();
		if(pUnit==HPA) return _getPressureHPA();
		if(pUnit==ATM) return _getPressureATM();
	}
	float FlowIO::_getPressureHPA(){
		return _getPressurePSI() * 68.947572932;
	}
	float FlowIO::_getPressurePSI(){
		uint32_t raw_psi = _getRawPressure();
		if (raw_psi == 0xFFFFFFFF) return NAN;

		//See Derivation in the google Docs file: "MPRLS Pressure Derivation + API"
		float psi = raw_psi*(25/13421772.8)-3.125;
		return psi;
	}
	float FlowIO::_getPressureATM(){
		return _getPressurePSI() * 0.0680459639;
	}
