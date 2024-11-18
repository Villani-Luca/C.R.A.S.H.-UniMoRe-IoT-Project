#pragma once
#include "ArduinoMqttClient.h"
#include "ArduinoJson.h"
#include "arduino_secrets.h"
#include "modules.h"

#define POSITION_TOPIC "client-position-update"
#define CRASH_TOPIC "client-crash-notification"
#define DEVICE_CRASH_TOPIC(d) device_topic(d)

struct PositionUpdate {
    const char device[10];
    double longitude; 
    double latitude;
};

struct CrashNotification {
    const char device[10];
    double longitude; 
    double latitude;
};

struct CrashAlert {
    double longitude; 
    double latitude;
};

void send_position_update(MqttClient mqtt, PositionUpdate data);
void send_crash_notification(MqttClient mqtt, CrashNotification data);

void parse_crash_alert(const char* msg, CrashAlert& alert);

String device_topic(const char* d);