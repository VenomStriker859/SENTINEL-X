#pragma once

#include <stddef.h>
#include <stdint.h>

#include "protocol.h"
#include "lora_manager.h"


class ImageSession {

public:

    void begin(LoRaManager *lora);


    bool sendImage(
        uint8_t *data,
        size_t len,
        uint16_t messageID
    );


    bool sendText(
        const char *text,
        uint16_t messageID
    );


    bool sendAudio(
        uint8_t *data,
        size_t len,
        uint16_t messageID
    );


private:

    LoRaManager *loraRef;


    bool sendData(
        uint8_t *data,
        size_t len,
        uint16_t messageID,
        uint8_t packetType
    );
};