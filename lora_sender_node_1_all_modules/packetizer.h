#pragma once
#include "protocol.h"

class Packetizer {
public:
    static uint16_t createPackets(
        uint8_t *data,
        uint16_t dataSize,
        ImagePacket *packets,
        uint16_t messageID,
        uint8_t packetType
    );
};  