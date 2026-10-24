// crc16.h
#pragma once
#include <stdint.h>
#include <stddef.h>

class CRC16 {
public:
    static uint16_t calculate(const uint8_t *data, size_t len);
};