#include "ABI.h"
#include <Arduino.h>

namespace ABI {
int pin_clk;
int pin_data;
int pin_load;
int pin_read;
int registerCount;

byte *registers;
} // namespace ABI

void abi_setup(ABInit init) {
  ABI::pin_clk = init.clk;
  ABI::pin_data = init.data;
  ABI::pin_load = init.load;
  ABI::pin_read = init.read;
  ABI::registerCount = init.bytes ? init.bytes : 1;
  pinMode(ABI::pin_data, INPUT);
  pinMode(ABI::pin_clk, OUTPUT);
  pinMode(ABI::pin_load, OUTPUT);
  pinMode(ABI::pin_read, OUTPUT);

  digitalWrite(ABI::pin_clk, LOW);
  digitalWrite(ABI::pin_load, HIGH);
  digitalWrite(ABI::pin_read, HIGH);

  ABI::registers = (byte *)calloc(sizeof(byte), ABI::registerCount);
}

void abi_loop() {
  digitalWrite(ABI::pin_read, HIGH);

  // digitalWrite(clockEnablePin, HIGH);
  // digitalWrite(ploadPin, LOW);
  // delayMicroseconds(PULSE_WIDTH_USEC);
  // digitalWrite(ploadPin, HIGH);
  // digitalWrite(clockEnablePin, LOW);

  // Load data into shift register
  digitalWrite(ABI::pin_load, LOW);
  AB_DELAY(1);
  digitalWrite(ABI::pin_load, HIGH);

  // Enable read
  digitalWrite(ABI::pin_clk, LOW);
  digitalWrite(ABI::pin_read, LOW);
  // Read from shift ABI::registers, least-significant byte first
  for (int r = 0; r < ABI::registerCount; ++r) {
    // ABI::registers[i] = shiftIn(ABI::pin_data, ABI::pin_clk, LSB_FIRST);
    byte value = 0;
    for (int i = 7; i >= 0; --i) {
      if (digitalRead(ABI::pin_data)) {
        value |= (1 << i);
      }
      digitalWrite(ABI::pin_clk, HIGH);
      AB_DELAY(1);
      digitalWrite(ABI::pin_clk, LOW);
    }
    ABI::registers[r] = value;
  }
  digitalWrite(ABI::pin_read, HIGH);
}

void abi_pinMode(const Pin &pin, uint32_t dwMode) {
  switch (pin.type) {

  case kABIPinHardware:
    return pinMode(pin.pin, dwMode);

  case kABIPinShiftRegister:
    // NOOP
    break;
  }
}

int abi_digitalRead(const Pin &pin) {
  switch (pin.type) {

  case kABIPinHardware:
    return digitalRead(pin.pin);

  case kABIPinShiftRegister:
    const int registerIndex = pin.pin / sizeof(byte);
    // assert(registerIndex >= 0);
    // assert(registerIndex < ABI::registerCount);
    const int bit = 1 << (pin.pin % sizeof(byte));
    return bit == (ABI::registers[registerIndex]);
  }

  return 0;
}

void abi_debug() {
  for (int r = 0; r < ABI::registerCount; r++) {
    Serial.printf("Input Register %d: [ ", r);
    for (int i = 7; i >= 0; --i) {
      bool on = ABI::registers[r] & (1 << i);
      if (on) {
        Serial.printf("%d ", i);
      } else {
        Serial.print("_ ");
      }
    }
    Serial.println("]");
  }
}
