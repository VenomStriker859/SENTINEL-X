
#include "microphone_manager.h"

void microphoneBegin() {
    pinMode(MICROPHONE_PIN, INPUT);
    analogReadResolution(12);

    Serial.println("[MIC] MAX9814 initialized");
    Serial.printf("[MIC] Input GPIO: %d\n", MICROPHONE_PIN);
}

uint16_t microphoneReadSample() {
    return static_cast<uint16_t>(
        analogRead(MICROPHONE_PIN)
    );
}

bool microphoneCaptureSamples(
    uint16_t* buffer,
    size_t sampleCount,
    uint32_t sampleRateHz
) {
    if (buffer == nullptr ||
        sampleCount == 0 ||
        sampleRateHz == 0 ||
        sampleRateHz > 20000) {
        return false;
    }

    const uint32_t intervalUs = 1000000UL / sampleRateHz;
    uint32_t nextSampleTime = micros();

    for (size_t i = 0; i < sampleCount; i++) {
        while ((int32_t)(micros() - nextSampleTime) < 0) {
            // Wait for the next sampling interval
        }

        buffer[i] = microphoneReadSample();
        nextSampleTime += intervalUs;
    }

    return true;
}
