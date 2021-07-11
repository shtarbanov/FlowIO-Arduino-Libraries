#include <bluefruit.h>
#include <Module16Inputs.h>

#define DEVICE_NAME "FlowIO_Analog"// Device Name: Maximum 30 bytes

Module16Inputs input16;

//#################---Define Service and Characteristic Names---####################
BLEService analogService;
  BLECharacteristic chrAnalogValues16;
//##################################################################################

void setup() {
  input16 = Module16Inputs(); //This must be done the very first item to minimize the click on startup (Bug #44 on Github)  
  //NOTE: You cannot do the object initialization together with the declaration, because it will cause the 
  //hardware to not work fine, even though it will compile fine.
  Serial.begin(115200);
  //All Bluefruit.config***() function must be called before Bluefruit.begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX); // Config the peripheral connection with maximum bandwidth. Do I need this?
  Bluefruit.configUuid128Count(15); //by default this is 10, and we have more than 10 services & characteristics on FlowIO
  Bluefruit.begin();
  Bluefruit.setTxPower(8);
  // Supported tx_power values depending on mcu:
    // - nRF52832: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +3dBm and +4dBm.
    // - nRF52840: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +2dBm, +3dBm, +4dBm, +5dBm, +6dBm, +7dBm and +8dBm.
  Bluefruit.setName(DEVICE_NAME);
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  //#####################---Create Services---####################
  createAnalogService();
  //##############################################################
  startAdvertising();
}

void startAdvertising(void) {
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.ScanResponse.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.addService(analogService); //advertise service uuic

  Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode

  Bluefruit.Advertising.start(0);
}

void loop() {
  read16Channels_IfContinuousReadRequested_Every(50);
}

void connect_callback(uint16_t conn_handle){
  //Do something on connect
}
void disconnect_callback(uint16_t conn_handle, uint8_t reason){
  //stop everything on disconnect
}
