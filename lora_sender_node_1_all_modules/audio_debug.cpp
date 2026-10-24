#include "audio_debug.h"

#include <Arduino.h>
#include "mbedtls/base64.h"

#include <stdlib.h>
#include <string.h>


void printAudioOverSerial(
    const uint8_t *buf,
    size_t len
) {

    size_t requiredLen = 0;


    int result =
        mbedtls_base64_encode(
            nullptr,
            0,
            &requiredLen,
            buf,
            len
        );


    if (
        result != 0 &&
        result !=
        MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL
    ) {

        Serial.println(
            "Audio Base64 size calculation failed."
        );

        return;
    }


    uint8_t *encodedBuf =
        (uint8_t*)malloc(
            requiredLen + 1
        );


    if (!encodedBuf) {

        Serial.println(
            "Failed to allocate audio Base64 buffer."
        );

        return;
    }


    size_t encodedLen = 0;


    result =
        mbedtls_base64_encode(
            encodedBuf,
            requiredLen,
            &encodedLen,
            buf,
            len
        );


    if (result != 0) {

        Serial.println(
            "Audio Base64 encoding failed."
        );

        free(encodedBuf);

        return;
    }


    encodedBuf[encodedLen] = '\0';


    Serial.println("AUDIO_START");


    const size_t CHUNK = 120;


    for (
        size_t i = 0;
        i < encodedLen;
        i += CHUNK
    ) {

        size_t chunkLen =
            (
                encodedLen - i < CHUNK
            )
            ?
            (encodedLen - i)
            :
            CHUNK;


        char lineBuf[
            CHUNK + 1
        ];


        memcpy(
            lineBuf,
            encodedBuf + i,
            chunkLen
        );


        lineBuf[chunkLen] = '\0';


        Serial.println(
            lineBuf
        );
    }


    Serial.println("AUDIO_END");


    free(encodedBuf);
}