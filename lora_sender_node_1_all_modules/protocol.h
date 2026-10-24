#pragma once

#include <stdint.h>

#define PROTOCOL_VERSION 1

#define MAX_PAYLOAD_SIZE 200

#define LORA_MAX_PACKET_SIZE 255

#define TEXT_MESSAGE_ID_OFFSET 10000
#define GPS_MESSAGE_ID_OFFSET 20000
#define AUDIO_MESSAGE_ID_OFFSET 30000

// 128 packets × 200 bytes = 25.6 KB maximum payload
#define MAX_IMAGE_PACKETS 128

#define ACK_BITMAP_BYTES (MAX_IMAGE_PACKETS / 8)


enum PacketType : uint8_t {

    IMAGE_PACKET = 1,

    ACK_PACKET = 2,

    TEXT_PACKET = 3,

    GPS_PACKET = 4,

    AUDIO_PACKET = 5
};


#pragma pack(push, 1)


struct PacketHeader {

    uint8_t version;

    uint8_t type;

    uint16_t messageID;

    uint16_t packetNumber;

    uint16_t totalPackets;

    uint8_t payloadLength;
};


struct ImagePacket {

    PacketHeader header;

    uint8_t payload[MAX_PAYLOAD_SIZE];

    uint16_t crc;
};


struct ImageAckPacket {

    uint8_t type;

    uint16_t messageID;

    uint16_t totalPackets;

    uint8_t missingBitmap[ACK_BITMAP_BYTES];

    uint16_t crc;
};


#pragma pack(pop)