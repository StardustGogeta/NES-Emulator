#include "nes.h"

NES::NES() {
    memory = new CoreMemory();
    cpu = new CPU();
    ppu = new PPU(*cpu);
    cpu->memory = memory;
    cpu->ppu = ppu;
}

NES::~NES() {
    delete memory;
    delete cpu;
    delete ppu;
}

void NES::loadROM(ROM* rom) {
    rom->loadIntoMemory(memory);
}
