bool readContinuously = false;
uint8_t numberOfSamples = 1;
uint16_t adcValues16[16];
int lastRunTime = millis();


void createAnalogService(void) {
  uint8_t analogServiceUUID[16] = {0x07,0xaa,0x00,0x00,0x00,0x00,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-00000000aa05"
  uint8_t chrAnalogValues16UUID[16]= {0x07,0xaa,0x00,0x00,0x00,0xc2,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b,0x0b}; //"0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa05"
  
  analogService = BLEService(analogServiceUUID);
  analogService.begin();

  chrAnalogValues16 = BLECharacteristic(chrAnalogValues16UUID);
  chrAnalogValues16.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY | CHR_PROPS_WRITE);
  chrAnalogValues16.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  chrAnalogValues16.setFixedLen(32); //3 because the GUI is writing 3 bytes to the characteristic.
  chrAnalogValues16.setWriteCallback(onWrite_chrAnalogValues16);
  chrAnalogValues16.begin();
}

//#####################################--BEGIN FUNCTIONS FOR "chrAnalogValue"--###########################################
//This is executed when a central device writes to the characteristic.
void onWrite_chrAnalogValues16(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len) {
  Serial.println("Value Written");  
  if(len==1){
    if(data[0]==0x00){ //Stop continuous reading
      readContinuously = false;   //set the readContinously flag to false to activate the function in main loop.
      Serial.println("Stop reading");
    }
    else if(data[0]==0x01){ //Read 16 channels once, send result as notification.
      readContinuously = false;
      input16.read16ChannelsInto(adcValues16);
//      for(int i=0; i<16; i++){
//        adcValues16[i]=i+100;
//      }
      chrAnalogValues16.notify(&adcValues16,sizeof(adcValues16));
    }
  }
  if(len==2 && data[0]==0x02){ //Continuously read the 16 channels
    if(data[1]>0 && data[1]<=10){ //and report moving average with data[1] samples 
      readContinuously=true;
      numberOfSamples=data[1];
    }
  }
}

//This function sends an ADC value notification if the device is in an active state, and if the value is different from last time.
//I want to execute this function once every few hundred milliseconds in a nonblocking fashion.
void read16Channels_IfContinuousReadRequested_Every(uint8_t dt){
  if(Bluefruit.connected() && readContinuously==true){ //This is mandatory here becasue we cannot execute .notify8() unless connected.
    if(millis() - lastRunTime > dt){
      input16.read16ChannelsMovAvgInto(adcValues16, numberOfSamples); //recall that if numberOfSamples=1, this will not compute a moving average.
      chrAnalogValues16.notify(&adcValues16,sizeof(adcValues16));
      for(int i=0; i<16; i++){
        Serial.print(adcValues16[i]);
        Serial.print("\t");
      }
      Serial.println();
      lastRunTime = millis();
    }    
  }
}
