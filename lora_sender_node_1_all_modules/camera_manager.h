#pragma once
#include <stdint.h>
#include <stddef.h>

bool cameraInit();
bool captureImage(uint8_t **outBuf, size_t *outLen);
void releaseImage();