#include <HoloMove.h>
#include <RoboClaw.h>
#include <math.h>
RoboClaw roboclaw(&Serial, 10000);
HoloMove hm(&roboclaw);
float dir = 0.0;
float speed = 255.0;
float a,b,c,d;



void setup() {
  Serial.begin(9600);
  roboclaw.begin(38400);  

//  hm.move(0, speed, 0, &a, &b, &c, &d);
//  Serial.print("moving: "); Serial.println(a);Serial.println(b);Serial.println(c);Serial.println(d);

}

void loop() {
  hm.move(dir, speed, 0, &a, &b, &c, &d);
  dir = fmod(dir + 0.5, 360);
}
