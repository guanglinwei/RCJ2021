#include <HoloMove.h>
#include <RoboClaw.h>

#define XCENTER 108
#define YCENTER 100

RoboClaw roboclaw(&Serial2, 10000);
HoloMove hm(&roboclaw, Serial);

int *data;
int count = 0;
int totalData = 5;
int readingInput = 0;
int hasNewData = 0;

int xJustPressed = 0;
int xIsPressed = 0;

void setup() {
  Serial1.begin(9600);
  // MyBlue.begin(9600);
  Serial.begin(4800);
  roboclaw.begin(38400);
  Serial.println("ok");
  data = new int[totalData];
  hm.stop();
}

float getAngle(int x, int y)
{
    // 0, 0 is center
    // return fmod(fmod(-atan2(x - HALF_CAM_W, y - HALF_CAM_H) * RAD_TO_DEG, 360) + 360 , 360);
    return fmod(fmod(-atan2(x, y) * RAD_TO_DEG, 360) + 360 , 360);
}

int roundm(int x, int m) {
  float r = x;
  r += m/2;
  r -= fmod(r, m);
  return (int)r;
}

void loop() {
  if(count >= totalData) {
    Serial.println("a");
    readingInput = 0;
    hasNewData = 1;
    count = 0;
  }

  if(hasNewData) {
    hasNewData = 0;
    
    // 0: lstick x, 1: lstick y
    // 2: lt, 3 rt
    // 4: x button
    int _x = roundm(data[0] - XCENTER, 25);
    int _y = roundm(YCENTER - data[1], 25);
    float ang = getAngle(_x, _y);
    float mag = sqrt(_x * _x + _y * _y);
    float speed = roundm(mag, 22);

    int rot = data[2] - data[3];

    // Serial.print("Moving at ang: "); Serial.print(ang); Serial.print(", speed: "); Serial.println(speed);

    int _minrot = 15;
    if(speed > 24) hm.move(ang, speed, (rot < _minrot) ? 0 : (rot / 2));
    else {
      if(abs(rot) < _minrot) hm.stop();
      else hm.setSpeeds(rot, rot, rot, rot);
    }

    xJustPressed = (!xIsPressed && data[4]) ? 1 : 0;
    xIsPressed = data[4];

    if(xJustPressed) {
      // kick
      Serial.println("kick");
    }
  }

  if(Serial1.available()) {
//    Serial.println("data: ");
    unsigned char _d = Serial1.peek();
    // unsigned char data = Serial1.read();
    // Serial.println(data);
    // each get of controller data starts with 255
    if(_d == 0xff) {
      // Serial.println("new- ");
      // Serial1.read();
      readingInput = 1;
      count = 0;

      // for(int i = 0; i < totalData; i++) {
      //   data[i] = Serial1.read();
      //   Serial.println(data[i]);
      // }

      // readingInput = 0;
      // hasNewData = 1;
    }
    else if(readingInput) {
      if(count >= totalData) {
        Serial.println("b");
        readingInput = 0;
        hasNewData = 1;
        count = 0;
      }
      else {
        // Serial.println(_d);
        data[count] = _d;
        count++;
      }
    }
    Serial.println(Serial1.read());
  }
}
