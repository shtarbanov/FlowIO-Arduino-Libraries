bool readContinuously = false;
uint8_t channelNumber = 1;
uint8_t numberOfSamples = 1; //for computing the moving average
int lastRunTime = millis();

void createAnalogService(void) {
  uint8_t analogServiceUUID[16]     = {0x07,0xaa,0x00,0x00,0x00,0x00,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-00000000aa05"
  uint8_t chrAnalogValueUUID[16]=     {0x07,0xaa,0x00,0x00,0x00,0xc1,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa05"
  
  analogService = BLEService(analogServiceUUID);
  analogService.begin();

  chrAnalogValue = BLECharacteristic(chrAnalogValueUUID);
  chrAnalogValue.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY | CHR_PROPS_WRITE);
  chrAnalogValue.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  chrAnalogValue.setFixedLen(3); //3 because the GUI is writing 3 bytes to the characteristic.
  chrAnalogValue.setWriteCallback(onWrite_chrAnalogValue);
  chrAnalogValue.begin();
}

//This is executed when a central device writes to the characteristic.
void onWrite_chrAnalogValue(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value Written");  
  if(len==1 && data[0]==0x00){ //Stop continuous reading
    readContinuously = false;   //set the readContinously flag to false to activate the function in main loop.
    Serial.println("Stop reading");
  }
  if(len==2 && data[0]==0x01){ //Read a single value
    readContinuously = false; //stop continuous reading if happening.
    if(data[1]>0 && data[1]<17){
      uint16_t adcValue = input16.readChannel(data[1]);
      chrAnalogValue.notify(&adcValue,sizeof(adcValue));
      Serial.print("Read 1 sample from channel ");
      Serial.println(data[1]);
    }
  }  
  else if(len==3 && data[0]==0x02){ //Read Continuously
    if(data[1]>0 && data[1]<17 && data[2]>0 && data[2]<=10){
      readContinuously = true;
      channelNumber = data[1];
      numberOfSamples = data[2];
      Serial.print("Read continuously channel ");
      Serial.print(channelNumber);
      Serial.print(", moving avg. over ");
      Serial.print(numberOfSamples);
      Serial.println(" samples.");
    }
  }
}

//This function sends an ADC value notification if the device is in an active state, and if the value is different from last time.
void readSingleChannel_IfContinuousReadRequested_Every(uint8_t dt){
  if(Bluefruit.connected() && readContinuously==true){ //This is mandatory here becasue we cannot execute .notify8() unless connected.
    if(millis() - lastRunTime > dt){   
      uint16_t adcValue = input16.readChannelMovAvg(channelNumber, numberOfSamples); //recall that if numberOfSamples=1, this will not compute a moving average.
      chrAnalogValue.notify(&adcValue,sizeof(adcValue));
      lastRunTime = millis();
    }  
  }
}
