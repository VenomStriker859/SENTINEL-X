
#include "button_manager.h"

static constexpr uint8_t BUTTON_PIN = 38;
static constexpr unsigned long DEBOUNCE_MS = 40;
static constexpr unsigned long DOUBLE_PRESS_MS = 350;

static bool lastRawState = HIGH;
static bool stableState = HIGH;
static unsigned long lastChangeTime = 0;
static unsigned long lastReleaseTime = 0;
static uint8_t clickCount = 0;

void beginSosButton() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    lastRawState = digitalRead(BUTTON_PIN);
    stableState = lastRawState;
    lastChangeTime = millis();

    Serial.println("[BUTTON] Button manager initialized.");
}

ButtonAction pollSosButton() {
    unsigned long now = millis();
    bool rawState = digitalRead(BUTTON_PIN);

    if (rawState != lastRawState) {
        lastRawState = rawState;
        lastChangeTime = now;
    }

    if (now - lastChangeTime >= DEBOUNCE_MS &&
        rawState != stableState) {
        stableState = rawState;

        if (stableState == HIGH) {
            clickCount++;
            lastReleaseTime = now;

            Serial.print("[BUTTON DEBUG] Release detected. Click count: ");
            Serial.println(clickCount);
        }
    }

    if (clickCount > 0 &&
        now - lastReleaseTime >= DOUBLE_PRESS_MS) {
        uint8_t completedClicks = clickCount;
        clickCount = 0;

        if (completedClicks == 1) {
            Serial.println("[BUTTON DEBUG] Single click confirmed.");
            return BUTTON_SINGLE_PRESS;
        }

        if (completedClicks >= 2) {
            Serial.println("[BUTTON DEBUG] Double click confirmed.");
            return BUTTON_DOUBLE_PRESS;
        }
    }

    return BUTTON_NO_ACTION;
}
