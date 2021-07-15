#include <Adafruit_LSM303_U.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_LSM303_Mag_Unified compass = Adafruit_LSM303_Mag_Unified(54321);

void displaySensorDetails(void)
{
  sensor_t sensor;
  compass.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Magnetometer Test"); Serial.println("");

    /* Enable auto-gain */
    compass.enableAutoRange(true);
    compass.begin();
    
    displaySensorDetails();
}

float c(float y, float x) {
    float v = atan2(y, x) / 3.1415926535 * 180;
    return v < 0 ? v + 360 : v;
}

float MagMinX, MagMaxX;
float MagMinY, MagMaxY;
float MagMinZ, MagMaxZ;

int count = 0;

void calibrate() {
    sensors_event_t magEvent;

    compass.getEvent(&magEvent);
    if (magEvent.magnetic.x < MagMinX) MagMinX = magEvent.magnetic.x;
    if (magEvent.magnetic.x > MagMaxX) MagMaxX = magEvent.magnetic.x;

    if (magEvent.magnetic.y < MagMinY) MagMinY = magEvent.magnetic.y;
    if (magEvent.magnetic.y > MagMaxY) MagMaxY = magEvent.magnetic.y;

    if (magEvent.magnetic.z < MagMinZ) MagMinZ = magEvent.magnetic.z;
    if (magEvent.magnetic.z > MagMaxZ) MagMaxZ = magEvent.magnetic.z;

    if(count > 10) {
        Serial.print("Mag Minimums: "); Serial.print(MagMinX); Serial.print("  ");Serial.print(MagMinY); Serial.print("  "); Serial.print(MagMinZ); Serial.println();
    Serial.print("Mag Maximums: "); Serial.print(MagMaxX); Serial.print("  ");Serial.print(MagMaxY); Serial.print("  "); Serial.print(MagMaxZ); Serial.println(); Serial.println();
        count = 0;
        return;
    }
    count++;
}

void loop() {
    calibrate();
    return;
    sensors_event_t event;
    compass.getEvent(&event);
    float _x = event.magnetic.x;
    float _y = event.magnetic.y;
    float _z = event.magnetic.z;
    Serial.println("------------------------------------");
    Serial.print("X / Y:  "); Serial.println(c(_x, _y));
    Serial.print("X / Z:  "); Serial.println(c(_x, _z));
    Serial.print("Y / X:  "); Serial.println(c(_y, _x));
    Serial.print("Y / Z:  "); Serial.println(c(_y, _z));
    Serial.print("Z / X:  "); Serial.println(c(_z, _x));
    Serial.print("Z / Y:  "); Serial.println(c(_z, _y));
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);
}