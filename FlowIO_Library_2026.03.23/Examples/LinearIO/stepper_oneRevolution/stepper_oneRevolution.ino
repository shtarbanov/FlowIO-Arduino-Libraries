
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */
#include <FlowIO.h>
#include <Stepper.h>

const int stepsPerRevolution = 1000;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 17,18,19,26);
FlowIO flowio;

void setup() {
  flowio = FlowIO(GENERAL);
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  // initialize the serial port:
  flowio.pixel(1,1,1);
}

void loop() {
  // step one revolution  in one direction:
  myStepper.step(stepsPerRevolution);
  delay(500);

  // step one revolution in the other direction:
  myStepper.step(-stepsPerRevolution);
  delay(500);
}
