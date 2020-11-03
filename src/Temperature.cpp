#include "temperature.h"

#include "DebugLine.h"

Temperature::Temperature(OneWire &oneWire) : oneWire(oneWire), sensors(&oneWire) {
}

void Temperature::setup() {
  sensors.begin();
  uint8_t count = sensors.getDeviceCount();
  Serial_printf("%d total sensors\n");
  count = sensors.getDS18Count();
  Serial_printf("Temperature Sensors:\n");
  Serial_printf("  DS18: %d\n", count);
};

//
