// crc16.cpp
#include "crc16.h"

uint16_t CRC16::calculate(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++){
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t b = 0; b < 8; b++){
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : (crc << 1);
        }
    }
    return crc;
}