#pragma once

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

struct AccelReading {
    float x;
    float y;
    float z;
};

struct Position {
    float longitude;
    float latitude;
};

void setup_sensors();
void accel_read(AccelReading& reading);

