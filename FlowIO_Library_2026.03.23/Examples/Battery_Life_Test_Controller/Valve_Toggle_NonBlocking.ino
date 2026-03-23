void valveToggle(uint16_t T) {  //the argument is in milliseconds
  static uint32_t timelastchecked=millis(); //executes only once in the program's lifetime
  if (T == 0) return;
  else if(millis()-timelastchecked < T)  return;
  else if(millis()-timelastchecked >= T){
    Serial.print("\nToggle ");
    timelastchecked = millis(); //update the variable so we know how much time has passed between now and the next invocation.
    //toggleValves
    flowio.getHardwareStateOf(INLET) ? flowio.closeInletValve() : flowio.openInletValve();
    flowio.getHardwareStateOf(PORT1) ? flowio.closePorts(0b00000011) : flowio.openPorts(0b00000011);
    flowio.getHardwareStateOf(LEDBLUE) ? flowio.blueLED(LOW) : flowio.blueLED(HIGH);
    // Serial.println(flowio.getHardwareStateOf(LEDBLUE));
    Serial.println(flowio.getHardwareState(),BIN);
  }
}