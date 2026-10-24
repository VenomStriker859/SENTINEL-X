#include "packetizer.h"
#include "crc16.h"
#include <string.h>

uint16_t Packetizer::createPackets(
    uint8_t *data,
    uint16_t dataSize,
    ImagePacket *packets,
    uint16_t messageID,
    uint8_t packetType
)
{
    uint16_t totalPackets = (dataSize + MAX_PAYLOAD_SIZE - 1) / MAX_PAYLOAD_SIZE;

    for (uint16_t i = 0; i < totalPackets; i++)
    {
        packets[i].header.version = PROTOCOL_VERSION;
        packets[i].header.type = packetType;
        packets[i].header.messageID = messageID;
        packets[i].header.packetNumber = i;
        packets[i].header.totalPackets = totalPackets;

        uint16_t remaining = dataSize - (i * MAX_PAYLOAD_SIZE);
        uint8_t length = remaining > MAX_PAYLOAD_SIZE ? MAX_PAYLOAD_SIZE : remaining;

        packets[i].header.payloadLength = length;

        memcpy(packets[i].payload, data + (i * MAX_PAYLOAD_SIZE), length);

        packets[i].crc = CRC16::calculate((uint8_t*)&packets[i].header, sizeof(PacketHeader));
        packets[i].crc ^= CRC16::calculate(packets[i].payload, length);
    }

    return totalPackets;
}