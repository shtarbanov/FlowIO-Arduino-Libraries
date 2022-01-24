#define in1 26
#define in2 19
#define in3 18
#define in4 17
#define en 25 //not needed if resistor is soldered.

void setup() {
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  digitalWrite(LED_BLUE,HIGH);
}

void loop() {
  digitalWrite(in1,LOW); //out #4 from left
  digitalWrite(in2,LOW); //out #3 from left
  digitalWrite(in3,LOW); //out #2 from left
  digitalWrite(in4,HIGH);
  delay(1000);
}
