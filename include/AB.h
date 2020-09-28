#pragma once

#include <Arduino.h>
#include <cstdint>

typedef enum {
  kABIPinHardware = 0,
  kABIPinShiftRegister = 1,
} PinType;

typedef struct {
  PinType type;
  uint32_t pin;
} Pin;

#define AB_DELAY(N) delay(N)
