#include <iostream>
#include <cstring>
#include "memory.h"
#include "rom.h"
#include "nes.h"

bool runNesTest() {
    std::cout << "Running nestest...\n";

    ROM* rom = new ROM();
    rom->setPath("../nestest.nes");

    NES* nes = new NES();
    nes->loadROM(rom);
    nes->cpu->reset(0xc000); // Set initial program counter

    for (int i = 0; i < 10; i++) {
        uint8_t val = nes->cpu->peek();
        std::cout << "Byte: " << std::hex << (int)val << std::dec << std::endl;
        nes->cpu->cycle();
    }

    return true;
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

    if (path == "nestest") {
        runNesTest();
    } else {
        ROM* rom = new ROM(); // Prepare to load ROM file
        rom->setPath(path); // Decode header into ROM object, parse header
        
        NES* nes = new NES();
        nes->loadROM(rom);
    }
    
    return 0;
}
