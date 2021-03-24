#include <HyperDisplay_KWH018ST01_4WSPI.h>
#include <DisplayController.h>

#define TFT_PWM_PIN 13         // Pin definitions
#define TFT_CS_PIN 45
#define TFT_DC_PIN 44
#define SPI_PORT SPI
#define SPI_SPEED 32000000 

#define JOY_SW_PIN 2

KWH018ST01_4WSPI myTFT;
DisplayController myDisplay(myTFT, Serial);

// char *dataLabels[4];
int i = 0;
char istr[4];

void testprint() {
    Serial.println("Aaa");
}
void setup() {
    Serial.begin(9600);
    Serial.println("Display Controller test");

    // joystick 
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(JOY_SW_PIN, INPUT_PULLUP);

    
    const char *dataLabels[4] = {"Label 1", "Label 2", "Label 3", "Label 4"};
    myDisplay.begin(TFT_DC_PIN, TFT_CS_PIN, TFT_PWM_PIN, SPI, SPI_SPEED);
    myDisplay.switchToMenuWindow(dataLabels, 4);
    // myDisplay.switchToLogWindow(dataLabels, 4);
    // myDisplay.updateLogValue(0, "aaa");
    myDisplay.updateDisplay();

    myDisplay.addOnClickToWindow(2, &testprint);
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
    // Serial.println(p);
    // Serial.println(rawJoyX);
    // Serial.println(rawJoyY);
    if(p == 0) {
        myDisplay.handleJoystickInputUtil(DisplayController::JoystickInputType::IN_CLICK);
    }
    else {
        myDisplay.handleJoystickAxis(rawJoyX, rawJoyY);
    }

    myDisplay.updateDisplay();
    delay(100);
}