#pragma once

#include "AB.h"

typedef struct {
  int clk;
  int data;
  int load;
  int read;
  int bytes;
} ABInit;

void abi_setup(ABInit init);
void abi_loop();
int abi_digitalRead(const Pin &pin);
void abi_pinMode(const Pin &pin, uint32_t dwMode);
void abi_debug();
