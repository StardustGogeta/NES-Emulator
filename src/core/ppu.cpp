#include "ppu.h"
#include "cpu.h"
#include <cstring>

PPU::PPU(CPU& cpu) : cpu(cpu) {
    cyclesExecuted = scanline = cyclesOnLine = 0;
    running = false;
    bg16sr0 = bg16sr1 = bg8sr0 = bg8sr1 = 0;
    
    // Set the PPU registers to 0xff
    memset(registers, 0xff, 8);
}

uint8_t PPU::readRegister(addr_t address) {
    return registers[address];
}

void PPU::writeRegister(addr_t address, uint8_t data) {
    registers[address] = data;
}

void PPU::start() {
    running = true;

    while (running) {
        std::unique_lock lck(cpuToPpuMutex);
        cpuToPpuCV.wait(lck);
        
        // We want 3 PPU cycles for every CPU cycle.
        for (int i = 0; i < 3; i++) {
            cycle();
        }

        cpu.ppuToCpuCV.notify_one();
    }
}

void PPU::cycle() {
    if (cyclesOnLine) {
        if (cyclesOnLine < 321) {
            // TODO: Get nametable byte, attribute table byte, pattern table data as appropriate

            // Output a pixel to the screen
        }
    }

    cyclesExecuted++;
    scanline = cyclesExecuted / 341;
    cyclesOnLine = cyclesExecuted % 341;
}

void PPU::cycles(int n) {
    for (int i = 0; i < n; i++) {
        cycle();
    }
}

bool PPU::checkRunning() {
    return running;
}
