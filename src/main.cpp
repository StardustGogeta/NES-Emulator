#include "memory.h"
#include "rom.h"
#include "nes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

const int DEFAULT_TEST_CASES = 360;

bool runNesTest(int testCases) {
    std::cout << "Running nestest...\n";
    if (!testCases) {
        testCases = DEFAULT_TEST_CASES;
    }

    ROM* rom = new ROM();
    rom->setPath("../test/nestest.nes");

    NES* nes = new NES();
    nes->loadROM(rom);
    nes->cpu->reset(0xc000); // Set initial program counter
    nes->cpu->startLogging("../test/cpuLog.txt");

    for (int i = 0; i < testCases; i++) {
        nes->cpu->cycle();
    }

    nes->cpu->stopLogging();

    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "Hello world!\n";
    std::string path;
    int testCases = 0;

    if (argc > 1) {
        path = argv[1];
        std::cout << "File argument: " << path << std::endl;

        if (argc > 2) {
            testCases = atoi(argv[2]);
        }
    }
    else {
        path = "mario.nes";
        std::cout << "Defaulting to " << path << std::endl;
    }

    if (path == "nestest") {
        runNesTest(testCases);
    } else {
        ROM* rom = new ROM(); // Prepare to load ROM file
        rom->setPath(path); // Decode header into ROM object, parse header
        
        NES* nes = new NES();
        nes->loadROM(rom);
    }
    
    return 0;
}
