#pragma once
#include <cstring>
#include <fstream>

// Data

extern uint8_t header[16], memory[0x10000], PRG_ROM_size, CHR_ROM_size,
    flags6, flags7, PRG_RAM_size, flags9, flags10, mapper;

extern std::string mirroring;

extern bool persistentMemory, trainer, fourScreenVRAM, NES2, playchoice10, VS_unisystem;

extern std::ifstream ROM;

// Functions

void loadRom(std::string);
