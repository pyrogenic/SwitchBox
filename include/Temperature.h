#pragma once

#include <DallasTemperature.h>
#include <OneWire.h>

typedef enum {
  kTS_DS3231,
  kTS_DS18,
} TemperatureSensorType;

class Temperature {
public:
  Temperature(OneWire &oneWire);

  void setup();

private:
  OneWire &oneWire;
  DallasTemperature sensors;
};
