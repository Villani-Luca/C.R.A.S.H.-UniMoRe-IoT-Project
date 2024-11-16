#include "sensors.h"

// non so se é una buona idea avere uno stato globale separato da main
// inoltre questo approccio lega l'implementazione a un numero fisso di accelerometri
// TODO: da rivedere

#if SENSOR_ACCEL_ENABLED
Adafruit_ADXL345_Unified accel;
#endif

// ##### SETUP #####
void setup_accel(){
    if(!accel.begin()){
        Serial.println("No valid ADXL345 sensor found");
        while(1);
    }
}

void setup_sensors(){
    #if SENSOR_ACCEL_ENABLED
    setup_accel();
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


