#include <HoloMove.h>
#include <RoboClaw.h>

#define XCENTER 108
#define YCENTER 100
#define KICKRELAY 12
#define CHARGERELAY 11
#define ULONG_MAX 0xffffffffUL

RoboClaw roboclaw(&Serial2, 10000);
HoloMove hm(&roboclaw, Serial);

int *data;
int count = 0;
int totalData = 5;
int readingInput = 0;
int hasNewData = 0;

int xJustPressed = 0;
int xIsPressed = 0;
int xCooldown = 1000;
unsigned long xCooldownFinishMillis = 0;

int yJustPressed = 0;
int yIsPressed = 0;

void setup() {
  Serial1.begin(9600);
  // MyBlue.begin(9600);
  Serial.begin(4800);
  roboclaw.begin(38400);
  Serial.println("ok");
  data = new int[totalData];
  hm.stop();

  pinMode(KICKRELAY, OUTPUT);
  pinMode(CHARGERELAY, OUTPUT);
  digitalWrite(KICKRELAY, LOW);
  digitalWrite(CHARGERELAY, HIGH);
}

float getAngle(int x, int y)
{
    return fmod(fmod(-atan2(x, y) * RAD_TO_DEG, 360) + 360 , 360);
}

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

int roundm(int x, int m) {
  float r = abs(x);
  r += 1.0 * m/2;
  r -= fmod(r, m);
  return (int)r * sgn(x);
}


// kicker
// void charge() {
//   kickOff();
//   chargeOn();
//   delay(700);
//   chargeOff();
// }
// void kick() {
//   chargeOff();
//   kickOn();
//   delay(400);
//   kickOff();
//   charge();
// }

void chargeOn() {
  digitalWrite(CHARGERELAY, LOW);
}
void chargeOff() {
  digitalWrite(CHARGERELAY, HIGH);
}
void kickOn() {
  digitalWrite(KICKRELAY, LOW);
}
void kickOff() {
  digitalWrite(KICKRELAY, HIGH);
}

unsigned long kick1FinishMillis = 0;
unsigned long kick2FinishMillis = 0;
int kicking = 0;

void kick() {
  if(kicking) return;
  Serial.println("kicking");
  kicking = 1;
  chargeOff();
  kickOn();
  kick1FinishMillis = millis() + 400;
  kick2FinishMillis = kick1FinishMillis + 700;
}

void handleTimedEvents() {
  if(kicking) {
    Serial.println("handle");
    if(millis() > kick1FinishMillis) {
      kickOff(); kickOff();
      chargeOn();
      kick1FinishMillis = ULONG_MAX;
      Serial.println("1");
    }
    if(millis() > kick2FinishMillis) {
      chargeOff();
      kick2FinishMillis = ULONG_MAX;
      kicking = 0;
      Serial.println("2");
    }
  }
}


void loop() {
  handleTimedEvents();

  if(count >= totalData) {
    readingInput = 0;
    hasNewData = 1;
    count = 0;
  }

  if(hasNewData) {
    hasNewData = 0;
    
    // 0: lstick x, 1: lstick y
    // 2: lt, 3 rt
    // 4: x button
    int _x = roundm(data[0] - XCENTER, 50);
    int _y = roundm(YCENTER - data[1], 50);
    float ang = getAngle(_x, _y);
    float mag = sqrt(_x * _x + _y * _y);
    float speed = roundm(mag, 50);

    int rot = data[2] - data[3];

    // Serial.print("Moving at ang: "); Serial.print(ang); Serial.print(", speed: "); Serial.println(speed);

    int _minrot = 15;
    if(speed > 24) hm.move(ang, speed, (abs(rot) < _minrot) ? 0 : (rot / 2));
    else {
      if(abs(rot) < _minrot) hm.stop();
      else hm.setSpeeds(rot, rot, rot, rot);
    }

    xJustPressed = (!xIsPressed && data[4]) ? 1 : 0;
    xIsPressed = data[4];

    if(xJustPressed && millis() > xCooldownFinishMillis) {
      xCooldownFinishMillis = millis() + xCooldown;
      // kick
      Serial.println("kick");
      kick();
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
    // Serial.println(Serial1.read());
    Serial1.read();
  }
}
