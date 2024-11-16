#include "Arduino.h"
#include "WiFiS3.h"

#include "arduino_secrets.h"
#include <Arduino_LED_Matrix.h>

#include <ArduinoJson.h>
#include "ArduinoMqttClient.h"

#include "messages.h"

// uncomment to enable mqtt
// #define MQTT_ENABLE 

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

// ##### decls #####
void printWifiStatus();
void setup_wifi();
void setup_mqtt();

void setup()
{
  Serial.begin(9600); // initialize serial communication
  matrix.begin();

  setup_wifi();

  #if defined(MQTT_ENABLE)
  setup_mqtt();
  #endif
  
  matrix.loadFrame(frames[MATRIX_OK]);
}

void loop()
{
  // check sensors for crash
  // if crash send crash
  // else check if position update threshold has passed
  // if threshold passed then send position update

  #if defined(MQTT_ENABLE)
  mqttclient.poll();

  uint64_t now = millis();
  if (now - last_position_update_ts > 10000)
  {
    last_position_update_ts = now;
    send_position_update(mqttclient, {
      	.device = SECRET_DEVICE_ID,
        .longitude = 10.402873236388588,
        .latitude = 44.085398523165935,
    });
  }

  if (now - last_crash_update_ts  > 30000)
  {
    last_crash_update_ts = now;
    send_crash_notification(mqttclient, {
        .device = SECRET_DEVICE_ID,
        .longitude = 12.402873236388588,
        .latitude = 43.085398523165935,
    });
  }
  #endif
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

// ##### #####
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
