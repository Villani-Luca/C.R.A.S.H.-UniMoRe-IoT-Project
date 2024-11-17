#include <Arduino.h>
#include <WiFiS3.h>
#include <Arduino_LED_Matrix.h>
#include <ArduinoJson.h>
#include <ArduinoMqttClient.h>
#include "modules.h"

#include "messages.h"
#include "arduino_secrets.h"
#include "sensors.h"

// ##### WIFI #####
int status = WL_IDLE_STATUS;
WiFiSSLClient wificlient;
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)

// ##### MQTT #####
String device_crash_topic = DEVICE_CRASH_TOPIC(SECRET_DEVICE_ID); 
constexpr short mqtt_keep_alive = 90 * 1000; // 90 secondi keep alive
MqttClient mqttclient(wificlient);

// ##### Led Matrix #####
enum SetupPhase
{
  MATRIX_OK = 0,
  MATRIX_WIFI = 1,
  MATRIX_H = 2,
  MATRIX_L = 3
};

ArduinoLEDMatrix matrix;
const unsigned long frames[4][3] = {
    {0x3184a444, 0x42081100, 0xa0040000}, // HEARTH
    {0x0003FC40, 0x29F92044, 0xE2110040}, // WIFI
    {0x00000030, 0xC30C3FC3, 0x0C30C000}, // H
    {0x00030030, 0x03003003, 0x003FC000}, // L
};

// ##### Update control params #####
uint64_t last_position_update_ts = 0;
uint64_t last_crash_update_ts = 0;
uint64_t last_accel_reading_ts = 0;
uint64_t last_gps_reading_ts = 0;

float acceleration_threshold = 2;
//constexpr float gravity = 9.8;

constexpr byte max_data_points = 10;
constexpr float alpha_step = 1.0 / max_data_points;
float alpha = 1.0;
float acceleration_avg = 0;

// ##### decls #####
void printWifiStatus();
void setup_wifi();
void setup_mqtt();
float accel_module(AccelReading reading);

void setup()
{
  Serial.begin(9600); // initialize serial communication
  matrix.begin();

  #if WIFI_ENABLED
  setup_wifi();
  #endif

  #if MQTT_ENABLED
  setup_mqtt();
  #endif

  #if SENSOR_ENABLED
  setup_sensors();
  #endif
  
  matrix.loadFrame(frames[MATRIX_OK]);
}

void loop()
{
  AccelReading accel_reading;
  Position gps_position = {
    .longitude = 10.402873236388588,
    .latitude = 44.085398523165935,
  };
  uint64_t now = millis();
  bool crash = false;

  #if SENSOR_ACCEL_ENABLED
  if(now - last_accel_reading_ts > 200){
    last_accel_reading_ts = now;
    accel_read(accel_reading);
    float module = accel_module(accel_reading);
    acceleration_avg = (alpha * module) + (1.0 - alpha) * acceleration_avg;

    Serial.print("X: "); Serial.print(accel_reading.x); Serial.print(" ");
    Serial.print("Y: "); Serial.print(accel_reading.y); Serial.print(" ");
    Serial.print("Z: "); Serial.print(accel_reading.z); Serial.print(" ");
    Serial.print("T: "); Serial.print(module); Serial.print(" ");
    Serial.print("A: "); Serial.print(acceleration_avg); Serial.print(" ");
    Serial.println();

    // TODO: sistema di controllo piú sofisticato per evitare falsi positivi ecc...
    if(module - acceleration_avg > acceleration_threshold){
      Serial.println("CRASH CRASH CRASH CRASH CRASH");
      crash = true;
    }

    if(alpha > alpha_step){
      alpha -= alpha_step;
    }
  }
  #endif

  #if SENSOR_GPS_ENABLED

  if(now - last_gps_reading_ts > 200){
    last_gps_reading_ts = now;
    gps_read(gps_position);
  
    Serial.print("Latitude= "); 
    Serial.print(gps_position.latitude, 6);
    Serial.print(" Longitude= "); 
    Serial.print(gps_position.longitude, 6);
    Serial.println();
    Serial.println();
  }
  #endif

  // check sensors for crash
  // if crash send crash
  // else check if position update threshold has passed
  // if threshold passed then send position update

  #if MQTT_ENABLED
  mqttclient.poll();

  if (now - last_position_update_ts > 10000)
  {
    last_position_update_ts = now;
    send_position_update(mqttclient, {
      	.device = SECRET_DEVICE_ID,
        .longitude = gps_position.longitude,
        .latitude = gps_position.latitude,
    });
  }

  if (crash)
  {
    last_crash_update_ts = now;
    send_crash_notification(mqttclient, {
        .device = SECRET_DEVICE_ID,
        .longitude = gps_position.longitude,
        .latitude = gps_position.latitude,
    });
  }
  #endif
}


// ##### #####
float accel_module(AccelReading reading){
  return sqrtf(reading.x*reading.x + reading.y*reading.y + reading.z*reading.z);
}

void onMqttMessage(int messageSize) {
    String messagetopic = mqttclient.messageTopic();

    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(messagetopic);
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // use the Stream interface to print the contents
    while (mqttclient.available()) {
      Serial.print((char)mqttclient.read());
    }
    Serial.println();
    Serial.println();

    if(messagetopic == device_crash_topic){
      // call device crash topic handler
    }
}

// ##### SETUP FUNCTION #####
void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup_wifi(){
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }
  matrix.loadFrame(frames[MATRIX_WIFI]);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware CURRENT: " + fv);
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid); // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(1000);
  }

  Serial.println("Connected...");

  printWifiStatus(); // you're connected now, so print out the status
}

void setup_mqtt(){
  if(status != WL_CONNECTED){
    Serial.println("Wifi not connected, interrupting");
    while(1);
  }
  
  mqttclient.onMessage(onMqttMessage);
  mqttclient.setKeepAliveInterval(mqtt_keep_alive);
  mqttclient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
  if(!mqttclient.connect(SECRET_MQTT_HOST, SECRET_MQTT_PORT)){
    Serial.print("MQTT connection failed! Error Code = ");
    Serial.println(mqttclient.connectError());

    while(1);
  }

  Serial.println("Connected to MQTT Broker");
  Serial.println();

  Serial.print("Subscribing to ");
  Serial.print(device_crash_topic);
  Serial.println();

  mqttclient.subscribe(device_crash_topic);
}
