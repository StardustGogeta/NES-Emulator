#pragma once
#include <cstdint>

typedef uint16_t addr_t;

/*
    This class is designed to encapsulate memory access to prevent
    simple mistakes with memory mirroring and other easy errors.
*/
class Memory {
    public:
        uint8_t read(addr_t address);
        void write(addr_t address, uint8_t data);
        void clear();

    private:
        uint8_t memory[0x10000];
        addr_t mapAddress(addr_t address);
};
