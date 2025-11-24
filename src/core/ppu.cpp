#include "ppu.h"
#include "cpu.h"
#include <cassert>
#include <cstdint>

namespace {
    constexpr uint16_t PPUCTRL_NMI_OUTPUT = 0x80;
    constexpr uint16_t PPUSTATUS_VBLANK = 0x80;
};

PPU::PPU(CPU& cpu) : cpu(cpu) {
    cyclesExecuted = scanline = cyclesOnLine = 0;
    running = false;
    bg16sr0 = bg16sr1 = bg8sr0 = bg8sr1 = 0;
}

uint8_t PPU::readRegister(addr_t address) {
    assert(address <= PPUDATA);
    uint8_t ret = registers[address];
    if (address == PPUSTATUS) {
        // TODO: Handle more special cases with PPU registers, especially VBL timing
        // See https://www.nesdev.org/wiki/PPU_frame_timing for more details
        
        // Reading status register will change the value of bit 7
        // TODO: Fix very precise timing constraints here
        writeRegister(PPUSTATUS, ret & ~PPUSTATUS_VBLANK);
        // printf("Cleared VBL value!\n");
    }
    return ret;
}

void PPU::writeRegister(addr_t address, uint8_t data) {
    // TODO: figure out how to ignore writes before reaching the pre-render
    // scanline of the next frame (after 33132 PAL cycles)

    registers[address] = data;
    // Write to the PPU open bus
    // TODO: match all the behaviors listed here: https://www.nesdev.org/wiki/Open_bus_behavior#PPU_open_bus
    registers[PPUSTATUS] = (registers[PPUSTATUS] & 0xe0) | (data & 0x1f);

    // PPU should turn on an non-maskable interrupt if and only if the
    // vblank flag is high and NMI output is high
    if ((address == PPUCTRL || address == PPUSTATUS) && 
        (registers[PPUCTRL] & PPUCTRL_NMI_OUTPUT) &&
        (registers[PPUSTATUS] & PPUSTATUS_VBLANK)) {
        nmiState = true;
    }
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
            writeRegister(PPUSTATUS, readRegister(PPUSTATUS) | PPUSTATUS_VBLANK);
        }
        if (scanline == 261 && cyclesOnLine == 0) {
            // Clear the vblank bit on the second cycle of this line
            // TODO: Also clear sprite overflow bit?
            writeRegister(PPUSTATUS, readRegister(PPUSTATUS) & ~PPUSTATUS_VBLANK);
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
    return (registers[PPUMASK] & 0b11000) != 0;
}

void PPU::cycles(int n) {
    for (int i = 0; i < n; i++) {
        cycle();
    }
}

bool PPU::checkRunning() {
    return running;
}

bool PPU::checkNmiFlag() const noexcept {
    return nmiState;
}

void PPU::clearNmiFlag() {
    nmiState = false;
}
