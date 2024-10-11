float autoPowerOff(uint8_t T) {  //the argument is in units of minutes
  static float remainingTime = T;
  //The following static variable initilizations will execute only firs first time this function is invoked, and will be ignored afterwareds.
  static uint32_t timelastchecked=millis();
  static uint8_t alertMinutes = T; //this will decrease by 1 whenever remainingTime goes down by a full minute.
  if (T == 0) return 0;           //AutoOff is disabled. Exit the function.
  if(millis()-timelastchecked < 5000) return remainingTime; //if autoPowerOff() was invoked less than 5sec ago, exit the function to save resources.
  else timelastchecked = millis(); //update the variable so we know how much time has passed between now and the next invocation.

  //Anything below this line will be executed only once every 5 seconsds.  
  uint32_t timeSinceLastStateChange = millis() - flowio.getTimeOfLastStateChange();  //in milliseconds.
  remainingTime = T - timeSinceLastStateChange / 60000.0f;  //value is in minutes
  
  if(remainingTime <= 0){     //IF YOU GET THIS SIGN MIXED WITH >=, YOU WILL BRICK YOUR DEVICE!!!
    flowio.powerOFF();
    Serial.println(F("Powering OFF"));
    delay(100); //this delay ensures nothing can be executed in the 54 milliseconds that FlowIO needs to shut down.
    flowio.setTimeOfLastStateChange(); //Record a state change to then restart off timer in case FlowIO fails to power off.
  }
  //alertMinutes starts at T and decreases by 1 every minute; this condition is true only once-per-minute.
  else if(remainingTime <= alertMinutes){
    Serial.print(alertMinutes);
    Serial.println(F(" min till Off."));
    alertMinutes--;
  }
  //Reset the alerter if timer has been reset. If remaning time is more than a minute ahead of alertMinutes, 
  //then we know that the remaining time must have been reset, as that can't happen otherwise.
  else if(remainingTime-alertMinutes > 1) alertMinutes=T;

  return remainingTime;
}



// void autoPowerOff(uint8_t T) {  //the argument is in units of minutes
//   //The following static variable initilizations will execute only firs first time this function is invoked, and will be ignored afterwareds.
//   static uint32_t timelastchecked=millis();
//   static uint8_t alertMinutes = T; //this will decrease by 1 whenever remainingTime goes down by a full minute.
//   if (T == 0) return;           //AutoOff is disabled. Exit the function.
//   if(millis()-timelastchecked < 5000) return; //if autoPowerOff() was invoked less than 5sec ago, exit the function to save resources.
//   else timelastchecked = millis(); //update the variable so we know how much time has passed between now and the next invocation.

//   //Anything below this line will be executed only once every 5 seconsds.  
//   uint32_t timeSinceLastStateChange = millis() - flowio.getTimeOfLastStateChange();  //in milliseconds.
//   float remainingTime = T - timeSinceLastStateChange / 60000.0f;  //value is in minutes
  
//   if(remainingTime <= 0){     //IF YOU GET THIS SIGN MIXED WITH >=, YOU WILL BRICK YOUR DEVICE!!!
//     flowio.powerOFF();
//     Serial.println(F("Powering OFF"));
//     delay(100); //this delay ensures nothing can be executed in the 54 milliseconds that FlowIO needs to shut down.
//     flowio.setTimeOfLastStateChange(); //Record a state change to then restart off timer in case FlowIO fails to power off.
//   }
//   //alertMinutes starts at T and decreases by 1 every minute; this condition is true only once-per-minute.
//   else if(remainingTime <= alertMinutes){
//     Serial.print(alertMinutes);
//     Serial.println(F(" min till Off."));
//     alertMinutes--;
//   }
//   //Reset the alerter if timer has been reset. If remaning time is more than a minute ahead of alertMinutes, 
//   //then we know that the remaining time must have been reset, as that can't happen otherwise.
//   else if(remainingTime-alertMinutes > 1) alertMinutes=T;
// }