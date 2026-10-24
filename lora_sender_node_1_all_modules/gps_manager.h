
#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H

#include <Arduino.h>

struct GPSData {
    double latitude = 0.0;
    double longitude = 0.0;
    double altitude = 0.0;
    uint32_t satellites = 0;
    bool valid = false;
};

void beginGPS();
void updateGPS();
GPSData getGPSData();

#endif
