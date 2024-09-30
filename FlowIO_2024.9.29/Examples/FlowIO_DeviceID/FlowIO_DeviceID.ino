/*  This code is written by: Ali Shtarbanov 
 *   
 *  If you enjoy FlowIO and the other resources I share freely on softrobotics.io, you can
 *  buy me a coffee and send me an encouraging comment at https://www.buymeacoffee.com/shtarbanov
 *  This just shows me you appreciate the time and resources I dedicate to this effort, and it 
 *  gives me the mental encouragement and motivation to keep this project alive and growing. 
 *  Remembering those who support me and their words of appreciation, is what gives me the 
 *  strength to keep going, especially in difficult moments. Thank you for your support!
 *  
 */
#include <bluefruit.h>
#define DEVICE_NAME "FlowIO_Info"// Device Name: Maximum 30 bytes
BLEDis devInfo; //this creats both the service and characteristic(s) automatically.

void setup(){
  Serial.begin(115200);
  analogReference(AR_INTERNAL_3_0);   // Set the analog reference to 3.0V (default = 3.6V) b/c battery voltage may drop to <3.6V and then default will become inaccurate.
  analogReadResolution(12);   // Set the resolution to 12-bit (0..4095). Can be 8, 10, 12 or 14
  Bluefruit.autoConnLed(true);   // Setup the BLE LED to be enabled on CONNECT
  //All Bluefruit.config***() function must be called before Bluefruit.begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Config the peripheral connection with maximum bandwidth
  Bluefruit.configUuid128Count(15); //by default this is 10, and we have more than 10 services & characteristics on FlowIO
  Bluefruit.begin();
  Bluefruit.setTxPower(8);
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  devInfo.setSystemID(getMcuUniqueID());

  //We must explicitly set to NULL the characteristics that we don't want to create:
  devInfo.setHardwareRev(NULL); 
  devInfo.setManufacturer(NULL); 
  devInfo.setSoftwareRev(NULL);
  devInfo.setModel(NULL);
  devInfo.setSerialNum(NULL); //by setting it to null, this defaults to the MCU serial number.
  //We can't avoid creating the SerialNum characteristic unless we modify the library.
  devInfo.begin();
  
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);   // Advertising packet
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.restartOnDisconnect(true); //Restart advertising on disconnect.
  Bluefruit.ScanResponse.addName();   // Secondary Scan Response packet (optional)
  Bluefruit.Advertising.addService(devInfo);   //advertise service uuid
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode

  Bluefruit.Advertising.start(0);  
}
void loop(){
}
void connect_callback(uint16_t conn_handle){ // callback invoked when central connects
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
}
