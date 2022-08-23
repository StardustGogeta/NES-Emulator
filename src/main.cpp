#include <iostream>
#include <cstring>
#include "memory.h"
#include "rom.h"
#include "nes.h"

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
    
    NES* nes = new NES();
    nes->loadROM(rom);
}
