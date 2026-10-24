
#pragma once

// =====================================================
// SELECT YOUR BOARD (uncomment exactly ONE)
// =====================================================
#define BOARD_GENERIC_S3_CAM      // Freenove / generic "ESP32-S3-WROOM CAM" with OV2640
//#define BOARD_XIAO_S3_SENSE     // Seeed XIAO ESP32S3 Sense

// =====================================================
// Button Configuration
// =====================================================
#define BUTTON_PIN 38

// =====================================================
// LoRa Configuration
// =====================================================
#define LORA_FREQUENCY 433E6

// LoRa wiring. These pins are NOT used by the camera, PSRAM, flash or USB on the S3.
#if defined(BOARD_GENERIC_S3_CAM)
  // Camera uses GPIO 4-18 (except 14). Do not use 19/20 (USB) or 26-37 (flash/PSRAM).
  // GPIO 48 is the onboard LED, so MISO uses GPIO 1. DIO0 is not used by this code
  // (the sender polls the radio), so leave the module's DIO0 pin unconnected.
  #define LORA_SCK   47
  #define LORA_MISO  1
  #define LORA_MOSI  21
  #define LORA_SS    14
  #define LORA_RST   2
  #define LORA_DIO0  -1
#elif defined(BOARD_XIAO_S3_SENSE)
  // XIAO header pins: D8=7 (SCK), D9=8 (MISO), D10=9 (MOSI), D1=2 (CS), D2=3 (RST), D3=4 (DIO0)
  #define LORA_SCK   7
  #define LORA_MISO  8
  #define LORA_MOSI  9
  #define LORA_SS    2
  #define LORA_RST   3
  #define LORA_DIO0  4
#else
  #error "Select a board in config.h"
#endif

// =====================================================
// Transmission Configuration
// =====================================================
#define MAX_RETRIES       5
#define ACK_TIMEOUT_MS    3000
#define BATCH_GAP_MS      500
#define IMAGE_SESSION_TIMEOUT_MS 15000
