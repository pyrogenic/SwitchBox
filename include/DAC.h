#pragma once

// Make sure that this is set to the value in volts of VCC
#define ADC_REFERENCE_VOLTAGE 5.0
#include <Adafruit_PCF8591.h>
#include <Arduino.h>

typedef enum DACChannel {
  AIN0 = _BV(0),
  AIN1 = _BV(1),
  AIN2 = _BV(2),
  AIN3 = _BV(3),
};

class DAC {
public:
  DAC(uint8_t i2caddr = PCF8591_DEFAULT_ADDR, TwoWire *theWire = &Wire) : i2caddr(i2caddr), theWire(theWire), found(false), pcf8591() {}
  void setup();
  bool available() const { return found; }
  void debug();

private:
  uint8_t i2caddr;
  TwoWire *theWire;
  bool found;
  Adafruit_PCF8591 pcf8591;
};
