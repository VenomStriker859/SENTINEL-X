#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "config.h"
#include "protocol.h"
#include "lora_manager.h"
#include "image_session.h"
#include "camera_manager.h"
#include "image_debug.h"
#include "crc16.h"
#include <esp_random.h>
#include "button_manager.h"
#include "microphone_manager.h"
#include "gps_manager.h"

// SentinelNet: capture on the ESP32 camera, then send JPEG + text over LoRa.
// Laptop serial commands: TEXT:<message> followed by CAPTURE.

LoRaManager loraManager;
ImageSession imageSession;
static constexpr size_t MAX_IMAGE_BYTES = MAX_IMAGE_PACKETS * MAX_PAYLOAD_SIZE;
static uint16_t messageCounter = 0;
static bool loraReady = false;
static bool cameraReady = false;
static String pendingText;
static uint8_t audioBuf[MAX_IMAGE_PACKETS * MAX_PAYLOAD_SIZE];
static uint16_t audioCounter = 1;

void initializeLoRa() {
  Serial.println("[LORA] Initializing radio...");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  loraReady = loraManager.begin(LORA_FREQUENCY, LORA_SS, LORA_RST, LORA_DIO0);
  if (!loraReady) {
    Serial.println("[LORA][ERROR] Initialization failed.");
    return; 
  }
  imageSession.begin(&loraManager);
  loraManager.startReceive();
  Serial.println("[LORA] Initialization successful.");
}

void sendTextOnly(const String &message) {
  if (!loraReady || message.length() == 0) {
    Serial.println("[TEXT][ERROR] LoRa not ready or message empty.");
    return;
  }
  uint16_t id = TEXT_MESSAGE_ID_OFFSET + messageCounter;
  bool ok = imageSession.sendText(message.c_str(), id);
  Serial.println(ok ? "[TEXT] Transmission SUCCESS." : "[TEXT] Transmission FAILED.");
  messageCounter++;
  Serial.println("WAITING_FOR_MESSAGE");
}

void captureAndTransmit() {
  if (!loraReady) {
    Serial.println("[ERROR] LoRa is not initialized.");
    pendingText = "";
    return;
  }
  if (pendingText.length() == 0) {
    Serial.println("[CAMERA][ERROR] Send TEXT:<message> before CAPTURE.");
    return;
  }
  if (!cameraReady) {
    Serial.println("[CAMERA][ERROR] Camera initialization failed; check camera pin mapping and ribbon connection.");
    pendingText = "";
    return;
  }

  uint8_t *jpeg = nullptr;
  size_t jpegLength = 0;
  Serial.println("[CAMERA] Capturing image from ESP32 camera...");
  if (!captureImage(&jpeg, &jpegLength) || jpeg == nullptr || jpegLength == 0) {
    Serial.println("[CAMERA][ERROR] Capture failed.");
    releaseImage();
    pendingText = "";
    return;
  }

  Serial.print("[CAMERA] Captured JPEG bytes: ");
  Serial.println(jpegLength);
  if (jpegLength > MAX_IMAGE_BYTES) {
    Serial.print("[CAMERA][ERROR] JPEG too large. Maximum bytes: ");
    Serial.println(MAX_IMAGE_BYTES);
    releaseImage();
    pendingText = "";
    return;
  }

  // Send a copy of the captured JPEG to the laptop over USB (IMG_START / base64 / IMG_END),
  // so the laptop script can save the "sent" image before it goes over LoRa.
  Serial.println("[CAMERA] Sending a copy of the captured image to the laptop...");
  printImageOverSerial(jpeg, jpegLength);

  uint16_t frameId = messageCounter;
  Serial.println("[LORA] Sending captured image...");
  bool imageOk = imageSession.sendImage(jpeg, jpegLength, frameId);
  releaseImage();
  if (!imageOk) {
    Serial.println("[LORA][ERROR] Image transmission failed; text not sent.");
    pendingText = "";
    messageCounter++;
    Serial.println("WAITING_FOR_MESSAGE");
    return;
  }
  Serial.println("[LORA] Image transmission successful.");

  Serial.println("[LORA] Sending associated text...");
  uint16_t textId = TEXT_MESSAGE_ID_OFFSET + frameId;
  bool textOk = imageSession.sendText(pendingText.c_str(), textId);
  Serial.println(textOk ? "[LORA] Text transmission successful." : "[LORA][ERROR] Text transmission failed.");
  pendingText = "";
  messageCounter++;
  Serial.println("[TRANSFER] Finished.");
  Serial.println("WAITING_FOR_MESSAGE");
}

// Audio comes from the LAPTOP microphone. The laptop script sends:
//   AUDIO_BIN:<length>:<crc16>\n   then <length> raw bytes (IMA ADPCM audio message)
// We answer AUDIO_READY, read the bytes, check the CRC and transmit them over LoRa.
void receiveAndTransmitAudio(uint32_t len, uint16_t crcExpected) {
  if (!loraReady) {
    Serial.println("[AUDIO][ERROR] LoRa is not initialized.");
    return;
  }
  if (len == 0 || len > sizeof(audioBuf)) {
    Serial.print("[AUDIO][ERROR] Bad audio length: ");
    Serial.println(len);
    return;
  }

  Serial.println("AUDIO_READY");
  Serial.setTimeout(15000);
  size_t got = Serial.readBytes((char *)audioBuf, len);
  Serial.setTimeout(1000);
  if (got != len) {
    Serial.print("[AUDIO][ERROR] Only received ");
    Serial.print(got);
    Serial.print(" of ");
    Serial.print(len);
    Serial.println(" bytes from the laptop.");
    Serial.println("WAITING_FOR_MESSAGE");
    return;
  }
  uint16_t crc = CRC16::calculate(audioBuf, len);
  if (crc != crcExpected) {
    Serial.println("[AUDIO][ERROR] CRC mismatch on USB transfer; audio discarded.");
    Serial.println("WAITING_FOR_MESSAGE");
    return;
  }
  Serial.print("[AUDIO] Received ");
  Serial.print(len);
  Serial.println(" bytes from the laptop (CRC OK).");

  uint16_t id = AUDIO_MESSAGE_ID_OFFSET + audioCounter;
  Serial.println("[LORA] Sending audio...");
  bool ok = imageSession.sendAudio(audioBuf, len, id);
  audioCounter++;
  Serial.println(ok ? "[LORA] Audio transmission successful." : "[LORA][ERROR] Audio transmission failed.");
  Serial.println("WAITING_FOR_MESSAGE");
}


void handleButtonActions() {
  static int previousState = -1;

  int state = digitalRead(38);

  // Print only when the GPIO state changes.
  if (state != previousState) {
    Serial.print("[BUTTON DEBUG] GPIO 38 = ");
    Serial.println(state == LOW ? "LOW (pressed)" : "HIGH (released)");
    previousState = state;
  }

  ButtonAction action = pollSosButton();

  if (action == BUTTON_SINGLE_PRESS) {
    Serial.println("[BUTTON] Single press: image capture");

    if (pendingText.length() == 0) {
      pendingText = "Button captured image";
    }

    captureAndTransmit();
  }
  else if (action == BUTTON_DOUBLE_PRESS) {
    Serial.println("[BUTTON] Double press: request audio capture");
    Serial.println("AUDIO_CAPTURE_REQUEST");
  }
}

void recordMicrophoneToLaptop() {
  constexpr uint32_t SAMPLE_RATE = 8000;
  constexpr uint32_t DURATION_SECONDS = 10;
  constexpr size_t SAMPLE_COUNT = SAMPLE_RATE * DURATION_SECONDS;
  constexpr size_t AUDIO_BYTES = SAMPLE_COUNT * sizeof(int16_t);

  Serial.println("[MIC] Recording 10 seconds...");

  // Allocate the recording buffer in heap memory.
  uint16_t* samples = (uint16_t*)malloc(AUDIO_BYTES);

  if (samples == nullptr) {
    Serial.println("[MIC][ERROR] Not enough memory for recording.");
    return;
  }

  microphoneBegin();

  bool captured = microphoneCaptureSamples(
    samples,
    SAMPLE_COUNT,
    SAMPLE_RATE
  );

  if (!captured) {
    free(samples);
    Serial.println("[MIC][ERROR] Sampling failed.");
    return;
  }

  // Convert 12-bit ADC readings to signed 16-bit PCM.
  
  // Calculate the actual DC offset from the recording.
  uint32_t sum = 0;
  for (size_t i = 0; i < SAMPLE_COUNT; i++) {
    sum += samples[i];
  }
  int32_t dcOffset = sum / SAMPLE_COUNT;

  // Remove DC offset and amplify the voice signal.
  int16_t* pcm = (int16_t*)samples;

  for (size_t i = 0; i < SAMPLE_COUNT; i++) {
    int32_t centered = (int32_t)samples[i] - dcOffset;
    int32_t amplified = centered * 24;

    if (amplified > 32767) amplified = 32767;
    if (amplified < -32768) amplified = -32768;

    pcm[i] = (int16_t)amplified;
  }


  // IMPORTANT: From this header onward, send only binary audio
  // until all AUDIO_BYTES have been transmitted.
  Serial.printf("MIC_AUDIO_START:%u\n", (unsigned int)AUDIO_BYTES);
  Serial.flush();

  Serial.write((const uint8_t*)pcm, AUDIO_BYTES);
  Serial.flush();

  free(samples);

  // Do not print another status line here: the Python receiver
  // expects the exact number of binary bytes after the header.
}

void setup() {
  Serial.setRxBufferSize(4096);   // audio bytes arrive from the laptop in one burst
  Serial.begin(115200);
  beginGPS();
  beginSosButton();
  delay(1500);
  Serial.println();
  Serial.println("========================================");
  Serial.println(" SENTINELNET ESP32-S3 CAMERA SENDER");
  Serial.println(" Camera image + text over LoRa");
  Serial.println("========================================");

  // Camera first, so a camera problem is visible before LoRa starts.
  Serial.println("[BOOT] Camera initialization skipped for microphone test.");
  cameraReady = false;

  Serial.println("[BOOT] Step 2: LoRa init...");
  initializeLoRa();

  audioCounter = 1 + (esp_random() % 900);
  Serial.println("[BOOT] Ready.");
  Serial.println("Commands: TEXT:<message> then CAPTURE");
  Serial.println("          TEST_TEXT:<message> for text-only test");
  Serial.println("          AUDIO_BIN:<len>:<crc> + bytes for audio from the laptop microphone");
  Serial.println("WAITING_FOR_MESSAGE");
}



void loop() {
  // Check the physical button first.
  handleButtonActions();
  updateGPS();

  static unsigned long lastGPSPrint = 0;

  if (millis() - lastGPSPrint >= 2000) {
      lastGPSPrint = millis();

      GPSData data = getGPSData();

      if (data.valid) {
          Serial.printf(
              "[GPS] Lat: %.6f | Lon: %.6f | Satellites: %lu\n",
              data.latitude,
              data.longitude,
              (unsigned long)data.satellites
          );
      }  else {
          Serial.println("[GPS] Waiting for satellite fix...");
      }
  }

  // Process commands arriving from the laptop.
  if (!Serial.available()) {
    return;
  }

  String line = Serial.readStringUntil('\n');
  line.trim();

  if (line.length() == 0) {
    return;
  }

  if (line == "MIC_TEST") {
    recordMicrophoneToLaptop();
    return;
  }

  if (line.startsWith("TEXT:")) {
    pendingText = line.substring(5);

    if (pendingText.length() == 0) {
      Serial.println("[USB][ERROR] Text message is empty.");
      return;
    }

    Serial.print("[USB] Text stored for next camera image: ");
    Serial.println(pendingText);
    Serial.println("[USB] Send CAPTURE to take a picture and transmit it.");
    return;
  }

  if (line == "CAPTURE") {
    captureAndTransmit();
    return;
  }

  if (line.startsWith("AUDIO_BIN:")) {
    int p = line.indexOf(':', 10);

    if (p < 0) {
      Serial.println("[AUDIO][ERROR] Bad AUDIO_BIN command.");
      return;
    }

    uint32_t len = (uint32_t)line.substring(10, p).toInt();
    uint16_t crc = (uint16_t)line.substring(p + 1).toInt();

    receiveAndTransmitAudio(len, crc);
    return;
  }

  if (line.startsWith("TEST_TEXT:")) {
    sendTextOnly(line.substring(10));
    return;
  }

  // Preserve backwards-compatible text-only testing.
  sendTextOnly(line);
}

