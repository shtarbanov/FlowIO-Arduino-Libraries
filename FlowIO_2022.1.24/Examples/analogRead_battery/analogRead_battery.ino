#include <Adafruit_TinyUSB.h> // for Serial

#define VBAT_PIN          A6            //A7 for nrf52832, A6 for nrf52840

float getVBAT_mV() {
  //Vbat(ADC) = ADC*(3000mV/4096steps)*(R1+R2)/R2. 
  //For nrf52830: R1=806k, R2=2000k;  (R1+R2)/R2 = 1.403
  //For nrf52840: R1=100k, R2=100k;   (R1+R2)/R2 = 2
  float vBat_mV = 2*analogRead(VBAT_PIN)*3000 / 4096.0;
  return vBat_mV;
}
uint8_t getBatteryPercentage(){ //this computation is based on LiPo chemistry.
  float mv = getVBAT_mV();
  if(mv<3300) return 0;
  if(mv<3600) return (mv-3300)/30;
  mv = mv-3600;
  return 10 + (mv * 0.15F );  // thats mv/6.66666666
}
void setup() {
  analogReference(AR_INTERNAL_3_0);
  analogReadResolution(12);   // Set the resolution to 12-bit (0..4095). Can be 8, 10, 12 or 14
  pinMode(VBAT_PIN,INPUT);

}

void loop() {
  Serial.begin(115200);
  while(!Serial) delay(10);
  Serial.println("Started");
//  Serial.println(analogRead(VBAT_PIN));
//  Serial.println(readVBAT());
  Serial.println(getBatteryPercentage());
  delay(100);
}
