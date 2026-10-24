#include "camera_manager.h"
#include "config.h"
#include "esp_camera.h"
#include <Arduino.h>

// ---------------------------------------------------------------
// Camera pin maps (ESP32-S3). Chosen by the board in config.h.
// ---------------------------------------------------------------
#if defined(BOARD_GENERIC_S3_CAM)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    15
  #define SIOD_GPIO_NUM     4
  #define SIOC_GPIO_NUM     5
  #define Y9_GPIO_NUM      16
  #define Y8_GPIO_NUM      17
  #define Y7_GPIO_NUM      18
  #define Y6_GPIO_NUM      12
  #define Y5_GPIO_NUM      10
  #define Y4_GPIO_NUM       8
  #define Y3_GPIO_NUM       9
  #define Y2_GPIO_NUM      11
  #define VSYNC_GPIO_NUM    6
  #define HREF_GPIO_NUM     7
  #define PCLK_GPIO_NUM    13
#elif defined(BOARD_XIAO_S3_SENSE)
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    10
  #define SIOD_GPIO_NUM    40
  #define SIOC_GPIO_NUM    39
  #define Y9_GPIO_NUM      48
  #define Y8_GPIO_NUM      11
  #define Y7_GPIO_NUM      12
  #define Y6_GPIO_NUM      14
  #define Y5_GPIO_NUM      16
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM      17
  #define Y2_GPIO_NUM      15
  #define VSYNC_GPIO_NUM   38
  #define HREF_GPIO_NUM    47
  #define PCLK_GPIO_NUM    13
#endif

#include "img_converters.h"   // frame2jpg() for sensors without hardware JPEG

// Image size and quality. Raise only after the link works end to end
// (QVGA 320x240 needs PSRAM enabled in Tools > PSRAM when the sensor has no JPEG).
#define CAM_FRAME_SIZE      FRAMESIZE_QQVGA   // 160x120
#define HW_JPEG_QUALITY     20                // hardware JPEG: higher number = smaller file
#define SW_JPEG_QUALITY     40                // software JPEG: higher number = better/bigger file

static camera_fb_t *currentFb = nullptr;
static uint8_t *swJpegBuf = nullptr;
static bool softwareJpeg = false;

static bool initWith(pixformat_t format, esp_err_t *errOut){
    camera_config_t config = {};   // zero every field: newer cores add fields, garbage values crash

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.sccb_i2c_port = -1;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = format;
    config.frame_size = CAM_FRAME_SIZE;
    config.jpeg_quality = HW_JPEG_QUALITY;
    config.fb_count = 1;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

    if (psramFound()) {
        config.fb_location = CAMERA_FB_IN_PSRAM;
    } else {
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    *errOut = esp_camera_init(&config);
    return *errOut == ESP_OK;
}

static void printSensor(){
    sensor_t *s = esp_camera_sensor_get();
    if (s) Serial.printf("[CAMERA] Sensor PID: 0x%02x\n", s->id.PID);
}

bool cameraInit(){
    Serial.println(psramFound()
        ? "[CAMERA] PSRAM found: frame buffer in PSRAM."
        : "[CAMERA] No PSRAM (set Tools > PSRAM to OPI PSRAM): frame buffer in DRAM.");

    esp_err_t err = ESP_OK;

    // 1) Preferred: sensor produces JPEG itself (OV2640, OV3660, OV5640).
    if (initWith(PIXFORMAT_JPEG, &err)) {
        softwareJpeg = false;
        Serial.println("[CAMERA] Hardware JPEG mode.");
        printSensor();
        return true;
    }
    Serial.printf("[CAMERA] JPEG init failed, error 0x%x\n", err);

    // 2) Sensor has no JPEG (GC0308, GC2145 and others): capture RGB565, compress in software.
    if (err == ESP_ERR_NOT_SUPPORTED) {
        Serial.println("[CAMERA] Sensor has no hardware JPEG. Using RGB565 + software JPEG.");
        esp_camera_deinit();
        if (initWith(PIXFORMAT_RGB565, &err)) {
            softwareJpeg = true;
            printSensor();
            return true;
        }
        Serial.printf("[CAMERA] RGB565 init failed, error 0x%x\n", err);
    }
    return false;
}

bool captureImage(uint8_t **outBuf, size_t *outLen){
    // The first frames after init are often stale/dark (auto-exposure): throw some away.
    for (int i = 0; i < 2; i++) {
        camera_fb_t *warm = esp_camera_fb_get();
        if (warm) esp_camera_fb_return(warm);
    }

    currentFb = esp_camera_fb_get();
    if (!currentFb){
        Serial.println("[CAMERA] Capture failed");
        return false;
    }

    if (softwareJpeg) {
        uint8_t *jpg = nullptr;
        size_t jpgLen = 0;
        bool ok = frame2jpg(currentFb, SW_JPEG_QUALITY, &jpg, &jpgLen);
        esp_camera_fb_return(currentFb);
        currentFb = nullptr;
        if (!ok || jpg == nullptr || jpgLen == 0) {
            Serial.println("[CAMERA] Software JPEG conversion failed (out of memory? enable PSRAM).");
            return false;
        }
        swJpegBuf = jpg;
        *outBuf = jpg;
        *outLen = jpgLen;
        return true;
    }

    *outBuf = currentFb->buf;
    *outLen = currentFb->len;
    return true;
}

void releaseImage(){
    if (swJpegBuf){
        free(swJpegBuf);
        swJpegBuf = nullptr;
    }
    if (currentFb){
        esp_camera_fb_return(currentFb);
        currentFb = nullptr;
    }
}
