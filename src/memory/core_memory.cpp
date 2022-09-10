#include "core_memory.h"
#include "rom.h"
#include "ppu.h"
#include <cstring>
#include <iostream>

/*
    Create a new generic memory object.
*/
CoreMemory::CoreMemory() {
    PRG_ROM_size = 0;
}

/*
    Destroy the generic memory object. 
*/
CoreMemory::~CoreMemory() {}

/*
    Reads two consecutive bytes of data from a given memory address.
    If wrap is true, the second read wraps to the beginning of the page.
*/
uint16_t CoreMemory::readWord(addr_t address, bool wrap /* =false */) {
    addr_t addr2 = address + 1;
    if (wrap) {
        // Check if second address is on next page
        if (!(addr2 % 0x100)) {
            addr2 -= 0x100;
        }
    }
    return (read(addr2) << 8) | read(address); 
}

/*
    Maps a memory address in the valid range to one of the PPU register indices.
*/
addr_t CoreMemory::mapPPU(addr_t address) {
    if (0x2000 > address || address >= 0x4000) {
        throw std::runtime_error("Address is not in valid PPU register range!");
    }
    return (address - 0x2000) % 8;
}

/*
    Reads a byte of data from one of the PPU registers.
*/
uint8_t CoreMemory::readPPU(addr_t address) {
    return ppu->readRegister(address);
}

/*
    Writes a byte of data to one of the PPU registers.
*/
void CoreMemory::writePPU(addr_t address, uint8_t data) {
    return ppu->writeRegister(address, data);
}

/*
    Sets the PRG-ROM size (in 16 KB units).
*/
void CoreMemory::set_PRG_ROM_size(uint8_t PRG_ROM_size) {
    this->PRG_ROM_size = PRG_ROM_size;
}

