#include <Wire.h>
#include "MS5837.h"
MS5837 sensor;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  Wire.begin();

  // Initialize pressure sensor .init() returns true if successfull.
  while (!sensor.initSensor()) {
    Serial.println("Init failed!");
    delay(5000);
  }  
}

void loop() {
  sensor.requestPressureAndTemp(); //Read the pressure and temperature and stores them internally. 

  Serial.print("P = ");
  Serial.print(sensor.pressure_mbar);
  Serial.print(" mbar\t");
  Serial.print(sensor.pressure_psi);
  Serial.print(" psi\t");
  Serial.print(sensor.getPressure(ATM));
  Serial.println(" atm");

  Serial.print("T = ");
  Serial.print(sensor.temp_c);
  Serial.println(" deg C");

  delay(1000);
}
