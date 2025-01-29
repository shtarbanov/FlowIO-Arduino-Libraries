void pumpToggle(uint16_t T) {  //the argument is in milliseconds
  static uint32_t timelastchecked=millis(); //executes only once in the program's lifetime
  if (T == 0) return;
  else if(millis()-timelastchecked < T)  return;
  else if(millis()-timelastchecked >= T){
    Serial.print("\nToggle ");
    timelastchecked = millis(); //update the variable so we know how much time has passed between now and the next invocation.
    //toggle pumps
    flowio.getHardwareStateOf(PUMP1) ? flowio.stopPump(1) : flowio.startPump(1,255);
    // flowio.getHardwareStateOf(PUMP2) ? flowio.stopPump(2) : flowio.startPump(2,255);
    flowio.getHardwareStateOf(LEDBLUE) ? flowio.blueLED(LOW) : flowio.blueLED(HIGH);
    Serial.println(flowio.getHardwareState(),BIN);
    //I want to have one pump on while the other is off, and vice versa. For this to happen,
    //no change is needed in the function, but I just need to change the initial condition so that the
    //pumps are out of phase upon exiting the setup loop.
  }
}