#include <Adafruit_VCNL4010.h>
#include <Servo.h>

#define PIN 10
#define MIN_PULSE 1000
#define MAX_PULSE 2000
Servo ESC;
float val = 0.0;
Adafruit_VCNL4010 vcnl;
unsigned long mytime;
int delaytime = 1000;
unsigned long d;
bool running;
void setup() {
  Serial.begin(9600);
  if (! vcnl.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  // put your setup code here, to run once:
  ESC.attach(PIN, MIN_PULSE, MAX_PULSE);
  ESC.write(0);
  delay(2000);
  Serial.write("startin");
  
}

void loop() {
  Serial.println(vcnl.readProximity());
  dribble();
  //delay(300);
 
}
void dribble() {
  if(vcnl.readProximity()>2800) {
      if(!running) {
        d = millis();
        start();
      } 
      else {
        d = millis();
      
      }
  }
  else {
    if(running) {
      if (millis() - d > delaytime) {
        stoprunning();
      }
    }
  }
}
void start() {
  running = true;
  ESC.write(40);
}
void stoprunning() {
  running = false;
  ESC.write(0);
}
 
//for(int x = 0; x <= 100; x += 10) {
//    ESC.write(x);
//    delay(500);
//  }
//
//  for(int x = 100; x > 0; x -= 10) {
//    ESC.write(x);
//    delay(500);
//  }
//
//  ESC.write(0);
//  delay(1000); 
  
