#include <Servo.h>

#define PIN 10
#define MIN_PULSE 1000
#define MAX_PULSE 2000
Servo ESC;
float val = 0.0;

void setup() {
  // put your setup code here, to run once:
  ESC.attach(PIN, MIN_PULSE, MAX_PULSE);
  ESC.write(0);
  delay(2000);

  for(int x = 0; x <= 100; x += 10) {
    ESC.write(x);
    delay(500);
  }

  for(int x = 100; x > 0; x -= 10) {
    ESC.write(x);
    delay(500);
  }

  ESC.write(0);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
//  pot = analogRead(A0);
//  pot = map(pot, 
  
}
