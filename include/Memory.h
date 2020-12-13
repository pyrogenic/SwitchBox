#pragma once

#include <stdlib.h>

void memory_setup();
void memory_loop();
size_t memory_largest_free_block(bool fresh);
