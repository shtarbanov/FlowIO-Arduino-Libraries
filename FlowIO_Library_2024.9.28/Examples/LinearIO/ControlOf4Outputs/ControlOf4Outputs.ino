#include <bluefruit.h>

#define in1 17
#define in2 18
#define in4 19
#define in3 26
//#define en 25 //not needed if resistor is soldered.

void setup() {
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  digitalWrite(LED_BLUE,HIGH);
}

void loop() {
  digitalWrite(in1,HIGH); //#1 from left side
  digitalWrite(in2,LOW); //#2 from left side
  digitalWrite(in3,LOW); //#3 from left side
  digitalWrite(in4,LOW); //#4 from left side
  delay(1000);
} 

//Activating 3 and 4 causes them to go LOW. Doesn't affect the others
//Activating 1 and 2 causes them to go LOW. Doesn't affect the others
//Activating 2 and 3 works
//Activating 1 and 3 works
//Activating 1 and 4 works
//Activating 2 and 4 
