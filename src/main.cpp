#include <iostream>
#include <cstring>
#include "memory.h"
#include "rom.h"

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

    ROM* rom = new ROM(); // Prepare to load ROM file
    rom->setPath(path); // Decode header into ROM object, parse header
    rom->parseHeader();
    
    Memory* memory = new Memory(rom->PRG_ROM_size); // Initialize main NES memory
    memory->clear();

    rom->loadIntoMemory(memory);
}
