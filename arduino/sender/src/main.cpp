#include "Arduino.h"
#include "WiFiS3.h"

#include "arduino_secrets.h"
#include <Arduino_LED_Matrix.h>

#include <ArduinoJson.h>
#include "ArduinoMqttClient.h"

// please enter your sensitive data in the Secret tab/arduino_secrets.h
WiFiSSLClient wificlient;
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)

// TODO da cambiarli in modo che usino / invece che trattini
const char position_topic[] = "client-position-update";
const char crash_notification_topic[] = "client-crash-notification";
const char crash_topic[] = "update/"SECRET_DEVICE_ID"/crash"; // TODO sarebbe carino farlo che sia configurabile
MqttClient mqttclient(wificlient);

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;

enum SetupPhase
{
  MATRIX_OK = 0,
  MATRIX_WIFI = 1,
  MATRIX_H = 2,
  MATRIX_L = 3
};

// LED MATRIX
ArduinoLEDMatrix matrix;
const unsigned long frames[4][3] = {
    {0x3184a444, 0x42081100, 0xa0040000}, // HEARTH
    {0x0003FC40, 0x29F92044, 0xE2110040}, // WIFI
    {0x00000030, 0xC30C3FC3, 0x0C30C000}, // H
    {0x00030030, 0x03003003, 0x003FC000}, // L
};

uint64_t last_position_update_ts = 0;
uint64_t last_crash_update_ts = 0;

void printWifiStatus();
void setup_wifi();
void setup_mqtt();

void setup()
{
  Serial.begin(9600); // initialize serial communication
  matrix.begin();

  setup_wifi();
  setup_mqtt();
  
  matrix.loadFrame(frames[MATRIX_OK]);
}

void loop()
{
  int messageSize = mqttclient.parseMessage();
  if (messageSize) {
    // we received a message, print out the topic and contents
    Serial.print("Received a message with topic '");
    Serial.print(mqttclient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // use the Stream interface to print the contents
    while (mqttclient.available()) {
      Serial.print((char)mqttclient.read());
    }
    Serial.println();
    Serial.println();
  }

  uint64_t now = millis();
  if (now - last_position_update_ts > 10000)
  {
    last_position_update_ts = now;

    // creat JSON message for Socket.IO (event)
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();

    json["device"] = SECRET_DEVICE_ID;
    json["longitude"] = 10.402873236388588;
    json["latitude"] = 44.085398523165935;

    // JSON to String (serializion)
    String output;
    serializeJson(doc, output);
  
    // Print JSON for debugging
    Serial.println(output);

    mqttclient.beginMessage(position_topic);
    mqttclient.print(output);
    mqttclient.endMessage();
  }

  if (now - last_crash_update_ts  > 30000)
  {
    last_crash_update_ts = now;

    // creat JSON message for Socket.IO (event)
    DynamicJsonDocument doc(1024);
    JsonObject json = doc.to<JsonObject>();

    json["device"] = SECRET_DEVICE_ID;
    json["longitude"] = 12.402873236388588;
    json["latitude"] = 43.085398523165935;

    // JSON to String (serializion)
    String output;
    serializeJson(doc, output);
  
    // Print JSON for debugging
    Serial.println(output);

    mqttclient.beginMessage(crash_notification_topic);
    mqttclient.print(output);
    mqttclient.endMessage();
  }
}

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
  
  mqttclient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
  if(!mqttclient.connect(SECRET_MQTT_HOST, SECRET_MQTT_PORT)){
    Serial.print("MQTT connection failed! Error Code = ");
    Serial.println(mqttclient.connectError());

    while(1);
  }

  Serial.println("Connected to MQTT Broker");
  Serial.println();

  Serial.print("Subscribing to ");
  Serial.print(crash_topic);
  Serial.println();

  mqttclient.subscribe(crash_topic);
}
