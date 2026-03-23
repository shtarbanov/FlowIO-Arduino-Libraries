uint8_t _pumpPins[2]={15,16};
uint8_t _portValvePins[5]={12,11,10,9,6}; //left pin is port 1.
uint8_t _inletValvePin=5; //(right) valve
uint8_t _outletValvePin=14; //(left) valve

void setup() {
    pinMode(_inletValvePin, OUTPUT);
    analogWrite(_inletValvePin,0);
    pinMode(_outletValvePin, OUTPUT);
    analogWrite(_outletValvePin,0);
    for(int i=0; i < 2; i++){
      pinMode(_pumpPins[i], OUTPUT);
      analogWrite(_pumpPins[i],0);
    }
    for(int i=0; i < 5; i++){
      pinMode(_portValvePins[i], OUTPUT);
      analogWrite(_portValvePins[i],LOW);
    }
}

void loop() {
  //openInletValve();
  //delay(1000);
  holdInletValve(160); //even at 200, the current saved is substantial!
  delay(1000);
  closeInletValve();
  delay(1000);
}

void openInletValve(){
  analogWrite(_inletValvePin,255);
}
void holdInletValve(uint8_t holdPWM){
  analogWrite(_inletValvePin,holdPWM);
}
void closeInletValve(){
  analogWrite(_inletValvePin,0);
} 
