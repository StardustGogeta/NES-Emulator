#pragma once
#include "rom.h"
#include "core_memory.h"
#include "cpu.h"
#include "ppu.h"

class NES {
    public:
        NES();
        void loadROM(ROM& rom);

        std::shared_ptr<CoreMemory> memory;
        std::unique_ptr<CPU> cpu;
        std::shared_ptr<PPU> ppu;
        
    private:
        NES(const NES&) = delete;
        NES& operator=(const NES&) = delete;
};
