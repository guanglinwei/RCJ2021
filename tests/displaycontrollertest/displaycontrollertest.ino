#include <HyperDisplay_KWH018ST01_4WSPI.h>
#include <DisplayController.h>
#include <HoloMove.h>
#include <RoboClaw.h>
#include <Adafruit_VEML7700.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Pixy2.h>
#include <Wire.h>
#include <StateMachine.h>
#include <ArduinoUnit.h>
// #include <AsyncTimer.h>
#include <Async.h>
#include <Adafruit_VCNL4010.h>
#include <Servo.h>

#pragma region DEFINES

#define TFT_PWM_PIN 13         // Pin definitions
#define TFT_CS_PIN 45
#define TFT_DC_PIN 44
#define SPI_PORT SPI
#define SPI_SPEED 32000000 

#define CAM_W 316
#define CAM_H 208
#define HALF_CAM_W 158
#define HALF_CAM_H 104
#define RAD_TO_DEG 57.295779513082320876798154814105
#define PI 3.1415926535

#define JOY_SW_PIN 2

#define KICKRELAY 12
#define CHARGERELAY 11

#define PROXIMITY_PIN 10
#define PROXIMITY_MIN_PULSE 1000
#define PROXIMITY_MAX_PULSE 2000

#pragma endregion

#pragma region GLOBAL VARS

// AsyncTimer _t;
// Async _a(Serial);

KWH018ST01_4WSPI myTFT;
DisplayController myDisplay(myTFT, Serial);
RoboClaw roboclaw(&Serial2, 10000);
HoloMove hm(&roboclaw, Serial);
Pixy2 pixy;

Servo ESC;
int isDribbling = 0;
Adafruit_VCNL4010 proximity;


// State
StateMachine stateMachine(8);
enum State {
    None = -1,
    Default = 0,
    Chase,
    Score
};

// Robot data, used after stateLoop
float MOVESPEED = 0;
float MOVEDIR = 0;
float MOVEROT = 0;
// enum RobotDataType {
//     // static values, cannot be changed
//     None = -1, 
//     STATICS_SEPARATOR, // anything below this will be update
//     BallX,
//     BallY, 
//     LastBallX, 
//     LastBallY
// };
// float RobotData[];

// camera viewer
int ballX, ballY, lastBallX, lastBallY = 0;
// int bGoalX, bGoalY, yGoalX, yGoalY = 0;
int allyGoalX, allyGoalY, enemyGoalX, enemyGoalY = 0;
int isAllyGoalYellow = 0;
float ang = 0.0; // angle towards ball. up is 0, increases CCW
int ballSeen = 0;
int speed = 120;

// light sensors
Adafruit_VEML7700 vemls[4];
int vemlData[4];
enum VEMLDirection: uint8_t {
    Up = 0,
    Right = 1,
    Down,
    Left
};

// compass
Adafruit_LSM303_Accel_Unified compass = Adafruit_LSM303_Accel_Unified(54321);
float forwardHeading = 0;
float currentHeading = 0;

int robotCanMove = 0;

// kicker
int kicking = 0;

#pragma endregion

#pragma region UTILITIES

char _stringToPrint[50];
template<typename... Args> void printFormat(const char* format, Args... args)
{
  sprintf(_stringToPrint, format, args...);
  Serial.println(_stringToPrint);
}

int isAngleWithinInterval(float check, float center, float allowance) {
    float d = fmod(check - center + 180 + 360, 360) - 180;
    return abs(d) < allowance;
}

// Returns better direction to turn to face a certain angle 
// 1 if ccw, -1 if cw, 0 if both angles are equal
int turnDirectionToAngle(float current, float expected) {
    if(current == expected) return 0;
    float diff = abs(current - expected);
    if(diff < 180) {
        return current < expected ? 1 : -1;
    }
    return current > expected ? 1 : -1;
}

// + / -, 1 / -1
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

float sqrDist(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

// change var to be closer to target by d
void transitionVar(int *var, int target, int d) {
    d = abs(d);
    if(*var > target) *var = max(*var - d, target);
    else *var = min(*var + d, target);
}

float getAngle(int x, int y)
{
    // 0, 0 is center
    // return fmod(fmod(-atan2(x - HALF_CAM_W, y - HALF_CAM_H) * RAD_TO_DEG, 360) + 360 , 360);
    return fmod(fmod(-atan2(x, y) * RAD_TO_DEG, 360) + 360 , 360);
}

int _t = 0;
void testPrint() {
    Serial.println(_t++);
}

#pragma endregion

// char *dataLabels[4];
int i = 0;
char istr[4];

void switchAllyGoal() {
    isAllyGoalYellow = !isAllyGoalYellow;
    stateMachine.SetState(State::Default);
}

void allowRobotMove() {
    Serial.println("chase");
    robotCanMove = 1;
    stateMachine.SetState(State::Chase);
}

// approach an angle from behind it
float calculateTangentialAngle(float angle, float diff) {
    if(angle < 180) return angle + diff;
    return angle - diff;
}

void getDataFromCamera()
{
    // lastBallX = ballX < 0 ? lastBallX : ballX;
    // lastBallY = ballY < 0 ? lastBallY : ballY;
    lastBallX = ballX;
    lastBallY = ballY;

    pixy.ccc.getBlocks();

    if(!pixy.ccc.numBlocks) {
        ballSeen = 0;
        Serial.println("none\n");
        return;
    }

    for(int i = 0; i < pixy.ccc.numBlocks; i++) {
        switch(pixy.ccc.blocks[i].m_signature) {
            // ball
            case 1:
                ballSeen = 1;
                ballX = pixy.ccc.blocks[i].m_x - HALF_CAM_W;
                ballY = pixy.ccc.blocks[i].m_y - HALF_CAM_H;

                // TEST??
                ballY *= -1;

                // if the blob is not on the mirror, ignore it
                if(abs(ballX) - 12 > HALF_CAM_H || ballX * ballX + ballY * ballY - 12 > HALF_CAM_H * HALF_CAM_H) {
                    ballSeen = 0;
                    Serial.println("not on mirror\n");
                    return;
                }
                // Serial.print(ballX); Serial.print(" | "); Serial.println(ballY);

                ang = getAngle(ballX, ballY);
                ang += 180;
                if(ang > 360) {
                    ang -= 360;
                }
                return;
        
            // yellow goal
            case 2:
                if(isAllyGoalYellow) {
                    allyGoalX = pixy.ccc.blocks[i].m_x;
                    allyGoalY = pixy.ccc.blocks[i].m_y;
                }
                else {
                    enemyGoalX = pixy.ccc.blocks[i].m_x;
                    enemyGoalY = pixy.ccc.blocks[i].m_y;
                }
                break;

            // blue goal
            case 3:
                if(isAllyGoalYellow) {
                    enemyGoalX = pixy.ccc.blocks[i].m_x;
                    enemyGoalY = pixy.ccc.blocks[i].m_y;
                }
                else {
                    allyGoalX = pixy.ccc.blocks[i].m_x;
                    allyGoalY = pixy.ccc.blocks[i].m_y;
                }
                break;

        }

    }
}

// int mapCameraCoordinateToScreen(int from) {
//     return map(from, 0, )
// }

void cameraViewerLoop() {
    Serial.println("camera viewer loop");
    getDataFromCamera();

    int r = 50;

    // erase the last position of the ball
    // int displayLastBallX = map(lastBallY, 0, CAM_H, 64-54, 64+54);
    // int displayLastBallY = map(lastBallX, HALF_CAM_W - HALF_CAM_H, HALF_CAM_W + HALF_CAM_H, 54-54, 54+54);
    
    // test 
    int displayLastBallX = map(lastBallY, -HALF_CAM_H, HALF_CAM_H, 64-r, 64+r);
    int displayLastBallY = map(lastBallX, -HALF_CAM_H, HALF_CAM_H, 54-r, 54+r);
    myDisplay.myTFT.pixel(displayLastBallX, displayLastBallY, (color_t)&myDisplay.defaultColor);

    // draw the ball as an orange pixel if it is seen
    // otherwise draw a light gray pixel at where it last was
    // int displayBallX = map(ballY, 0, CAM_H, 64-54, 64+54);
    // int displayBallY = map(ballX, HALF_CAM_W - HALF_CAM_H, HALF_CAM_W + HALF_CAM_H, 54-r, 54+r);

    // test
    int displayBallX = map(ballY, -HALF_CAM_H, HALF_CAM_H, 64-r, 64+r);
    int displayBallY = map(ballX, -HALF_CAM_H, HALF_CAM_H, 54-r, 54+r);

    if(ballSeen) {
        myDisplay.myTFT.pixel(displayBallX, displayBallY, (color_t)&myDisplay.ORANGE);
    }
    else {
        myDisplay.myTFT.pixel(displayBallX, displayBallY, (color_t)&myDisplay.LIGHT_GRAY);
    }
}

void switchToCameraViewer() {
    allowRobotMove();
    myDisplay.clearDisplay();
    Serial.println("switch to camera view");
    myDisplay.switchToCustomWindow(&cameraViewerLoop);
    myDisplay.myTFT.circle(64, 54, 54, false, (color_t)&myDisplay.DARK_GRAY);
}

// Color sensors
int tcaselect(uint8_t i) {
    // Serial.println(i);
    if(i > 7) return 0;
    Wire.beginTransmission(0x70);
    Wire.write(1 << i);
    Wire.endTransmission(); 
}

// read from VEMLs, called once 
int getWhiteLines() {
    int MAX_LUX = 15000; // placeholder, lux value for white line
    int ret = 0;
    for(int i = 0; i < 4; i++) {
        vemlData[i] = 0;
        if(vemls[i].readLux() > MAX_LUX) {
            ret += 1;
            vemlData[i] = 1;
        }
    }

    return ret;
}

float readCompassHeading() {
    sensors_event_t event;
    compass.getEvent(&event);

    // printFormat("%i, %i, %i", event.magnetic.x, event.magnetic.y, event.magnetic.z);

    float heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;

    if(heading < 0) heading += 360;
    return heading;
}

// 1: ccw. -1: cw
void rotate(float speed, int direction) {
    hm.setSpeeds(speed * direction, speed * direction, speed * direction, speed * direction);
}

int rotateToPoint(float speed, int x, int y, float allowance) {
    float _a = getAngle(x, y);
    if(isAngleWithinInterval(0, _a, allowance)) {
        return 0;
    }

    rotate(speed, turnDirectionToAngle(0, _a));
    return 1;
}
// move in a direction and turn to face a certain direction
void moveAndTurnToHeading(float direction, float speed, float heading, float allowance = 10, float rotation = 60) {
    // int allowance = 10;
    // int rotation = 60;
    // if close enough to correct direction
    if(isAngleWithinInterval(currentHeading, heading, allowance)) {
        // hm.move(direction, speed);
        MOVEDIR = direction;
        MOVESPEED = speed;
    }
    else {
        // hm.move(direction, speed, turnDirectionToAngle(currentHeading, heading) * rotation);
        MOVEDIR = direction;
        MOVESPEED = speed;
        

        // TODO: FIX THIS
        MOVEROT = turnDirectionToAngle(currentHeading, heading) * rotation;
    }
}

void moveAndTurnToPoint(float direction, float speed, int x, int y, float allowance = 10, float rotation = 10) {
    float _a = getAngle(x, y);
    if(isAngleWithinInterval(0, _a, allowance)) {
        // hm.move(direction, speed);
        MOVEDIR = direction;
        MOVESPEED = speed;
    }
    else {
        // hm.move(direction, speed, turnDirectionToAngle(currentHeading, heading) * rotation);
        MOVEDIR = direction;
        MOVESPEED = speed;
        MOVEROT = turnDirectionToAngle(0, _a) * rotation;
    }
}

void moveToAndFace(float speed, int x, int y, float allowance = 10, float rotation = 60) {
    moveAndTurnToPoint(getAngle(x, y), speed, x, y, allowance, rotation);
}

void kick() {
    // TODO:
    // _t.setTimeout([&]() {
    //     //
    // }, 1234);
}

void dribbleOn() {
    isDribbling = 1;
    ESC.write(40);
}

void dribbleOff() {
    isDribbling = 0;
    ESC.write(0);
}

void chaseBallLoop() {
    getDataFromCamera();
    float ballAngle = getAngle(ballX, ballY);
    Serial.println(ballAngle);
    Serial.println(ballX);
    Serial.println(ballY);
    Serial.println("");
    // if close to ball
    if(sqrDist(0, 0, ballX, ballY) < 100) {
        // if already facing the ball, dribble or change state
        // if(isAngleWithinInterval(0, ballAngle, 20) && proximity.readProximity() < 2000) {
        if(isAngleWithinInterval(0, ballAngle, 20)) {
            // stateMachine.SetState(State::Score);
            return;
        }
        // else turn towards it
        rotateToPoint(speed, ballX, ballY, 15);
    }
    else {
        moveAndTurnToPoint(ballAngle, speed, ballX, ballY);
    }
}

// TODO: turn on dribbler on enter, off on exit
void moveBallToEnemyGoal() {
    // if does not have ball, change state
    if(sqrDist(0, 0, ballX, ballY) > 100 && isAngleWithinInterval(getAngle(ballX, ballY), 0, 20)) {
        stateMachine.SetState(State::Chase); // placeholder
    }
    else {
        // if close to enemy goal, shoot
        if(sqrDist(0, 0, enemyGoalX, enemyGoalY) < 600) {
            
        }
        // else move to enemy goal
        else {
            moveToAndFace(speed, enemyGoalX, enemyGoalY);
        }
    }
}

void switchToGoalie() {

}

void goalieLoop() {
    // placeholder
    int _goalX = 0;
    int _goalY = 0;
    int _allowMoveLeft = 1;
    int _allowMoveRight = 1;

    int maxGoalAllowanceX = 15;
    // if a white line is seen on the left or right
    // check if the allied goal is close
    // if so, we are in the goal box
    if(vemlData[VEMLDirection::Left]) {
        if(abs(_goalX) <= maxGoalAllowanceX) {
            _allowMoveLeft = 0;
        }
    }
    else if(vemlData[VEMLDirection::Right]) {
        if(abs(_goalY) <= maxGoalAllowanceX) {
            _allowMoveRight = 0;
        }
    }

    int rotation = turnDirectionToAngle(currentHeading, forwardHeading) * 60;
    float speed = 100.0;
    float _ballAngle = getAngle(ballX, ballY);
    if(!isAngleWithinInterval(_ballAngle, 0, 15)) {
        if(ballX < 0 && _allowMoveLeft) {
            // hm.move(90, speed, rotation);
            MOVEDIR = 90;
            MOVESPEED = speed;
            MOVEROT = rotation;
        }
        if(ballX > 0 && _allowMoveRight) {
            // hm.move(270, speed, rotation);
            MOVEDIR = 270;
            MOVESPEED = speed;
            MOVEROT = rotation;
        }
    }

    // TODO: put this in the correct if statements
    // right now its just here

    // move to ball if close
    int maxSqrDist = 80; // placeholder value

    if(sqrDist(0, 0, ballX, ballY) < maxSqrDist) {
        // placeholder
        // should be positive or negative (not just scalar)
        float _dBallToGoal = 0;
        float _dToGoal = 0;
        float _dToBall = 0;
        // ball is in front of robot and goal
        if(_dToBall > 0) {
            //move towards it
        }
    }
}

#pragma region TESTING
// add _s after test name to skip
// ex: test(A_s) is skipped

// std::string expectFormat(float a, float b, std::string varName) {
//     std::string s;
//     sprintf(s, "%s=%f, expected %f", varName, a, b);
//     return s;
// }

// test(Angles) {
//     int err = 1;
//     assertNear(getAngle(0, 1), 0, err);
//     assertNear(getAngle(-1, 1), 45, err);
//     assertNear(getAngle(-1, 0), 90, err);
//     assertNear(getAngle(-1, -1), 135, err);
//     assertNear(getAngle(0, -1), 180, err);
//     assertNear(getAngle(1, -1), 225, err);
//     assertNear(getAngle(1, 0), 270, err);

//     assertTrue(isAngleWithinInterval(0, 0, 2));
//     assertTrue(isAngleWithinInterval(70, 80, 12));
//     assertTrue(isAngleWithinInterval(170, 190, 30));
//     assertTrue(isAngleWithinInterval(359, 0, 2));
//     assertTrue(isAngleWithinInterval(359, 1, 3));
//     assertTrue(isAngleWithinInterval(1, 359, 3));
//     assertFalse(isAngleWithinInterval(181, 179, 1));
//     assertFalse(isAngleWithinInterval(89, 94, 3));

//     assertEqual(turnDirectionToAngle(80, 90), 1);
//     assertEqual(turnDirectionToAngle(0, 1), 1);
//     assertEqual(turnDirectionToAngle(359, 0), 1);
//     assertEqual(turnDirectionToAngle(90, 269), 1);
//     assertEqual(turnDirectionToAngle(91, 270), 1);
//     assertEqual(turnDirectionToAngle(181, 0), 1);
//     assertEqual(turnDirectionToAngle(90, 80), -1);
//     assertEqual(turnDirectionToAngle(1, 0), -1);
//     assertEqual(turnDirectionToAngle(0, 359), -1);
//     assertEqual(turnDirectionToAngle(269, 90), -1);
//     assertEqual(turnDirectionToAngle(270, 91), -1);
//     assertEqual(turnDirectionToAngle(0, 181), -1);
//     assertEqual(turnDirectionToAngle(0, 0), 0);

// }

// Testing with ArduinoUnit
void setupTests() {
    Test::exclude("*_s");
    forwardHeading = readCompassHeading();
    hm.stop();
}
void runTests() {
    // Test::run();
    // Serial.println(readCompassHeading());
    // Serial.println(readCompassHeading() - forwardHeading);
    delay(300);
}

#pragma endregion


void setup() {
    // _t.setup();
    Serial.begin(9600);
    Serial.println("Display Controller test");
    roboclaw.begin(38400); 
    Wire.begin();

    // test async
    // _a.setTimeout([&] {
    //     Serial.println("2");
    // }, 2000);
    // _a.setTimeout([&] {
    //     Serial.println("1");
    // }, 1000);
    // _a.setTimeout([&] {
    //     _a.list();
    // }, 3000);
    // Serial.println("1aaa");
    // _a.setTimeout(testPrint, 2000);
    // Serial.println("2");
    // _a.setTimeout(testPrint, 1000);
    // _a.list();

    // joystick 
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    // dribbler and proximity sensors
    // ESC.attach(PROXIMITY_PIN, PROXIMITY_MIN_PULSE, PROXIMITY_MAX_PULSE);
    // ESC.write(0);

    // for (uint8_t t=0; t<8; t++) {
    //   tcaselect(t);
    //   Serial.print("TCA Port #"); Serial.println(t);

    //   for (uint8_t addr = 0; addr<=127; addr++) {
    //     if (addr == 0x70) continue;

    //     Wire.beginTransmission(addr);
    //     if (!Wire.endTransmission()) {
    //       Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
    //     }
    //   }
    // }
    // Serial.println("\ndone");

    // VEML7700 (light sensors)
    // for(int i = 0; i < 4; i++) {
    //     printFormat("tca select %d", i);
    //     tcaselect(i);
    //     vemls[i] = Adafruit_VEML7700();
    //     // while(!vemls[i].begin()) {
    //     //     printFormat("Failed to start VEML %d", i);
    //     //     delay(500);
    //     // }
    //     if(!vemls[i].begin()) {
    //         printFormat("Failed to start VEML %d", i);
    //         while(1);
    //     }

    //     vemls[i].setGain(VEML7700_GAIN_1);
    //     vemls[i].setIntegrationTime(VEML7700_IT_100MS); // 25, 50, 100, 200, 400, 800MS
    //     vemls[i].setLowThreshold(10000);
    //     vemls[i].setHighThreshold(20000);
    //     vemls[i].interruptEnable(false); 
    // }
    
    // Serial.println("veml");
    // Adafruit_VEML7700 v = Adafruit_VEML7700();
    // Serial.println("tca");
    // tcaselect(0);
    // Serial.println("begin");
    // v.begin();
    // Serial.println("began");
    // v.setGain(VEML7700_GAIN_1);
    // v.setIntegrationTime(VEML7700_IT_100MS); // 25, 50, 100, 200, 400, 800MS
    // v.setLowThreshold(10000);
    // v.setHighThreshold(20000);
    // v.interruptEnable(false);
    // Serial.println(v.readLux());

    // LSM303DLHC (compass)
    while(!compass.begin()) {
        Serial.println("Failed to start compass");
        delay(500);
    }

    // kicker
    // pinMode(KICKRELAY, OUTPUT);
    // pinMode(CHARGERELAY, OUTPUT);
    // digitalWrite(KICKRELAY, LOW);
    // digitalWrite(CHARGERELAY, HIGH);
    

    // StateMachine
    stateMachine.SetCallbacks(State::Default, &setupTests, &runTests);
    stateMachine.SetCallbacks(State::Chase, nullptr, &chaseBallLoop);
    stateMachine.SetCallbacks(State::Score, &dribbleOn, &moveBallToEnemyGoal, &dribbleOff);


    // TFT Display
    const char *dataLabels[4] = {"Label 1", "Move to ball", "Current Ally Goal: B", "Camera"};
    myDisplay.begin(TFT_DC_PIN, TFT_CS_PIN, TFT_PWM_PIN, SPI, SPI_SPEED);
    myDisplay.switchToMenuWindow(dataLabels, 4);
    // myDisplay.switchToLogWindow(dataLabels, 4);
    // myDisplay.updateLogValue(0, "aaa");
    myDisplay.updateDisplay();

    myDisplay.setOnClickForWindow(1, &allowRobotMove);
    myDisplay.setOnClickForWindow(2, &switchAllyGoal);
    myDisplay.setOnClickForWindow(3, &switchToCameraViewer);
    // myTFT.line(1, 1, 9, 9, 3);

    stateMachine.Start(State::Default);

    hm.stop();
    
}


void loop() {
    // _t.handle();
    MOVESPEED = 0;
    MOVEROT = 0;
    MOVEDIR = 0;
    // hm.setSpeeds(100, 100, 100, 100);
    // return;
    // test async
    // _a.update();
    // return;
    // float _d = 0;
    // hm.setSpeeds(0, 0, 0, 0);
    // while(1) {
    //     if(myDisplay.hadNewJoystickInput(analogRead(A0), analogRead(A1))) delay(50000);
    //     hm.move(_d, 100, 0);
    //     char _a[0];
    //     sprintf(_a, "%f", _d);
    //     myDisplay.setMenuLabel(0, _a);
    //     myDisplay.updateDisplay();
    //     _d += 45;
    //     delay(10000);
    // }
    // return;

    // get sensor data
    // if(1) {
    //     Serial.println(vemls[0].readLux());
    //     delay(500);
    //     return;
    // }
    stateMachine.StateLoop();
    if(1) {
        hm.move(MOVEDIR, MOVESPEED, MOVEROT);
    }
    // return;
    // Log test
    // myDisplay.updateLogValue(1, itoa(i, istr, 10));
    // Serial.println(istr);
    // i++;
    // myDisplay.updateDisplay();
    // if(i > 100) i = 0;

    // delay(200);


    // Menu test
    int rawJoyX = analogRead(A0);
    int rawJoyY = analogRead(A1);
    int p = digitalRead(JOY_SW_PIN);
    // Serial.println(p);
    
    int hadNewJ = myDisplay.hadNewJoystickInput(rawJoyX, rawJoyY);
    // if(hadNewJ) stateMachine.SetState(State::Default);
    if(p == 0) {
        Serial.println("press");
        myDisplay.handleJoystickInputUtil(DisplayController::JoystickInputType::IN_CLICK);
    }
    else {
        myDisplay.handleJoystickAxis(rawJoyX, rawJoyY);
    }
    myDisplay.updateDisplay();

    // if(!robotCanMove) hm.stop();

    // // holo move test
    // if(robotCanMove) {
    //     if(hadNewJ) {
    //         robotCanMove = 0;
    //         Serial.println("stop");
    //         hm.stop();
    //         return;
    //     }

    //     // getDataFromCamera();
    //     if(!ballSeen) return;
    //     Serial.println(ang);
    //     hm.move(ang, 100);
    // }
    delay(50);
}