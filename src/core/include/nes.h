#pragma once
#include "rom.h"
#include "memory.h"
#include "cpu.h"

class NES {
    public:
        NES();
        ~NES();
        void loadROM(ROM* rom);

        Memory* memory;
        CPU* cpu;
        
    private:
        NES(const NES&) = delete;
        NES& operator=(const NES&) = delete;
};
