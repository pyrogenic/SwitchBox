#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "DebugLine.h"

char buffer[255] = { 0 };

void debug_set(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);
}

const char * debug_get() {
  return buffer;
}

//void debug_loop() {
//
//}
