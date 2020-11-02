#include "RotaryEncoder.h"
#include "ABO.h"
#include "DebugLine.h"
#include <Arduino.h>

void rotary_setup(RotaryState &state) {
  if (state.pinA.pin.type == kABIPinHardware) {
    abi_pinMode(state.pinB.pin, INPUT);
    abo_digitalWrite(state.pinA.pin, HIGH);
  }
  debounce(state.pinA);
  if (state.pinB.pin.type == kABIPinHardware) {
    abi_pinMode(state.pinA.pin, INPUT);
    abo_digitalWrite(state.pinB.pin, HIGH);
  }
  debounce(state.pinB);
  if (state.pinSwitch.pin.type == kABIPinHardware) {
    abi_pinMode(state.pinSwitch.pin, INPUT);
    abo_digitalWrite(state.pinSwitch.pin, HIGH);
  }
  debounce(state.pinSwitch);
}

unsigned long ms = millis();

RotaryAction rotary_loop(RotaryState &state) {
  return kRotaryActionNone;
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
