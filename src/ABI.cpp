#include "ABI.h"
#include "DebugLine.h"
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
  // digitalWrite(clockEnablePin, HIGH);
  // digitalWrite(ploadPin, LOW);
  // delayMicroseconds(PULSE_WIDTH_USEC);
  // digitalWrite(ploadPin, HIGH);
  // digitalWrite(clockEnablePin, LOW);

  // Load data into shift register
  digitalWrite(ABI::pin_load, LOW);
  AB_DELAY(1);
  digitalWrite(ABI::pin_load, HIGH);
  AB_DELAY(1);

  // Enable read
  digitalWrite(ABI::pin_read, LOW);
  digitalWrite(ABI::pin_clk, LOW); // should be redundant
  // Read from shift ABI::registers, most-significant byte first
  for (int r = ABI::registerCount - 1; r >= 0; --r) {
    // ABI::registers[i] = shiftIn(ABI::pin_data, ABI::pin_clk, LSB_FIRST);
    byte value = 0;
    for (int i = 7; i >= 0; --i) {
      if (digitalRead(ABI::pin_data)) {
        value |= (1 << i);
      }
      digitalWrite(ABI::pin_clk, HIGH);
      AB_DELAY(1);
      digitalWrite(ABI::pin_clk, LOW);
      AB_DELAY(1);
    }
    ABI::registers[r] = value;
  }
  digitalWrite(ABI::pin_read, HIGH);
  AB_DELAY(10);
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
    const int registerIndex = pin.pin / BITS_PER_BYTE;
    if (registerIndex < 0) {
      Serial_printf("Illegal register index %d for pin %d\n", registerIndex, pin.pin);
      return 0;
    } else if (registerIndex >= ABI::registerCount) {
      Serial_printf("Illegal register index %d for pin %d\n", registerIndex, pin.pin);
      return 0;
    }
    const int bit = 1 << (pin.pin % BITS_PER_BYTE);
    return bit == (ABI::registers[registerIndex] & bit);
  }

  Serial_printf("Illegal pin type %d\n", pin.type);
  return 0;
}

void abi_debug() {
  for (int registerIndex = 0; registerIndex < ABI::registerCount; ++registerIndex) {
    Serial_printf("Input Register %d: [ ", registerIndex);
    for (int bitIndex = 7; bitIndex >= 0; --bitIndex) {
      const Pin pin = {kABIPinShiftRegister, static_cast<uint32_t>(bitIndex + (BITS_PER_BYTE * registerIndex))};
      bool on = abi_digitalRead(pin);
      if (on) {
        Serial_printf("%d ", bitIndex);
      } else {
        Serial.print("_ ");
      }
    }
    Serial.println("]");
  }
}
