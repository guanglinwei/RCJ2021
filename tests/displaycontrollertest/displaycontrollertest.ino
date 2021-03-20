#include <HyperDisplay_KWH018ST01_4WSPI.h>
#include <DisplayController.h>

#define TFT_PWM_PIN 13         // Pin definitions
#define TFT_CS_PIN 45
#define TFT_DC_PIN 44
#define SPI_PORT SPI
#define SPI_SPEED 32000000 

KWH018ST01_4WSPI myTFT;
DisplayController myDisplay(myTFT, Serial);

// char *dataLabels[4];
int i = 0;
char istr[4];
void setup() {
    Serial.begin(9600);
    Serial.println("Display Controller test");
    const char *dataLabels[4] = {"Label 1", "Label 2", "Label 3", "Label 4"};
    myDisplay.begin(TFT_DC_PIN, TFT_CS_PIN, TFT_PWM_PIN, SPI, SPI_SPEED);
    myDisplay.switchToLogWindow(dataLabels);
    myDisplay.updateLogValue(0, "aaa");
    myDisplay.updateDisplay();
    // myTFT.line(1, 1, 9, 9, 3);
    
}

void loop() {
    myDisplay.updateLogValue(1, itoa(i, istr, 10));
    // Serial.println(istr);
    i++;
    myDisplay.updateDisplay();
    if(i > 100) i = 0;

    delay(200);
}