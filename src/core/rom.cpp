#include "rom.h"
#include "core_memory.h"
#include "memory_factory.h"
#include <fstream>
#include <print>

ROM::ROM() {
    persistentMemory = trainer = fourScreenVRAM = nes2 =
        playchoice10 = VS_unisystem = false;
    mapper = PRG_ROM_size = CHR_ROM_size = PRG_RAM_size =
        flags6 = flags7 = flags9 = flags10 = 0;
}

void ROM::setPath(std::string newPath) {
    path = newPath;
}

void ROM::parseHeader() {
    // Open the file and go straight to the end
    std::ifstream romFile(path, std::ios::ate | std::ios::binary);
    std::streamoff romSize = romFile.tellg(); // Find file length
    std::println("The file size is {} bytes.", romSize);
    
    std::array<uint8_t, 16> header;
    romFile.seekg(0); // Reset to start of file
    for (int x = 0; x < 16; x++) {
        romFile >> std::hex >> header[x];
    }

    // Check that the first 4 characters are "NES\n"
    if (!std::equal(header.begin(), header.begin() + 4, "NES\x1A")) {
        std::println(stderr, "The NES file header is invalid.");
    } else {
        /*
            Header Format:
            0-3: Constant $4E $45 $53 $1A ("NES" followed by MS-DOS EOF)
            4: Size of PRG-ROM in 16 KB units
            5: Size of CHR-ROM in 8 KB units (Value 0 means the board uses CHR-RAM)
            6: Flags 6
            7: Flags 7
            8: Size of PRG-RAM in 8 KB units (Value 0 infers 8 KB for compatibility; see PRG RAM circuit)
            9: Flags 9
            10: Flags 10 (unofficial)
            11-15: Zero filled
        */

        // Identify all header components
        PRG_ROM_size = header[4];
        CHR_ROM_size = header[5];
        flags6 = header[6];
        flags7 = header[7];
        PRG_RAM_size = header[8];
        flags9 = header[9];
        flags10 = header[10];

        // Decode flags 6 and 7
        mirroring           = flags6 & 0b00000001 ? "vertical" : "horizontal";
        persistentMemory    = (flags6 & 0b00000010) > 0;
        trainer             = (flags6 & 0b00000100) > 0;
        fourScreenVRAM      = (flags6 & 0b00001000) > 0;
        mapper              = (flags6 >> 4) + (flags7 & 0b11110000);
        nes2                = (flags7 & 0b00001100) == 0b1000; // NES 2.0 not fully supported
        playchoice10        = (flags7 & 0b00000010) > 0;
        VS_unisystem        = (flags7 & 0b00000001) > 0;
        std::println("{} PRGROM\n{} PRGRAM\n{} CHRROM\n{} Mapper", PRG_ROM_size, PRG_RAM_size, CHR_ROM_size, mapper);
    }

    romFile.close();
}

/*
    Loads PRG-ROM from a file into memory and returns the object created.
*/
std::unique_ptr<CoreMemory> ROM::loadIntoMemory() {
    this->parseHeader();

    std::unique_ptr<CoreMemory> memory = MemoryFactory::create(mapper);

    memory->set_PRG_ROM_size(PRG_ROM_size);

    std::ifstream romFile;
    romFile.open(path, std::ios::binary);
    romFile >> std::noskipws;

    romFile.seekg(16); // Skip to the end of the header

    if (trainer) {
        romFile.seekg(528); // If there is a trainer section, skip it
    }

    // Load PRG-ROM into memory
    int PRG_ROM_size_bytes = PRG_ROM_size * 0x4000;
    uint8_t byte;
    for (int i = 0; i < PRG_ROM_size_bytes; i++) {
        romFile >> byte;
        memory->writeDirect(0x8000 + i, byte);
    }

    romFile.close();

    return memory;
}
