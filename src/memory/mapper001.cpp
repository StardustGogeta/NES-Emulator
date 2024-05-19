#include "mapper001.h"
#include "core_memory.h"

Mapper001::Mapper001() {
    clear();
}

uint8_t Mapper001::read(addr_t address) {
    if (0x2000 <= address && address < 0x4000) {
        return readPPU(mapPPU(address));
    }
    return memory[mapAddress(address)];
}


void Mapper001::writeDirect(exp_addr_t address, uint8_t data) {
    memory[address] = data;
}


void Mapper001::write(addr_t address, uint8_t data) {
    if (0x2000 <= address && address < 0x4000) {
        writePPU(mapPPU(address), data);
    } else if (address >= 0x8000) {
        // We are trying to write to PRG-ROM, so we intercept this
        // Instead, it modifies a shift register
        if (data & 0x80) {
            // Bit 7 is set, reset shift and control registers
            resetShift();
            controlReg |= 0x0c;
        } else {
            // Bit 7 not set, we push to the shift register
            bool lowBit = shiftReg & 1;
            shiftReg = (shiftReg >> 1) | ((data & 1) << 4);
            if (lowBit) {
                // The low bit was set, so the shift register is full
                int regId = address & 0x6000;
                uint8_t* registers[] = {&controlReg, &chrReg0, &chrReg1, &prgReg};
                *registers[regId] = shiftReg;
                resetShift();
            }
        }
    } else {
        // TODO: Prevent writing into ROM space?
        memory[mapAddress(address)] = data;
    }
}

void Mapper001::resetShift() {
    shiftReg = 0x10; // Reset shift register
}


void Mapper001::clear() {
    memory.fill(0); // Set array elements to zero
    controlReg = 0x0c; // Reset control register
    chrReg0 = chrReg1 = prgReg = 0; // Clear ROM registers
    resetShift();
}

/*
    Maps an address in the 64 KB addressable range to the proper location in memory.
*/
exp_addr_t Mapper001::mapAddress(exp_addr_t address) {
    // By implicitly promoting the argument, it can now address the full memory range
    // addr_t orig = address;
    if (address < 0x2000) {
        address %= 0x800;
    } else if (address < 0x4000) {
        address = (address % 0x8) + 0x2000;
    } else if (address >= 0x8000) {
        // This is where we access PRG-ROM
        // Figure out which PRG-ROM bank mode we are using
        // 0, 1 are 32 KB switchable
        // 2 is fixed at first and 16 KB switchable
        // 3 is 16 KB switchable and fixed at last
        int prgMode = (controlReg >> 2) & 3;
        // Figure out the index of the switchable bank
        int bankIndex = prgReg & 15;

        if (prgMode < 2) {
            // Switchable 32 KB bank
            address += (bankIndex >> 1) * 0x8000;
        } else if (address < 0xc000) {
            // We are in 0x8000 to 0xbfff
            // If prgMode == 2, we do nothing; the address is already correct
            if (prgMode == 3) {
                // Switchable 16 KB bank
                address += bankIndex * 0x4000;
            }
        } else if (prgMode == 3) {
            // We are in 0xc000 to 0xffff
            // Fixed to last bank
            // We subtract two from PRG_ROM_size to get the right index here - consider if it is one
            address += (PRG_ROM_size - 2) * 0x4000;
        } else {
            // We are in 0xc000 to 0xffff
            // Switchable 16 KB bank
            address += (bankIndex - 1) * 0x4000;
        }

        // std::cout << "Mapped " << std::hex << orig << " to " << address << " with prgmd " << prgMode << " ctrlrg " << (int)controlReg << std::dec << " bankInd " << bankIndex << " prgromsize " << (int)PRG_ROM_size << std::endl;
    } 
    return address;
}
