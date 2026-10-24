
#pragma once

#include <Arduino.h>

enum ButtonAction {
    BUTTON_NO_ACTION,
    BUTTON_SINGLE_PRESS,
    BUTTON_DOUBLE_PRESS
};

void beginSosButton();
ButtonAction pollSosButton();
