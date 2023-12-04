#include "nes.h"

NES::NES() {
    memory = nullptr;
    cpu = std::make_unique<CPU>();
    ppu = std::make_shared<PPU>(*cpu);
    cpu->ppu = ppu;
}

void NES::loadROM(ROM& rom) {
    memory = rom.loadIntoMemory();
    cpu->memory = memory;
    ppu->memory = memory;
    memory->ppu = ppu;
    cpu->setPC(true); // Load initial program counter from reset vector, don't add cycles
}
