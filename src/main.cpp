#include "memory.h"
#include "rom.h"
#include "nes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>

const int DEFAULT_TEST_CASES = 10000;

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
    nes->cpu->logger.start("../test/cpuLog.txt");

    #ifdef DEBUG
    auto start = std::chrono::high_resolution_clock::now();
    #endif

    std::thread cpuThread(&CPU::start, nes->cpu);
    
    for (int i = 0; i < testCases; i++) {
        nes->cpu->cycle();
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    nes->cpu->stop(cpuThread);

    #ifdef DEBUG
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Finished in " << diff.count() << " seconds" << std::endl;
    #endif

    std::cout << "Successfully ended execution." << std::endl;

    nes->cpu->logger.stop();

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
    } else if (path == "ADDRESSING_MODES") {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                std::cout << addressingModeNames[CPU::getAddressingMode((i << 4) | j)] << ", ";
            }
            std::cout << std::endl;
        }
    } else if (path == "INSTRUCTIONS") {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                std::cout << opcodeNames[CPU::getInstruction((i << 4) | j)] << ", ";
            }
            std::cout << std::endl;
        }
    } else {
        ROM* rom = new ROM(); // Prepare to load ROM file
        rom->setPath(path); // Decode header into ROM object, parse header
        
        NES* nes = new NES();
        nes->loadROM(rom);
    }
    
    return 0;
}
