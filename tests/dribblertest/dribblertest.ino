#include <Servo.h>

#define PIN 10
#define MIN_PULSE 1000
#define MAX_PULSE 2000
Servo ESC;
float val = 0.0;

void setup() {
  // put your setup code here, to run once:
  ESC.attach(PIN, MIN_PULSE, MAX_PULSE);
}

void loop() {
  // put your main code here, to run repeatedly:
//  pot = analogRead(A0);
//  pot = map(pot, 
  ESC.write(val);
  val += 0.5;
}
