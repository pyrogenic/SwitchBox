#pragma once

#include "AB.h"

typedef struct {
  int clk;
  int data;
  int load;
  int bytes;
} ABOnit;

void abo_setup(ABOnit init);
void abo_loop();
void abo_digitalWrite(const Pin &pin, bool value);
void abo_pinMode(const Pin &pin, uint32_t dwMode);
void abo_debug();
