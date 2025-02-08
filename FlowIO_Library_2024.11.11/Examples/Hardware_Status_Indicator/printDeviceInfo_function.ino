void printDeviceInfo(){
  Serial.print("\n-----------INFO---------------\nMinutes till power off: ");
  Serial.println(autoPowerOff(AUTOOFFTIMER));
  Serial.print("FlowIO System ID: ");
  Serial.println(getMcuUniqueID());
  Serial.print("Hardware state: ");
  Serial.println(flowio.getHardwareState(),BIN);
  Serial.print("------------------------------\n");
  flowio.blueLED(HIGH);
  // Serial.print("Sensor Present: ");
}