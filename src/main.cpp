#include <iostream>
#include <cstring>
#include "rom.h"

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
