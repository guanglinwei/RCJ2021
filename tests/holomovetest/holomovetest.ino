#include <HoloMove.h>
#include <RoboClaw.h>
#include <math.h>
RoboClaw roboclaw(&Serial2, 10000);
HoloMove hm(&roboclaw, Serial);
float dir = 0.0;
float speed = 100.0;
float a,b,c,d;

int VRx = A0;
int VRy = A1;
int SW = 2;
int xPosition = 0;
int yPosition = 0;
int SW_state = 0;

void setup() {
  Serial.begin(9800);
  roboclaw.begin(38400);  

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  //hm.move(0, 0, 0, &a, &b, &c, &d);
  //Serial.print("moving: "); Serial.println(a);Serial.println(b);Serial.println(c);Serial.println(d);

}

void loop() {
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);

  if(xPosition > 600) { //Right
    if(yPosition > 600) { //Up
      hm.move(315.0, speed, 0, &a, &b, &c, &d);
      Serial.print(315);
    } else if (yPosition == 0) { //Down
      hm.move(225.0, speed, 0, &a, &b, &c, &d);
      Serial.print(225);
    } else { //Only right
      hm.move(270.0, speed, 0, &a, &b, &c, &d);
      Serial.print(270);
    }
  } else if(xPosition == 0) {
    if(yPosition > 600) { //Up
      hm.move(45.0, speed, 0, &a, &b, &c, &d);
      Serial.print(45);
    } else if (yPosition == 0) { //Down
      hm.move(135.0, speed, 0, &a, &b, &c, &d);
      Serial.print(135);
    } else { //Only right
      hm.move(90.0, speed, 0, &a, &b, &c, &d);
      Serial.print(90);
    }
  } else {
    hm.move(0.0, 0, 0, &a, &b, &c, &d);
  }
  //hm.move(dir, speed, 0, &a, &b, &c, &d);
  //delay(600);

  //hm.move(90.0, speed, 0, &a, &b, &c, &d);
  //delay(600);
// 
//  hm.move(180.0, speed, 0, &a, &b, &c, &d);
//  delay(600);
//
//  hm.move(270.0, speed, 0, &a, &b, &c, &d);
//  delay(600);
 
  
}
