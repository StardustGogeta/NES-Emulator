#pragma once
#include "core_memory.h"

class Mapper000 : public CoreMemory {
    public:
        Mapper000();
        uint8_t read(addr_t address);
        void writeDirect(exp_addr_t address, uint8_t data);
        void write(addr_t address, uint8_t data);
        void clear();

    private:
        uint8_t memory[0x10000];
        addr_t mapAddress(addr_t address);
};
