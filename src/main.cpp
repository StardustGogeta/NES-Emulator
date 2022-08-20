#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <iomanip>

char header[16], memory[0x10000], PRG_ROM_size, CHR_ROM_size,
    flags6, flags7, PRG_RAM_size, flags9, flags10, mapper;
std::string mirroring;
bool persistentMemory, trainer, fourScreenVRAM, NES2, playchoice10, VS_unisystem;
std::ifstream ROM;

void loadRom(std::string path) {
    std::ifstream ROM;
    ROM.open(path, std::ifstream::ate | std::ifstream::binary);
    int ROMsize = ROM.tellg(); // Find file length
    std::cout << "The file size is " << ROMsize << " bytes.\n";
    ROM.seekg(0); // Reset to start of file
    char header[16];
    for (int x=0; x<16; x++) {
        ROM >> std::hex >> header[x];
        // cout << setfill('0') << setw(2) << hex << (int)header[x] << " Byte" << endl;
    }

    if (!(header[0] == 0x4e && header[1] == 0x45 && header[2] == 0x53 && header[3] == 0x1a)) {
        std::cout << "The NES file header is invalid.\n";
    } // Check that the first 4 characters are "NES\n"

    PRG_ROM_size = header[4], CHR_ROM_size = header[5],
    flags6 = header[6], flags7 = header[7], PRG_RAM_size = header[8],
    flags9 = header[9], flags10 = header[10]; // Identify all header components

    // Decode flags 6 and 7
    mirroring = flags6 & 0b00000001 ? "vertical" : "horizontal";
    persistentMemory    = flags6 & 0b00000010;
    trainer             = flags6 & 0b00000100;
    fourScreenVRAM      = flags6 & 0b00001000;
    mapper              =(flags6 & 0b11110000) + ((flags7 & 0b11110000) >> 4); // Mappers not supported
    NES2                =(flags7 & 0b00001100) == 0b1000; // NES 2.0 not fully supported
    playchoice10        = flags7 & 0b00000010;
    VS_unisystem        = flags7 & 0b00000001;
    // cout << hex << (int)PRG_ROM_size << " PRGROM\n" << (int)PRG_RAM_size << " PRGRAM\n" << (int)mapper << " Mapper\n";

    /* Header format
    0-3: Constant $4E $45 $53 $1A ("NES" followed by MS-DOS end-of-file)
    4: Size of PRG ROM in 16 KB units
    5: Size of CHR ROM in 8 KB units (Value 0 means the board uses CHR RAM)
    6: Flags 6
    7: Flags 7
    8: Size of PRG RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
    9: Flags 9
    10: Flags 10 (unofficial)
    11-15: Zero filled
    */

    ROM.close();
}

char readFromMemory(int index) { // Handles memory mirroring
    if (index < 0x2000) {
        return memory[index % 0x800];
    }
    else if (index < 0x4000) {
        return memory[(index % 0x8) + 0x2000];
    }
    else if (index < 0xC000) {
        return memory[index];
    }
    else if (PRG_ROM_size == 1) { // Mirrors 0x8000-0xBFFF to 0xC000-0xFFFF
        return memory[index - 0x4000];
    }
    else { // PRG ROM is not mirrored
        return memory[index];
    }
}

void writeToMemory() { // Handles memory mirroring

}

void initStorage() {
    memset(memory, 0, sizeof(memory)); // Set array elements to zero

}

int main(int argc, char* argv[]) {
    std::cout << "Hello world!\n";
    std::string path;
    if (argc > 1) {
        path = argv[1];
        std::cout << "File argument: " << path << std::endl;
    }
    else {
        path = "mario.nes";
        std::cout << "Defaulting to " << path << std::endl;
    }
    loadRom(path); // Decode header into ROM object, parse header
    initStorage(); // Initialize main NES memory
}
