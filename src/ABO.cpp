#include "ABO.h"
#include <Arduino.h>

namespace ABO {
int pin_clk;
int pin_data;
int pin_load;
int registerCount;

byte *registers;
} // namespace ABO

void abo_setup(ABOnit init) {
  ABO::pin_clk = init.clk;
  ABO::pin_data = init.data;
  ABO::pin_load = init.load;
  ABO::registerCount = init.bytes ? init.bytes : 1;

  pinMode(ABO::pin_data, INPUT);
  pinMode(ABO::pin_clk, OUTPUT);
  pinMode(ABO::pin_load, OUTPUT);

  digitalWrite(ABO::pin_clk, LOW);
  digitalWrite(ABO::pin_load, HIGH);

  ABO::registers = (byte *)calloc(sizeof(byte), ABO::registerCount);
}

void abo_loop() {
  digitalWrite(ABO::pin_load, LOW);
  // Write to shift registers, most-significant byte first
  for (int i = ABO::registerCount - 1; i >= 0; --i) {
    shiftOut(ABO::pin_data, ABO::pin_clk, MSB_FIRST, ABO::registers[i]);
  }
  digitalWrite(ABO::pin_load, HIGH);
}

void abo_pinMode(const Pin &pin, uint32_t dwMode) {
  switch (pin.type) {

  case kABIPinHardware:
    return pinMode(pin.pin, dwMode);

  case kABIPinShiftRegister:
    // NOOP
    break;
  }
}

void abo_digitalWrite(const Pin &pin, bool value) {
  switch (pin.type) {
  case kABIPinHardware:
    digitalWrite(pin.pin, value);
    break;

  case kABIPinShiftRegister:
    const int registerIndex = pin.pin / sizeof(byte);
    // assert(registerIndex >= 0);
    // assert(registerIndex < registerCount);
    const int bit = 1 << (pin.pin % sizeof(byte));
    const int bitValue = value ? 0 : 1 << bit;
    ABO::registers[registerIndex] = (ABO::registers[registerIndex] & ~bit) | bitValue;
    break;

    // default:
    //   break;
  }
}

void abo_debug() {
  for (int r = 0; r < ABO::registerCount; r++) {
    Serial.printf("Output Register %d: [ ", r);
    for (int i = 7; i >= 0; --i) {
      bool on = ABO::registers[r] & (1 << i);
      if (on) {
        Serial.printf("%d ", i);
      } else {
        Serial.print("_ ");
      }
    }
    Serial.println("]");
  }
}
