#pragma once

#include <Arduino.h>

#define BITS_PER_BYTE 8

typedef enum {
  kABIPinHardware = 0,
  kABIPinShiftRegister = 1,
} PinType;

typedef struct {
  PinType type;
  uint32_t pin;
} Pin;

#define AB_DELAY(N) delayMicroseconds(5 * N)
