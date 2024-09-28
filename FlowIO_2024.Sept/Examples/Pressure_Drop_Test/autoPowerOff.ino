//Note that the function below executes 'restartOffTimer()' every time there is a hardwarestate change.

int offTimerStart = millis();
uint8_t remainingTimeUint8;
bool remaining1minuteSent = false; //This needs to be reset back to false when a new activity occurs that extends the time till off.
bool remaining2minuteSent = false;
uint32_t timelastchecked=millis();

void autoPowerOff(uint8_t T) {  //the argument is in units of minutes
  if (T == 0) return;           //AutoOff is disabled. Exit the function.
  if(millis()-timelastchecked < 5000) return; //if autoPowerOff() was invoked less than 5sec ago, exit the function to save resources.

  timelastchecked = millis();
  uint32_t timeSinceLastStateChange = timelastchecked - flowio.getTimeOfLastStateChange();  //in milliseconds.
  float remainingTime = T - timeSinceLastStateChange / 60000.0f;  //value is in minutes
  Serial.println(remainingTime);
  if (remainingTime <= 0) {  //IF YOU GET THIS SIGN MIXED WITH >=, YOU WILL BRICK YOUR DEVICE!!!
      Serial.println("Powering OFF");
      int currenttime = millis();
      flowio.powerOFF();
      //You still have 54ms to execute a ton more code after invoking the powerOFF() function.
      //If we don't want anything to happen during that time, it's critical to put delay(100) right after the off() function.
      delay(55);
      Serial.println(millis()-currenttime);    
  } else if (remainingTime <= 1 && remaining1minuteSent == false) {
    Serial.println("1 minute till off");
    remaining1minuteSent = true;  //this is to ensure that we execute this only once.
  } else if (remainingTime <= 2 && remaining2minuteSent == false && remaining1minuteSent == false) {
    Serial.println("2 minutes till off");
    remaining2minuteSent = true;
  }
  //If we previously sent the notification of 1 minute or 2 minutes remaining and then an evet occurred that extened the time again,
  //we must now reset the two warning flags back to their default values of "false"
  else if(remainingTime >= 2 && remaining2minuteSent==true){
    remaining2minuteSent=false;
    remaining1minuteSent=false;
  }
  else if(remainingTime >= 1 && remaining1minuteSent==true){
    remaining1minuteSent=false;
  }
}
