#include <Adafruit_VEML7700.h>
#include <Wire.h>

#define TCAADDR1 0x70

Adafruit_VEML7700 veml0 = Adafruit_VEML7700();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("ll");

  Wire.begin();

  tcaselect(0);
  Serial.println("ok");
  if (!veml0.begin()) {
    Serial.println("Sensor not found");
    while (1);
  }
  Serial.println("sensor found");

  veml0.setGain(VEML7700_GAIN_1);
  veml0.setIntegrationTime(VEML7700_IT_800MS);
  veml0.setLowThreshold(10000);
  veml0.setHighThreshold(20000);
  veml0.interruptEnable(false);  
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println(veml0.readLux());
}

void tcaselect(uint8_t i) {
  if (i > 7) return;
  Serial.println(i);
  Wire.beginTransmission(TCAADDR1);
  Wire.write(1 << i);
  Wire.endTransmission();  
}
