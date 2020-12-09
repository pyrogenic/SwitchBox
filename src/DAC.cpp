#include "DAC.h"

void DAC::setup() {
#if DAC == PCF8591
  found = pcf8591.begin(i2caddr, theWire);
  Serial.print("DAC: connected? ");
  Serial.println(found ? "yes" : "no");
#endif
}

void DAC::debug() {
  if (!available()) {
    return;
  }

#if DAC == PCF8591
  // static byte debugLoop(0);

  int ana = pcf8591.analogRead(AIN0);
  Serial.print("AIN0 --> ");
  Serial.println(ana);

  // ana = pcf8591.analogRead(AIN1);
  // Serial.print("AIN1 --> ");
  // Serial.println(ana);

  // ana = pcf8591.analogRead(AIN2);
  // Serial.print("AIN2 --> ");
  // Serial.println(ana);

  // ana = pcf8591.analogRead(AIN3);
  // Serial.print("AIN3 --> ");
  // Serial.println(ana);

  // pcf8591.analogWrite(++debugLoop);
  // debugLoop = debugLoop % 256;
#endif
}