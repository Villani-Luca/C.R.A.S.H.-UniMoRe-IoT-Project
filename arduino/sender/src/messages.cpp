#include "messages.h"

void send_position_update(MqttClient mqtt, PositionUpdate data){
    JsonDocument json;

    json["device"] = data.device;
    json["longitude"] = data.longitude;
    json["latitude"] = data.latitude;

    publish_json(mqtt, json, POSITION_TOPIC);
}

void send_crash_notification(MqttClient mqtt, CrashNotification data){
    JsonDocument json;

    json["device"] = data.device;
    json["longitude"] = data.longitude;
    json["latitude"] = data.latitude;

    publish_json(mqtt, json, CRASH_TOPIC);
}

void publish_json(MqttClient mqtt, JsonDocument json, const char* topic){
    String output;
    
    // TODO error handleing
    serializeJson(json, output);

    mqtt.beginMessage(topic);
    mqtt.print(output);
    mqtt.endMessage();
}