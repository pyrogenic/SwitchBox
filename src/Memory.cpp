#include "Memory.h"

#include "DebugLine.h";
#include <Arduino.h>;

typedef struct Record {
  unsigned long ts;
  size_t largest_free_block;
};

Record startup;
Record first_loop = {0};
Record last;

#define MEMORY_CHECK_INTERVAL (1000)

void memory_setup() {
  memory_largest_free_block(true);
  startup = last;
  Serial_printf("[Memory] Boot: %db free\n", startup.largest_free_block);
}

void memory_loop() {
  memory_largest_free_block(first_loop.ts == 0);
  if (first_loop.ts == 0) {
    first_loop = last;
    Serial_printf("[Memory] Static: %db free\n", first_loop.largest_free_block);
  }
}

size_t memory_largest_free_block(bool fresh) {
  unsigned long now = millis();
  if (!fresh && last.ts + MEMORY_CHECK_INTERVAL < now) {
    return last.largest_free_block;
  }
  size_t freeMem = 0;
  int bit = sizeof(size_t) * 8 - 1;
  while (bit >= 0) {
    size_t check = freeMem | (1 << bit);
    void *mem = malloc(check);
    if (mem) {
      free(mem);
      freeMem = check;
    }
    --bit;
  }
  last = {now, freeMem};
  return freeMem;
}