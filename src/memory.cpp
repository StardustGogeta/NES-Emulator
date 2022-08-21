#include "memory.h"
#include "rom.h"
#include <cstdint>

// Data

uint8_t memory[0x10000];

// Functions

uint8_t readFromMemory(int index) { // Handles memory mirroring
    if (index < 0x2000) {
        return memory[index % 0x800];
    }
    else if (index < 0x4000) {
        return memory[(index % 0x8) + 0x2000];
    }
    else if (index < 0xC000) {
        return memory[index];
    }
    else if (PRG_ROM_size == 1) { // Mirrors 0x8000-0xBFFF to 0xC000-0xFFFF
        return memory[index - 0x4000];
    }
    else { // PRG ROM is not mirrored
        return memory[index];
    }
}

void clearMemory() {
    memset(memory, 0, sizeof(memory)); // Set array elements to zero
}
