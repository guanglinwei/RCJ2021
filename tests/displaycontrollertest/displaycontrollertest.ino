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
int ballSeen = 0;


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
//   return fmod(fmod(-atan2(x - HALF_CAM_W, y - HALF_CAM_H) * RAD_TO_DEG, 360) + 360 , 360);
    // test
    Serial.print("get "); Serial.print(x); Serial.print(" "); Serial.println(y);
    return fmod(fmod(-atan2(x, y) * RAD_TO_DEG, 360) + 360 , 360);
}

void getDataFromCamera()
{
    // lastBallX = ballX < 0 ? lastBallX : ballX;
    // lastBallY = ballY < 0 ? lastBallY : ballY;
    lastBallX = ballX;
    lastBallY = ballY;

    Serial.println("getting blocks");
    pixy.ccc.getBlocks();
    Serial.println("got blocks");

    if(!pixy.ccc.numBlocks) {
        ballSeen = 0;
        Serial.println("none\n");
        return;
    }

    for(int i = 0; i < pixy.ccc.numBlocks; i++){
        // Serial.println(pixy.ccc.blocks[i].m_signature);
        // if ball
        if(pixy.ccc.blocks[i].m_signature == 1){
            ballSeen = 1;
            ballX = pixy.ccc.blocks[i].m_x - HALF_CAM_W;
            ballY = pixy.ccc.blocks[i].m_y - HALF_CAM_H;

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
    // Serial.print("joystick | ");

    Serial.println("update display | ");
    myDisplay.updateDisplay();

    if(!robotCanMove) hm.stop();

    // holo move test
    if(robotCanMove) {
        if(hadNewJ) {
            robotCanMove = 0;
            Serial.println("stop");
            hm.stop();
            return;
        }

        // getDataFromCamera();
        if(!ballSeen) return;
        Serial.println(ang);
        hm.move(ang, 100);
    }
    Serial.println("---\n");
    delay(300);
}