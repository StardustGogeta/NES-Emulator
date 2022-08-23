#include "nes.h"

NES::NES() {
    memory = new Memory();
    cpu = new CPU();
}

void NES::loadROM(ROM* rom) {
    rom->loadIntoMemory(memory);
}
