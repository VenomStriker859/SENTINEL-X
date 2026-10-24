#include "image_session.h"

#include "packetizer.h"
#include "crc16.h"
#include "config.h"

#include <Arduino.h>
#include <string.h>

static ImagePacket packetBuffer[MAX_IMAGE_PACKETS];


void ImageSession::begin(LoRaManager *lora)
{
    loraRef = lora;
}


// =====================================================
// IMAGE
// =====================================================

bool ImageSession::sendImage(
    uint8_t *data,
    size_t len,
    uint16_t messageID
)
{
    return sendData(
        data,
        len,
        messageID,
        IMAGE_PACKET
    );
}


// =====================================================
// TEXT
// =====================================================

bool ImageSession::sendText(
    const char *text,
    uint16_t messageID
)
{
    return sendData(
        (uint8_t*)text,
        strlen(text),
        messageID,
        TEXT_PACKET
    );
}


// =====================================================
// AUDIO
// =====================================================

bool ImageSession::sendAudio(
    uint8_t *data,
    size_t len,
    uint16_t messageID
)
{
    return sendData(
        data,
        len,
        messageID,
        AUDIO_PACKET
    );
}


// =====================================================
// GENERIC DATA TRANSMISSION
// =====================================================

bool ImageSession::sendData(
    uint8_t *data,
    size_t len,
    uint16_t messageID,
    uint8_t packetType
)
{
    if (data == nullptr || len == 0)
    {
        Serial.println("[LORA] ERROR: Empty data.");
        return false;
    }


    const size_t MAX_DATA_SIZE =
        (size_t)MAX_IMAGE_PACKETS *
        MAX_PAYLOAD_SIZE;


    if (len > MAX_DATA_SIZE)
    {
        Serial.print("[LORA] ERROR: Data too large. ");
        Serial.print(len);
        Serial.print(" bytes > ");
        Serial.print(MAX_DATA_SIZE);
        Serial.println(" bytes maximum.");

        return false;
    }


    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("[LORA] TRANSMISSION START");
    Serial.println("----------------------------------------");


    Serial.print("[LORA] Packet type: ");
    Serial.println(packetType);


    Serial.print("[LORA] Message ID: ");
    Serial.println(messageID);


    Serial.print("[LORA] Data size: ");
    Serial.print(len);
    Serial.println(" bytes");


    Serial.print("[LORA] Payload/packet: ");
    Serial.print(MAX_PAYLOAD_SIZE);
    Serial.println(" bytes");


    uint16_t totalPackets =
        Packetizer::createPackets(
            data,
            (uint16_t)len,
            packetBuffer,
            messageID,
            packetType
        );


    if (totalPackets == 0)
    {
        Serial.println("[LORA] ERROR: No packets created.");
        return false;
    }


    Serial.print("[LORA] Total packets: ");
    Serial.println(totalPackets);


    Serial.print("[LORA] Packet size on air: ");
    Serial.print(sizeof(ImagePacket));
    Serial.println(" bytes");


    Serial.print("[LORA] Theoretical payload: ");
    Serial.print(
        (uint32_t)totalPackets *
        MAX_PAYLOAD_SIZE
    );
    Serial.println(" bytes");


    bool acked[MAX_IMAGE_PACKETS];

    for (uint16_t i = 0; i < totalPackets; i++)
    {
        acked[i] = false;
    }


    uint8_t attempt = 0;
    bool complete = false;


    unsigned long transmissionStart =
        millis();


    // =================================================
    // RETRY LOOP
    // =================================================

    while (
        attempt < MAX_RETRIES &&
        !complete
    )
    {
        attempt++;


        Serial.println();
        Serial.println("----------------------------------------");

        Serial.print("[LORA] ATTEMPT ");
        Serial.print(attempt);
        Serial.print("/");
        Serial.println(MAX_RETRIES);

        Serial.println("----------------------------------------");


        uint16_t sentThisRound = 0;


        unsigned long attemptStart =
            millis();


        // ---------------------------------------------
        // SEND PACKETS
        // ---------------------------------------------

        for (
            uint16_t i = 0;
            i < totalPackets;
            i++
        )
        {
            if (!acked[i])
            {
                Serial.print("[LORA] TX packet ");
                Serial.print(i + 1);
                Serial.print("/");
                Serial.print(totalPackets);

                Serial.print(" | ");

                Serial.print(sizeof(ImagePacket));
                Serial.println(" bytes");


                loraRef->send(
                    (uint8_t*)&packetBuffer[i],
                    sizeof(ImagePacket)
                );


                sentThisRound++;


                delay(50);
            }
        }


        unsigned long sendDuration =
            millis() - attemptStart;


        Serial.println();

        Serial.print("[LORA] Packets sent this attempt: ");
        Serial.println(sentThisRound);


        Serial.print("[LORA] Send duration: ");
        Serial.print(sendDuration);
        Serial.println(" ms");


        Serial.print("[LORA] Waiting for ACK (");
        Serial.print(ACK_TIMEOUT_MS);
        Serial.println(" ms timeout)...");


        // ---------------------------------------------
        // WAIT FOR ACK
        // ---------------------------------------------

        unsigned long waitStart =
            millis();


        bool ackReceived = false;


        while (
            millis() - waitStart <
            ACK_TIMEOUT_MS
        )
        {
            if (loraRef->available())
            {
                uint8_t buffer[
                    LORA_MAX_PACKET_SIZE
                ];


                size_t rlen =
                    loraRef->readPacket(
                        buffer
                    );


                Serial.print("[LORA] RX packet during ACK wait: ");
                Serial.print(rlen);
                Serial.println(" bytes");


                if (
                    rlen ==
                    sizeof(ImageAckPacket)
                )
                {
                    ImageAckPacket *ack =
                        (ImageAckPacket*)buffer;


                    uint16_t crcCheck =
                        CRC16::calculate(
                            (uint8_t*)ack,
                            sizeof(ImageAckPacket) -
                            sizeof(uint16_t)
                        );


                    if (
                        crcCheck == ack->crc &&
                        ack->type == ACK_PACKET &&
                        ack->messageID == messageID
                    )
                    {
                        ackReceived = true;


                        complete = true;


                        uint16_t missingCount = 0;


                        for (
                            uint16_t i = 0;
                            i < totalPackets &&
                            i < MAX_IMAGE_PACKETS;
                            i++
                        )
                        {
                            bool missing =
                                ack->missingBitmap[
                                    i / 8
                                ] &
                                (
                                    1 <<
                                    (i % 8)
                                );


                            if (missing)
                            {
                                acked[i] = false;
                                complete = false;
                                missingCount++;
                            }
                            else
                            {
                                acked[i] = true;
                            }
                        }


                        Serial.println();
                        Serial.println("[LORA] ACK RECEIVED");


                        Serial.print("[LORA] Missing packets: ");
                        Serial.println(missingCount);


                        if (complete)
                        {
                            Serial.println(
                                "[LORA] ACK confirms ALL packets."
                            );
                        }
                        else
                        {
                            Serial.println(
                                "[LORA] Receiver reports missing packets."
                            );
                        }


                        break;
                    }
                    else
                    {
                        Serial.println(
                            "[LORA] ACK rejected: CRC/type/message mismatch."
                        );
                    }
                }
            }
        }


        if (!ackReceived)
        {
            Serial.println();
            Serial.println(
                "[LORA] WARNING: No ACK received."
            );
        }
    }


    // =================================================
    // FINAL RESULT
    // =================================================

    unsigned long totalDuration =
        millis() - transmissionStart;


    Serial.println();
    Serial.println("========================================");


    if (!complete)
    {
        Serial.println(
            "[LORA] TRANSMISSION FAILED"
        );


        Serial.print("[LORA] Total time: ");
        Serial.print(totalDuration);
        Serial.println(" ms");


        Serial.println("================================");

        return false;
    }


    Serial.println(
        "[LORA] TRANSMISSION SUCCESS"
    );


    Serial.print("[LORA] Total packets: ");
    Serial.println(totalPackets);


    Serial.print("[LORA] Total time: ");
    Serial.print(totalDuration);
    Serial.println(" ms");


    Serial.println("================================");


    return true;
}