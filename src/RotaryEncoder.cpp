#include "RotaryEncoder.h"
#include "DebugLine.h"
#include <Arduino.h>

void rotary_setup(RotaryState &state) {
  pinMode(state.pinA.pin, INPUT);
  pinMode(state.pinB.pin, INPUT);
  pinMode(state.pinSwitch.pin, INPUT);

  digitalWrite(state.pinA.pin, HIGH);
  digitalWrite(state.pinB.pin, HIGH);
  digitalWrite(state.pinSwitch.pin, HIGH);
}

unsigned long ms = millis();

RotaryAction rotary_loop(RotaryState &state) {
  if (debounce(state.pinSwitch)) {
    if (!state.pinSwitch.value) {
      return kRotaryActionClick;
    }
  }
  // rest state - 0 0
  // rotating - 1 0 > 1 1 - clockwise
  bool a0 = state.pinA.value;
  bool b0 = state.pinB.value;
  if (a0 && b0) {
    bool clockwise = state.pinA.debounce < state.pinB.debounce;
    bool da = debounce(state.pinA);
    bool db = debounce(state.pinB);
    if (da && db) {
      return clockwise ? kRotaryActionClockwiseUp : kRotaryActionWiddershinsUp;
    }
    if (da) {
      return kRotaryActionClockwiseUp;
    }
    if (db) {
      return kRotaryActionWiddershinsUp;
    }
    return clockwise ? kRotaryActionClockwiseDown : kRotaryActionWiddershinsDown;
  }
  if (a0) {
    if (debounce(state.pinA)) {
      return kRotaryActionNone;
    }
    debounce(state.pinB);
    return kRotaryActionClockwiseDown;
  }
  if (b0) {
    if (debounce(state.pinB)) {
      return kRotaryActionNone;
    }
    debounce(state.pinA);
    return kRotaryActionWiddershinsDown;
  }
  debounce(state.pinA);
  debounce(state.pinB);
  return kRotaryActionNone;
}
