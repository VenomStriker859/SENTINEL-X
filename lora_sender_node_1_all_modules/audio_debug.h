#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <stddef.h>

void printAudioOverSerial(
    const uint8_t *buf,
    size_t len
);