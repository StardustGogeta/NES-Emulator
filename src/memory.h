#pragma once
#include <cstdint>

// Data

extern uint8_t memory[0x10000];

// Functions

uint8_t readFromMemory(int);

void clearMemory();
