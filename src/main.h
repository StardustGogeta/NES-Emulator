/*
    This header file is abusing its abilities.

    Rather than helping share variables and functions across files,
    it is being used to share global variables across the same file.

    This results in less functional-programming style, but is much easier
    when C++ lacks Python's parallel assignment features.
*/

#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <iomanip>
#include <assert.h>

char PRG_ROM_size, CHR_ROM_size, flags6, flags7, PRG_RAM_size, flags9, flags10, mapper;
std::string mirroring;
bool persistentMemory, trainer, fourScreenVRAM, NES2, playchoice10, VS_unisystem;
