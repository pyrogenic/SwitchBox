#pragma once

#define DEBOUNCE_INTERVAL (50)

typedef struct {
  int pin;
  int value;
  int last;
  int debounce;
} ButtonState;

bool debounce(ButtonState &buttonState, bool raw = false);
