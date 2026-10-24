#ifndef LORA_MANAGER_H
#define LORA_MANAGER_H

#include <Arduino.h>
#include <LoRa.h>

#define LORA_MAX_PACKET_SIZE 250

class LoRaManager
{
public:

    LoRaManager();

    bool begin(long frequency, int csPin, int resetPin, int irqPin);

    bool send(const uint8_t* data, size_t len);

    bool available();

    size_t readPacket(uint8_t* buffer);

    int getLastRssi();

    float getLastSnr();

    // Explicitly switch radio to receive mode
    void startReceive();

private:

    bool initialized;
    int lastPacketSize;
};

#endif