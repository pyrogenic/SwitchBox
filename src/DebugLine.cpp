#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DebugLine.h"

char buffer[255] = {0};

void debug_set(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 255, fmt, args);
  va_end(args);
}

const char *debug_get() {
  return buffer;
}

void Serial_printf(const char *fmt, ...) {
  if (!Serial.available()) {
    return;
  }
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
  Serial.print(buffer);
}

// void debug_loop() {
//
//}
