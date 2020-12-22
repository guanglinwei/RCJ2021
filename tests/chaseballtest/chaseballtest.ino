#include <StateMachine.h>
#include <RoboClaw.h>
#include "HoloMove.h"
#include "HyperDisplay_KWH018ST01_4WSPI.h" // Click here to get the library: http://librarymanager/All#SparkFun_HyperDisplay_KWH018ST01_4WSPI
#include "math.h"
// #include <Pixy2I2C.h>
#include <Pixy2.h>
#include <stdio.h>
#include <stdarg.h>


#define TFT_PWM_PIN 2         // Pin definitions
#define TFT_CS_PIN 45
#define TFT_DC_PIN 44
#define JOY_X_PIN 0
#define JOY_Y_PIN 1
#define JOY_SWITCH_PIN 2

#define SPI_PORT SPI
#define SPI_SPEED 32000000   
#define SERIAL_PORT Serial; 

#define CAM_W 316
#define CAM_H 208
#define HALF_CAM_W 158
#define HALF_CAM_H 104

#define RAD_TO_DEG 57.295779513082320876798154814105


volatile int joyX = 0, joyY = 0; //values from joystick
volatile bool allowButton = true; //allow joystick press input

boolean isYellow = false;
int test = 0;
boolean monitorColors = false;
int ballx, bally,tempx, tempy = 0;
double ang = 0; // angle towards ball. up is 0, increases CCW
float speed = 255.0;
float a,b,c,d; // motor speed for a-d
double dist = 0;

int maxTempCount = 8;
int tempCount = 0;

ILI9163C_color_18_t defaultColor;

StateMachine stateMachine;
// Pixy2I2C pixy;
Pixy2 pixy;
RoboClaw roboClaw(&Serial2, 10000);
HoloMove holoMove(&roboClaw);
KWH018ST01_4WSPI displayTFT;


void setup() {
  pinMode(JOY_SWITCH_PIN, INPUT_PULLUP);
  digitalWrite(JOY_SWITCH_PIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(JOY_SWITCH_PIN), switchPinInterrupt, HIGH);

  stateMachine = StateMachine(2);
  stateMachine.SetCallbacks(0, nullptr, &chaseLoop, nullptr);

  Serial.begin(115200);
  roboClaw.begin(38400);

  displayTFT.begin(TFT_DC_PIN, TFT_CS_PIN, TFT_PWM_PIN, SPI_PORT, SPI_SPEED);
  displayTFT.clearDisplay();

  setupColors();

  Serial.println("init pixy");
  pixy.init();
  // while(!pixy.init()){
  //   Serial.println("...");
  // }
  Serial.println("done");

  stateMachine.SetState(0);
}

void setupColors()
{
  displayTFT.setTextCursor(0, 0);

  defaultColor.r = 0xFF;
  defaultColor.g = 0x00;
  defaultColor.b = 0x00;

  displayTFT.setCurrentWindowColorSequence((color_t)&defaultColor);

  displayTFT.line(16, 16, 46, 46);
}

// void setCurrentWindowColor(ILI9163C_color_18_t* col){

// }
char stringToPrint[50];
template<typename... Args> void printAtOrigin(int x, int y, const char* format, Args... args)
{
  // if(clear) displayTFT.clearDisplay();
  displayTFT.setTextCursor(x, y);
  sprintf(stringToPrint, format, args...);
  displayTFT.println(stringToPrint);

}

void clear(){
  displayTFT.setTextCursor(0, 0);
  displayTFT.clearDisplay();
}

void switchPinInterrupt()
{
  if(allowButton) allowButton = false;
}

double getAngle(int x, int y)
{
  //0, 0 is top left
  return fmod(fmod(-atan2(x - HALF_CAM_W, y - HALF_CAM_H) * RAD_TO_DEG, 360) + 360 , 360);
}

void getDataFromCamera()
{
  ang++;
  if(ang >= 360) ang = 0;
  return;
  pixy.ccc.getBlocks();

  if (pixy.ccc.numBlocks){
    for(int i = 0; i < pixy.ccc.numBlocks; i++){
      if(pixy.ccc.blocks[i].m_signature == 1){
        ballx = pixy.ccc.blocks[i].m_x;
        bally = pixy.ccc.blocks[i].m_y;
        ang = getAngle(ballx, bally);
      }
    }
  }
}

void loop() 
{
//  roboClaw.ForwardBackwardM1(0x80, 63);
  stateMachine.StateLoop();
}

void chaseLoop()
{
  if(tempCount <= maxTempCount){
    tempCount++;
    return;
  }
  tempCount = 0;
  getDataFromCamera();
  holoMove.move(ang, speed, 0, &a, &b, &c, &d);

  clear();
  printAtOrigin(0, 0, "moving to (%d, %d)", ballx, bally);
  printAtOrigin(0, 10, "at angle %d", (int)ang);
  printAtOrigin(0, 20, "%d | %d | %d | %d", (int)a, (int)b, (int)c, (int)d);
}