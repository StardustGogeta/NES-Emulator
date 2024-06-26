#include "mapper000.h"
#include "core_memory.h"
#include <cstring>

Mapper000::Mapper000() {
    clear();
}

uint8_t Mapper000::read(addr_t address) {
    if (0x2000 <= address && address < 0x4000) {
        return readPPU(mapPPU(address));
    }
    return memory[mapAddress(address)];
}


void Mapper000::writeDirect(exp_addr_t address, uint8_t data) {
    memory[address] = data;
}


void Mapper000::write(addr_t address, uint8_t data) {
    if (0x2000 <= address && address < 0x4000) {
        writePPU(mapPPU(address), data);
    } else {
        // TODO: Prevent writing into ROM space?
        memory[mapAddress(address)] = data;
    }
}


void Mapper000::clear() {
    memset(memory, 0, sizeof(memory)); // Set array elements to zero
}

/*
    Takes in a memory address and returns a new one to
    simulate memory mirroring performed by the NES.

    Memory Layout:
    0x0000-0x00ff: Zero-page
    0x0100-0x01ff: Stack, where the stack pointer decreases as the stack grows.
    0x0200-0x07ff: General-purpose RAM
    0x0800-0x1fff: Mirror of previous section
    0x2000-0x2007: PPU control registers
    0x2008-0x3fff: Mirror of previous section
    0x4000-0x401f: Registers
    0x4020-0x5fff: Cartridge expansion ROM
    0x6000-0x7fff: Save RAM (SRAM)
    0x8000-0xbfff: PRG-ROM lower bank
    0xc000-0xffff: PRG-ROM upper bank (mirror of previous if only one bank)
        - 0xfffa-0xfffb: Non-maskable interrupt handler
        - 0xfffc-0xfffd: Power-on/reset handler
        - 0xfffe-0xffff: Break handler

*/
addr_t Mapper000::mapAddress(addr_t address) {
    if (address < 0x2000) {
        address %= 0x800;
    }
    else if (address < 0x4000) {
        address = (address % 0x8) + 0x2000;
    }
    else if (address >= 0xc000 && PRG_ROM_size == 1) { // Mirrors 0x8000-0xBFFF to 0xC000-0xFFFF
        address -= 0x4000;
    }
    return address;
}
