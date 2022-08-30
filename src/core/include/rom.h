#pragma once
#include "core_memory.h"
#include <string>
#include <cstdint>

class ROM {
    public:
        std::string mirroring;
        bool persistentMemory, trainer, fourScreenVRAM, nes2,
            playchoice10, VS_unisystem;
        uint8_t mapper, PRG_ROM_size, CHR_ROM_size, PRG_RAM_size;

        void setPath(std::string path);
        void parseHeader();
        void loadIntoMemory(CoreMemory* memory);

    private:
        uint8_t flags6, flags7, flags9, flags10;
        std::string path;
};
