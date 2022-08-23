#include "memory.h"
#include "rom.h"
#include <cstring>

/*
    Create a new memory object and store the PRG ROM size (in 16 KB units)
*/
Memory::Memory(uint8_t PRG_ROM_size) {
    this->PRG_ROM_size = PRG_ROM_size;
    clear();
}

/*
    Reads a byte of data from a given memory address.
*/
uint8_t Memory::read(addr_t address) {
    return memory[mapAddress(address)];
}

/*
    Writes a byte of data to a given memory address.
*/
void Memory::write(addr_t address, uint8_t data) {
    memory[mapAddress(address)] = data;
}

/*
    Writes a block of data from a source to a given memory address.
*/
void Memory::writeBlock(addr_t address, uint8_t *src, uint16_t numBytes) {
    memcpy(&memory[mapAddress(address)], src, numBytes);
}

/*
    Clears all stored memory.
*/
void Memory::clear() {
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
Memory::addr_t Memory::mapAddress(addr_t address) {
    if (address < 0x2000) {
        address %= 0x800;
    }
    else if (address < 0x4000) {
        address = (address % 0x8) + 0x2000;
    }
    else if (address >= 0xC000 && PRG_ROM_size == 1) { // Mirrors 0x8000-0xBFFF to 0xC000-0xFFFF
        address -= 0x4000;
    }
    return address;
}
