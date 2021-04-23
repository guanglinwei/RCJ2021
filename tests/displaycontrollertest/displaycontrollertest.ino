#include <HyperDisplay_KWH018ST01_4WSPI.h>
#include <DisplayController.h>
#include <HoloMove.h>
#include <RoboClaw.h>
#include <Pixy2.h>

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

#define JOY_SW_PIN 2

KWH018ST01_4WSPI myTFT;
DisplayController myDisplay(myTFT, Serial);
RoboClaw roboclaw(&Serial2, 10000);
HoloMove hm(&roboclaw, Serial);
Pixy2 pixy;

// camera viewer
int ballX, ballY, lastBallX, lastBallY = 0;
double ang = 0.0; // angle towards ball. up is 0, increases CCW


int robotCanMove = 0;

// char *dataLabels[4];
int i = 0;
char istr[4];

void testprint() {
    Serial.println("Click label 3");
}

void allowRobotMove() {
    robotCanMove = 1;
}


double getAngle(int x, int y)
{
  //0, 0 is top left
  return fmod(fmod(-atan2(x - HALF_CAM_W, y - HALF_CAM_H) * RAD_TO_DEG, 360) + 360 , 360);
}

void getDataFromCamera()
{
    lastBallX = ballX;
    lastBallY = ballY;

    pixy.ccc.getBlocks();

    if(!pixy.ccc.numBlocks) {
        ballX = -1;
        ballY = -1;
        return;
    }

    for(int i = 0; i < pixy.ccc.numBlocks; i++){
        Serial.println(pixy.ccc.blocks[i].m_signature);
        // if ball
        if(pixy.ccc.blocks[i].m_signature == 1){
            ballX = pixy.ccc.blocks[i].m_x;
            ballY = pixy.ccc.blocks[i].m_y;
            ang = getAngle(ballX, ballX);
            ang += 180;
            if(ang > 360) {
                ang -= 360;
            }
            return;
        }
    }
}

void cameraViewerLoop() {
    getDataFromCamera();

    if(lastBallX != -1) {
        myDisplay.pixel(lastBallX, lastBallY, myDisplay.defaultColor);
    }

    if(ballX != -1) {
        myDisplay.pixel(ballX, ballY, myDisplay.ORANGE);
    }
    else {
        myDisplay.pixel(ballX, ballY, myDisplay.LIGHT_GRAY);
    }
}

void switchToCameraViewer() {
    myDisplay.clearDisplay();
    myDisplay.myTFT.circle(64, 70, 54, false, myDisplay.WHITE);
    myDisplay.switchToCustomWindow();
}

void setup() {
    Serial.begin(9600);
    Serial.println("Display Controller test");
    roboclaw.begin(38400); 

    // joystick 
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    
    const char *dataLabels[4] = {"Label 1", "Move to ball", "Label 3", "Camera"};
    myDisplay.begin(TFT_DC_PIN, TFT_CS_PIN, TFT_PWM_PIN, SPI, SPI_SPEED);
    myDisplay.switchToMenuWindow(dataLabels, 4);
    // myDisplay.switchToLogWindow(dataLabels, 4);
    // myDisplay.updateLogValue(0, "aaa");
    myDisplay.updateDisplay();

    myDisplay.setOnClickForWindow(1, &allowRobotMove);
    myDisplay.setOnClickForWindow(2, &testprint);
    myDisplay.setOnClickForWindow(3, &switchToCameraViewer);
    // myTFT.line(1, 1, 9, 9, 3);
    
}


void loop() {
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
    
    int hadNewJ = myDisplay.hadNewJoystickInput(rawJoyX, rawJoyY);
    if(p == 0) {
        myDisplay.handleJoystickInputUtil(DisplayController::JoystickInputType::IN_CLICK);
    }
    else {
        myDisplay.handleJoystickAxis(rawJoyX, rawJoyY);
    }

    myDisplay.updateDisplay();
    if(!robotCanMove) hm.stop();

    // holo move test
    if(robotCanMove) {
        if(hadNewJ) {
            robotCanMove = 0;
            hm.stop();
            return;
        }

        getDataFromCamera();
        if(ballx == -1) return;
        Serial.println(ang);
        hm.move(ang, 100);
    }
    delay(100);
}