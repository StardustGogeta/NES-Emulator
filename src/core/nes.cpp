#include "nes.h"

NES::NES() {
    memory = nullptr;
    cpu = new CPU();
    ppu = new PPU(*cpu);
    cpu->ppu = ppu;
}

NES::~NES() {
    delete memory;
    delete cpu;
    delete ppu;
}

void NES::loadROM(ROM* rom) {
    memory = rom->loadIntoMemory();
    cpu->memory = memory;
    ppu->memory = memory;
    memory->ppu = ppu;
    cpu->setPC(true); // Load initial program counter from reset vector, don't add cycles
}
