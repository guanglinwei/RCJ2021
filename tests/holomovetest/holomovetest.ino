#include <HoloMove.h>
#include <RoboClaw.h>
#include <math.h>
RoboClaw roboclaw(&Serial, 10000);
HoloMove hm(&roboclaw);
int dir = 0;
float speed = 255.0;
float a,b,c,d;



void setup() {
  Serial.begin(9600);
  roboclaw.begin(38400);  

//  hm.move(0, speed, 0, &a, &b, &c, &d);
//  Serial.print("moving: "); Serial.println(a);Serial.println(b);Serial.println(c);Serial.println(d);

}

void loop() {
  hm.move(dir/10.0, speed, 0, &a, &b, &c, &d);
  dir = (dir + 5) % 3600;
}
