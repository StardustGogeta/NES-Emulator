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
    loadRom(path); // Decode header into ROM object, parse header
    
    Memory* memory = new Memory(); // Initialize main NES memory
    memory->clear();
}
