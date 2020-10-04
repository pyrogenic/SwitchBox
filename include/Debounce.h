#pragma once

#include "ABI.h"

#define DEBOUNCE_INTERVAL (50)
#define STEADY_INTERVAL (300)

typedef struct {
  Pin pin;
  int value;
  int last;
  int debounce;
} ButtonState;

bool debounce(ButtonState &buttonState, bool raw = false);

bool debounce_steady(ButtonState &buttonState);
