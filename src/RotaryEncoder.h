#pragma once

#include "Debounce.h"

typedef struct {
  ButtonState pinA;
  ButtonState pinB;
  ButtonState pinSwitch;
} RotaryState;

enum RotaryAction {
  kRotaryActionWiddershinsUp = -2,
  kRotaryActionWiddershinsDown = -1,
  kRotaryActionNone = 0,
  kRotaryActionClockwiseDown = 1,
  kRotaryActionClockwiseUp = 2,
  kRotaryActionClick = 3,
};

void rotary_setup(RotaryState &state);

RotaryAction rotary_loop(RotaryState &state);
