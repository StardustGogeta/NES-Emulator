#include "ppu.h"
#include "cpu.h"

PPU::PPU(CPU& cpu) : cpu(cpu) {
    cyclesExecuted = scanline = cyclesOnLine = 0;
    running = false;
    bg16sr0 = bg16sr1 = bg8sr0 = bg8sr1 = 0;
    
    // Set the PPU registers to 0xff?
    // Set initial state of 0x2002 register
    // Note that 0xa0 would be used if we began with the pre-render scanline
    writeRegister(0x2, 0x20);
}

uint8_t PPU::readRegister(addr_t address) {
    uint8_t ret = registers[address];
    if (address & 0x2) {
        // TODO: Handle more special cases with PPU registers, especially VBL timing
        // See https://www.nesdev.org/wiki/PPU_frame_timing for more details
        
        // Reading status register will change the value of bit 7
        // TODO: Fix very precise timing constraints here
        writeRegister(address, ret & ~0x80);
        // printf("Cleared VBL value!\n");
    }
    return ret;
}

void PPU::writeRegister(addr_t address, uint8_t data) {
    registers[address] = data;
    // Write to the PPU open bus
    registers[0x2] = (registers[0x2] & 0xf0) | (data & 0x0f);
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
    if (scanline < 240) {
        if (cyclesOnLine > 0) {
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
    }
    else if (scanline == 240) {
        // Idle PPU
        if (cyclesOnLine == 0) {
            // printf("Idle PPU\n");
        }
    }
    else /* 241 to 261 */ {
        // Vertical blanking
        if (scanline == 241 && cyclesOnLine == 0) {
            // Set the vblank value on the second cycle of this line
            writeRegister(0x2, readRegister(0x2) | 0x80);
        }
        if (scanline == 261 && cyclesOnLine == 0) {
            // Clear the vblank value on the second cycle of this line
            writeRegister(0x2, readRegister(0x2) & ~0x80);
        }
    }

    cyclesExecuted++;
    // If rendering is disabled, we skip this one particular PPU cycle
    if (renderingEnabled()) {
        if (scanline == 261 && cyclesOnLine == 339 && oddFrame) {
            cyclesExecuted++;
        }
    }
    scanline = (cyclesExecuted / 341) % 262;
    cyclesOnLine = cyclesExecuted % 341;
    // Maintain flag for frame parity check
    oddFrame = cyclesExecuted / (341 * 262) % 2 > 0;
}

bool PPU::renderingEnabled() {
    uint8_t mask = registers[1];
    return (mask & 0b11000) != 0;
}

void PPU::cycles(int n) {
    for (int i = 0; i < n; i++) {
        cycle();
    }
}

bool PPU::checkRunning() {
    return running;
}
