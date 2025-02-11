#include "sensors.h"


// non so se é una buona idea avere uno stato globale separato da main
// inoltre questo approccio lega l'implementazione a un numero fisso di accelerometri
// TODO: da rivedere

#if SENSOR_ACCEL_ENABLED
Adafruit_ADXL345_Unified accel;
#endif


#if SENSOR_GPS_ENABLED

// GPS usa Serial1 pin 0 e 1
// configurazione:
//  - LETTURA: MODULO TX => ARDUINO RX
//  - SCRITTURA/COMANDI: MODULO RX => TX TRAMITE LEVEL SHIFT DOWN 
//    per evitare di friggere il modulo
TinyGPSPlus gps;
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
    Serial1.begin(9600);
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
    #endif
}

int gps_read(Position& position){
    #if SENSOR_GPS_ENABLED
    while (Serial1.available() > 0){
        // get the byte data from the GPS
        gps.encode(Serial1.read());
        if(gps.location.isUpdated()){
            position.latitude = gps.location.lat();
            position.longitude = gps.location.lng();

            #ifdef ENABLE_DEBUG
            Serial.print("Satellites ");
            Serial.println(gps.satellites.value());
            #endif

            return 1;
        }
        return 0;
    }
    #endif

    return 0;
}


