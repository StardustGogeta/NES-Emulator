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
        // The first cycle is idle and should be ignored
        if (cyclesOnLine < 257) {
            /*
                We perform four memory accesses, each one taking two cycles.
                The PPU memory should not be accessed at this time, so we can just populate everything at once when needed.
                The four accesses are:
                - Nametable byte
                - Attribute table byte
                - Pattern table tile low
                - Pattern table tile high
            */
        }
        else if (cyclesOnLine < 321) {
            /*
                Now, we perform the following four memory accesses:
                - Garbage nametable byte
                - Garbage nametable byte
                - Pattern table tile low
                - Pattern table tile high
                Each takes two cycles, and we repeat for each of the eight sprites.
            */
        }
        else if (cyclesOnLine < 337) {
            /*
                We perform four memory accesses, each one taking two cycles, for two tiles.
                - Nametable byte
                - Attribute table byte
                - Pattern table tile low
                - Pattern table tile high
            */
        }
        else {
            /*
                Two final nametable bytes are fetched. (Used for MMC5.)
            */
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
