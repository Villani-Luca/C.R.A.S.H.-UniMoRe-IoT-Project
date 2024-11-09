/*
  WiFi Web Server LED Blink

  A simple web server that lets you blink an LED via the web.
  This sketch will print the IP address of your WiFi module (once connected)
  to the Serial Monitor. From there, you can open that address in a web browser
  to turn on and off the LED_BUILTIN.

  If the IP address of your board is yourAddress:
  http://yourAddress/H turns the LED on
  http://yourAddress/L turns it off

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the WiFi.begin() call accordingly.

  Circuit:
  * Board with NINA module (Arduino MKR WiFi 1010, MKR VIDOR 4000 and Uno WiFi Rev.2)
  * LED attached to pin 9

  created 25 Nov 2012
  by Tom Igoe

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#simple-webserver
 */

#include "Arduino.h"
#include "WiFiS3.h"

#include "arduino_secrets.h"
#include <Arduino_LED_Matrix.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>

#include <ArduinoJson.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;          // your network key index number (needed only for WEP)

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

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

#define WS_HOST "192.168.116.94"
#define MESSAGE_INTERVAL 30000
#define HEARTBEAT_INTERVAL 25000

SocketIOclient wsclient;

uint64_t messageTimestamp = 0;
uint64_t heartbeatTimestamp = 0;
bool isConnected = false;

void printWifiStatus();

void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case sIOtype_DISCONNECT:
    Serial.println("[WSc] Disconnected!");
    // Serial.println((char *)payload);
    // isConnected = false;
    break;
  case sIOtype_CONNECT:
    Serial.println("[WSc] Connected!");

    // send message to server when Connected
    isConnected = true;

    // join default namespace ( no auto join in Socket.IO V3)
    wsclient.send(sIOtype_CONNECT, "/");
    break;
    {
      char *sptr = NULL;
      int id = strtol((char *)payload, &sptr, 10);
      Serial.print("[IOc] get event: ");
      Serial.print((char *)payload);
      Serial.print(" id: ");
      Serial.println(id);

      if (id)
      {
        payload = (uint8_t *)sptr;
      }
      DynamicJsonDocument doc(1024);
      DeserializationError error = deserializeJson(doc, payload, length);
      if (error)
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      String eventName = doc[0];
      Serial.print("[IOc] event name: ");
      Serial.println(eventName.c_str());

      // Message Includes a ID for a ACK (callback)
      if (id)
      {
        // creat JSON message for Socket.IO (ack)
        DynamicJsonDocument docOut(1024);
        JsonArray array = docOut.to<JsonArray>();

        // add payload (parameters) for the ack (callback function)
        JsonObject param1 = array.createNestedObject();
        param1["now"] = millis();

        // JSON to String (serializion)
        String output;
        output += id;
        serializeJson(docOut, output);

        // Send event
        wsclient.send(sIOtype_ACK, output);
      }
    }
    break;
  case sIOtype_ACK:
    Serial.print("[IOc] get ack: ");
    Serial.println(length);
    break;
  case sIOtype_ERROR:
    Serial.print("[IOc] get error: ");
    Serial.println(length);
    break;
  case sIOtype_BINARY_EVENT:
    Serial.print("[IOc] get binary: ");
    Serial.println(length);
    break;
  case sIOtype_BINARY_ACK:
    Serial.print("[IOc] binary ack: ");
    Serial.println(length);
    break;
  }
}

void setup()
{
  Serial.begin(9600); // initialize serial communication
  matrix.begin();

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

  // server.begin();    // start the web server on port 80
  printWifiStatus(); // you're connected now, so print out the status

  // ##### Web socket #####
  Serial.println("Connecting to websocket...");
  wsclient.begin("192.168.116.94", 8011, "/socket.io/?EIO=4");
  wsclient.onEvent(socketIOEvent);
  
  Serial.print("Is websocket connected: ");
  Serial.println(wsclient.isConnected());
  //wsclient.setReconnectInterval(5000);

  matrix.loadFrame(frames[MATRIX_OK]);
}

void loop()
{
  wsclient.loop();

  uint64_t now = millis();
  if (now - messageTimestamp > 2000)
  {
    messageTimestamp = now;

    // creat JSON message for Socket.IO (event)
    DynamicJsonDocument doc(1024);
    JsonArray array = doc.to<JsonArray>();

    // add evnet name
    // Hint: socket.on('event_name', ....
    array.add("client-position-update");

    // add payload (parameters) for the event
    JsonObject param1 = array.createNestedObject();
    param1["deviceid"] = "DEV1";
    param1["location"] = array.createNestedObject();
    param1["location"]["x"] = 10.402873236388588;
    param1["location"]["y"] = 44.085398523165935;
    param1["timestamp"] = "2024-11-08T23:06:31.300Z";

    // JSON to String (serializion)
    String output;
    serializeJson(doc, output);

    // Send event
    wsclient.sendEVENT(output);

    // Print JSON for debugging
    Serial.println(output);
  }
  // wsclient.send("[client-position-update, {\"device\": \"DEV4\"}]", 49);

  // WiFiClient client = server.available(); // listen for incoming clients
  //  if (client)
  //  {                                 // if you get a client,
  //      Serial.println("new client"); // print a message out the serial port
  //      String currentLine = "";      // make a String to hold incoming data from the client
  //      while (client.connected())
  //      { // loop while the client's connected
  //          if (client.available())
  //          {                           // if there's bytes to read from the client,
  //              char c = client.read(); // read a byte, then
  //              Serial.write(c);        // print it out to the serial monitor
  //              if (c == '\n')
  //              { // if the byte is a newline character

  //                 // if the current line is blank, you got two newline characters in a row.
  //                 // that's the end of the client HTTP request, so send a response:
  //                 if (currentLine.length() == 0)
  //                 {
  //                     // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  //                     // and a content-type so the client knows what's coming, then a blank line:
  //                     client.println("HTTP/1.1 200 OK");
  //                     client.println("Content-type:text/html");
  //                     client.println();

  //                     // the content of the HTTP response follows the header:
  //                     client.print("<p style=\"font-size:7vw;\">Click <a href=\"/H\">here</a> turn the LED on<br></p>");
  //                     client.print("<p style=\"font-size:7vw;\">Click <a href=\"/L\">here</a> turn the LED off<br></p>");

  //                     // The HTTP response ends with another blank line:
  //                     client.println();
  //                     // break out of the while loop:
  //                     break;
  //                 }
  //                 else
  //                 { // if you got a newline, then clear currentLine:
  //                     currentLine = "";
  //                 }
  //             }
  //             else if (c != '\r')
  //             {                     // if you got anything else but a carriage return character,
  //                 currentLine += c; // add it to the end of the currentLine
  //             }

  //             // Check to see if the client request was "GET /H" or "GET /L":
  //             if (currentLine.endsWith("GET /H"))
  //             {
  //                 matrix.loadFrame(frames[MATRIX_H]);
  //             }
  //             if (currentLine.endsWith("GET /L"))
  //             {
  //                 matrix.loadFrame(frames[MATRIX_L]);
  //             }
  //         }
  //     }
  //     // close the connection:
  //     client.stop();
  //     Serial.println("client disconnected");
  // }
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
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
