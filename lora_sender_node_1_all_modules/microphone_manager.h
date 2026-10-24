
#pragma once

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

// MAX9814 analog output pin
#define MICROPHONE_PIN 4

// Initialize the microphone ADC
void microphoneBegin();

// Read one raw ADC sample (0-4095)
uint16_t microphoneReadSample();

// Capture a block of raw audio samples
bool microphoneCaptureSamples(
    uint16_t* buffer,
    size_t sampleCount,
    uint32_t sampleRateHz = 8000
);
