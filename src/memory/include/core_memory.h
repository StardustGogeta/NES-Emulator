#pragma once
#include <cstdint>
#include <memory>

using addr_t = uint16_t; // Allows addresses in the 64 KB range

// Allows expanded addresses for larger memory spaces due to mappers
// This should be much larger than necessary for any NES ROMs
using exp_addr_t = uint32_t;

class PPU;

/*
    This class is designed to encapsulate memory access to prevent
    simple mistakes with memory mirroring and other easy errors.
*/
class CoreMemory {
    friend class NES;

    public:
        virtual ~CoreMemory();

        /*
            Reads a byte of data from a given memory address.
        */
        virtual uint8_t read(addr_t address) = 0;

        uint16_t readWord(addr_t address, bool wrap=false);

        /*
            Writes a byte of data directly to a memory address, ignoring
            pre-conditions. This allows initial writing of the ROM data.
            The first argument needs to be large enough to address the full
            range of the larger mappers.
        */
        virtual void writeDirect(exp_addr_t address, uint8_t data) = 0;

        /*
            Writes a byte of data to a given memory address.
        */
        virtual void write(addr_t address, uint8_t data) = 0;

        addr_t mapPPU(addr_t address);

        uint8_t readPPU(addr_t address);

        void writePPU(addr_t address, uint8_t data);
        
        /*
            Clears all stored memory.
        */
        virtual void clear() = 0;
        
        void set_PRG_ROM_size(uint8_t PRG_ROM_size);

    protected:
        std::shared_ptr<PPU> ppu;

        CoreMemory();
        
        uint8_t PRG_ROM_size;
};
