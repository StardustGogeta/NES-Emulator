#pragma once
#include <cstdint>

/*
    This class is designed to encapsulate memory access to prevent
    simple mistakes with memory mirroring and other easy errors.
*/
class Memory {
    public:
        typedef uint16_t addr_t;
        
        Memory();
        Memory(uint8_t PRG_ROM_size);
        uint8_t read(addr_t address);
        uint16_t readWord(addr_t address);
        void write(addr_t address, uint8_t data);
        void writeBlock(addr_t address, uint8_t *src, uint16_t numBytes);
        void clear();
        void set_PRG_ROM_size(uint8_t PRG_ROM_size);

    private:
        uint8_t memory[0x10000],
            PRG_ROM_size;
        addr_t mapAddress(addr_t address);
};
