#pragma once
#include "ABO.h"

typedef enum PowerState {
  kPS_off,
  kPS_warmingUp,
  kPS_on,
  kPS_,
} PowerState;

class PowerControl {
public:
  PowerControl(Pin &pin, PowerState PowerTiming &timing) : pin(pin), timing(timing), state(state) {}

private:
  Pin pin;
};
