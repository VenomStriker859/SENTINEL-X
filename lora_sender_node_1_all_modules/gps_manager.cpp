
#include "gps_manager.h"
#include <TinyGPSPlus.h>

static constexpr int GPS_RX_PIN = 16;
static constexpr int GPS_TX_PIN = 17;
static constexpr uint32_t GPS_BAUD = 9600;

static HardwareSerial gpsSerial(1);
static TinyGPSPlus gps;
static GPSData gpsData;

void beginGPS() {
    gpsSerial.begin(
        GPS_BAUD,
        SERIAL_8N1,
        GPS_RX_PIN,
        GPS_TX_PIN
    );

    Serial.println("[GPS] UART initialized");
}

void updateGPS() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        gpsData.valid = gps.location.isValid();

        if (gpsData.valid) {
            gpsData.latitude = gps.location.lat();
            gpsData.longitude = gps.location.lng();
        }
    }

    if (gps.altitude.isValid()) {
        gpsData.altitude = gps.altitude.meters();
    }

    if (gps.satellites.isValid()) {
        gpsData.satellites = gps.satellites.value();
    }
}

GPSData getGPSData() {
    return gpsData;
}
