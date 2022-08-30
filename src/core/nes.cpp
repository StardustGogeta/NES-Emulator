#include "nes.h"

NES::NES() {
    memory = new CoreMemory();
    cpu = new CPU();
    cpu->memory = memory;
}

NES::~NES() {
    delete memory;
    delete cpu;
}

void NES::loadROM(ROM* rom) {
    rom->loadIntoMemory(memory);
}
