#include <HoloMove.h>
#include "HyperDisplay_KWH018ST01_4WSPI.h"
#include <RoboClaw.h>
#include <math.h>

#define PWM_PIN 13             // Pin definitions
#define CS_PIN 45
#define DC_PIN 44
#define SPI_PORT SPI
#define SPI_SPEED 32000000 
KWH018ST01_4WSPI myTFT;  
ILI9163C_color_18_t defaultColor;

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


bool runn = true;
void setup() {
  Serial.begin(9600);
  roboclaw.begin(38400);  

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP); 
  //hm.move(0, 0, 0, &a, &b, &c, &d);
  //Serial.print("moving: "); Serial.println(a);Serial.println(b);Serial.println(c);Serial.println(d);

  myTFT.begin(DC_PIN, CS_PIN, PWM_PIN, SPI_PORT, SPI_SPEED);
  myTFT.clearDisplay();
  myTFT.setTextCursor(0,0);            
  myTFT.setCurrentWindowColorSequence((color_t)&defaultColor);
  defaultColor = myTFT.hsvTo18b( 100, 255, 255 );
  
}

void loop() {
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  int state = 0;
  int lastState = 0;
  while(runn) {
    xPosition = analogRead(VRx);
    yPosition = analogRead(VRy);
    SW_state = digitalRead(SW);
    if(xPosition > 600) { //Right
      if(yPosition > 600) { //Up
        hm.move(315.0, speed, 0, &a, &b, &c, &d);
        state = 315;
      } else if (yPosition == 0) { //Down
        hm.move(225.0, speed, 0, &a, &b, &c, &d);
        state = 225;
      } else { //Only right
        hm.move(270.0, speed, 0, &a, &b, &c, &d);
        state = 270;
      }
    } else if(xPosition == 0) {
      if(yPosition > 600) { //Up
        hm.move(45.0, speed, 0, &a, &b, &c, &d);
        state = 45;
      } else if (yPosition == 0) { //Down
        hm.move(135.0, speed, 0, &a, &b, &c, &d);
        state = 135;
      } else { //Only right
        hm.move(90.0, speed, 0, &a, &b, &c, &d);
        state = 90;
      }
    } else if(yPosition>600){
      hm.move(0.0, speed, 0, &a, &b, &c, &d);
      state = 1;
    } else if(yPosition==0){
      hm.move(180.0, speed, 0, &a, &b, &c, &d);
      state = 180;
    } else {
      hm.move(0.0, 0, 0, &a, &b, &c, &d);
      state = 0;
    }
    if (state != lastState) {
      myTFT.clearDisplay();
      myTFT.setTextCursor(0,0);
      if (state == 1) {
        myTFT.print("0");
      } else if (state == 0) {
        myTFT.print("Stopped");
      } else {
        myTFT.print(state);
      }
    }
    lastState = state;
    
    if(SW_state == LOW) {
      runn = false;
    }
    //Serial.println(runn);
  }
  if(yPosition == 0 && xPosition == 0) {
    runn = true;
  }
  //Serial.println(runn);
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
