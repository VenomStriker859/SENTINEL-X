#include "image_debug.h"
#include <Arduino.h>
#include "mbedtls/base64.h"
#include <string.h>
#include <stdlib.h>

void printImageOverSerial(const uint8_t *buf, size_t len)
{
    size_t requiredLen = 0;

    int result = mbedtls_base64_encode(
        nullptr,
        0,
        &requiredLen,
        buf,
        len
    );

    if (result != 0 && result != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        Serial.println("Base64 size calculation failed");
        return;
    }

    uint8_t *encodedBuf =
        (uint8_t*)malloc(requiredLen + 1);

    if (!encodedBuf) {
        Serial.println(
            "Failed to allocate base64 buffer for image dump"
        );
        return;
    }

    size_t encodedLen = 0;

    result = mbedtls_base64_encode(
        encodedBuf,
        requiredLen,
        &encodedLen,
        buf,
        len
    );

    if (result != 0) {
        Serial.println("Base64 encoding failed");
        free(encodedBuf);
        return;
    }

    encodedBuf[encodedLen] = '\0';

    Serial.println("IMG_START");

    const size_t CHUNK = 120;

    for (size_t i = 0; i < encodedLen; i += CHUNK) {

        size_t chunkLen =
            (encodedLen - i < CHUNK)
            ? (encodedLen - i)
            : CHUNK;

        char lineBuf[CHUNK + 1];

        memcpy(
            lineBuf,
            encodedBuf + i,
            chunkLen
        );

        lineBuf[chunkLen] = '\0';

        Serial.println(lineBuf);
    }

    Serial.println("IMG_END");

    free(encodedBuf);
}   