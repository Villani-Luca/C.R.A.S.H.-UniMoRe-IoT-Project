#include "sensors.h"


// non so se é una buona idea avere uno stato globale separato da main
// inoltre questo approccio lega l'implementazione a un numero fisso di accelerometri
// TODO: da rivedere

#if SENSOR_ACCEL_ENABLED
Adafruit_ADXL345_Unified accel;
#endif

#if SENSOR_GPS_ENABLED
TinyGPSPlus gps;
SoftwareSerial gps_serial(SENSOR_GPS_RX_PIN,SENSOR_GPS_TX_PIN);
#endif


// ##### SETUP #####
void setup_accel(){
    #if SENSOR_ACCEL_ENABLED
    if(!accel.begin()){
        Serial.println("No valid ADXL345 sensor found");
        while(1);
    }
    #endif
}

void setup_gps(){
    #if SENSOR_GPS_ENABLED
    gps_serial.begin(SENSOR_GPS_BAUD_RATE);
    if(!true){
        Serial.println("Error during setup of gps sensor");
        while(1);
    }
    #endif
}

void setup_sensors(){
    #if SENSOR_ACCEL_ENABLED
    setup_accel();
    #endif

    #if SENSOR_GPS_ENABLED
    setup_gps();
    #endif
}

// ##### USE #####

void accel_read(AccelReading& reading) {
    #if SENSOR_ACCEL_ENABLED
    sensors_event_t evt;
    accel.getEvent(&evt);

    reading.x = evt.acceleration.x;
    reading.y = evt.acceleration.y;
    reading.z = evt.acceleration.z;
    #else
    reading.x = 0;
    reading.y = 0;
    reading.z = 0;
    #endif
}

void gps_read(Position& position){
    #if SENSOR_GPS_ENABLED

    while (gps_serial.available() > 0){
        // get the byte data from the GPS
        byte gpsData = gps_serial.read();
        Serial.write(gpsData);
    }

    /*
    while (gps_serial.available() > 0){
        Serial.println("boh");
        Serial.print(char(gps_serial.read()));
        // get the byte data from the GPS
        // gps.encode(gps_serial.read());
        // if(gps.location.isUpdated()){
        //     position.latitude = gps.location.lat();
        //     position.longitude = gps.location.lng();

        //     #ifdef ENABLE_DEBUG
        //     Serial.print("Satellites ");
        //     Serial.println(gps.satellites.value());
        //     #endif
        // }
    }
    */
    #else
        position.latitude = 0;
        position.longitude = 0;
    #endif
}


