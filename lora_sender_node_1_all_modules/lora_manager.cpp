#include "lora_manager.h"


LoRaManager::LoRaManager()
{
    initialized = false;
    lastPacketSize = 0;
}


bool LoRaManager::begin(long frequency, int csPin, int resetPin, int irqPin)
{
    LoRa.setPins(csPin, resetPin, irqPin);

    if (!LoRa.begin(frequency))
    {
        Serial.println("LoRa init failed");
        initialized = false;
        return false;
    }

    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(250E3);
    LoRa.setCodingRate4(5);
    LoRa.setTxPower(17);

    // Start in receive mode
    LoRa.receive();

    Serial.println("LoRa initialized");

    initialized = true;
    return true;
}


bool LoRaManager::send(const uint8_t* data, size_t len)
{
    if (!initialized)
    {
        return false;
    }

    if (len > LORA_MAX_PACKET_SIZE)
    {
        Serial.println("LoRa send failed: packet too large");
        return false;
    }

    // Make sure we are not currently receiving
    LoRa.idle();

    LoRa.beginPacket();
    LoRa.write(data, len);

    int result = LoRa.endPacket();

    // CRITICAL:
    // After transmission, immediately return to RX mode.
    LoRa.receive();

    return result == 1;
}


bool LoRaManager::available()
{
    if (!initialized)
    {
        return false;
    }

    lastPacketSize = LoRa.parsePacket();

    return lastPacketSize > 0;
}


size_t LoRaManager::readPacket(uint8_t* buffer)
{
    if (lastPacketSize <= 0)
    {
        return 0;
    }

    size_t bytesRead = 0;

    while (LoRa.available() &&
           bytesRead < LORA_MAX_PACKET_SIZE)
    {
        buffer[bytesRead++] = (uint8_t)LoRa.read();
    }

    lastPacketSize = 0;

    // Continue listening after reading the packet
    LoRa.receive();

    return bytesRead;
}


void LoRaManager::startReceive()
{
    if (!initialized)
    {
        return;
    }

    LoRa.receive();
}


int LoRaManager::getLastRssi()
{
    return LoRa.packetRssi();
}


float LoRaManager::getLastSnr()
{
    return LoRa.packetSnr();
}