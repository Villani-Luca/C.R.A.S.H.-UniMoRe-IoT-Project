#pragma once

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#include "modules.h"

#define SENSOR_GPS_RX_PIN 4
#define SENSOR_GPS_TX_PIN 3
#define SENSOR_GPS_BAUD_RATE 9600

struct AccelReading {
    float x;
    float y;
    float z;
};

struct Position {
    double longitude;
    double latitude;
};

void setup_sensors();
void accel_read(AccelReading& reading);
void gps_read(Position& position);
