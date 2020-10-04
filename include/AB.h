#pragma once

#include <Arduino.h>
#include <cstdint>

#define BITS_PER_BYTE 8

typedef enum {
  kABIPinHardware = 0,
  kABIPinShiftRegister = 1,
} PinType;

typedef struct {
  PinType type;
  int pin;
} Pin;

#define AB_DELAY(N) delayMicroseconds(5 * N)
