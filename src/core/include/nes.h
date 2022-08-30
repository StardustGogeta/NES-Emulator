#pragma once
#include "rom.h"
#include "core_memory.h"
#include "cpu.h"

class NES {
    public:
        NES();
        ~NES();
        void loadROM(ROM* rom);

        CoreMemory* memory;
        CPU* cpu;
        
    private:
        NES(const NES&) = delete;
        NES& operator=(const NES&) = delete;
};
