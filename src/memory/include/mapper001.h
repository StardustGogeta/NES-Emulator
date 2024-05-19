#pragma once
#include "core_memory.h"
#include <array>

class Mapper001 : public CoreMemory {
    public:
        Mapper001();
        uint8_t read(addr_t address);
        void writeDirect(exp_addr_t address, uint8_t data);
        void write(addr_t address, uint8_t data);
        void clear();

    private:
        std::array<uint8_t, 0x48000> memory;
        uint8_t shiftReg, controlReg,
            chrReg0, chrReg1, prgReg;

        exp_addr_t mapAddress(exp_addr_t address);
        void resetShift();
};
