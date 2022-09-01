#include "core_memory.h"
#include "rom.h"
#include "nes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>

#include "SDL.h"

const int DEFAULT_NESTEST_CASES = 26555;

auto now() {
    return std::chrono::high_resolution_clock::now();
}
 
auto awake_time() {
    using std::chrono::operator""us;
    return now() + 50us;
}

bool runNesTest(int testCases) {
    std::cout << "Running nestest...\n";
    if (!testCases) {
        testCases = DEFAULT_NESTEST_CASES;
    }

    ROM* rom = new ROM();
    rom->setPath("../test/nestest.nes");

    NES* nes = new NES();
    nes->loadROM(rom);
    
    for (int i = 0; i < 0x20; i++) { // Set APU registers to 0xff
        nes->memory->write(0x4000 | i, 0xff);
    }

    #ifdef DEBUG
    auto start = now();
    #endif

    // We queue up a couple NOPs at the beginning to synchronize clocks with nestest
    nes->cpu->runOpcode(0x64);
    nes->cpu->runOpcode(0x74);

    nes->cpu->setPC(0xc000); // Set initial program counter

    nes->cpu->logger.start("../test/cpuLog.txt");

    std::thread cpuThread(&CPU::start, nes->cpu);
    
    // Wait until the CPU starts up.
    while (!nes->cpu->checkRunning()) {
        std::this_thread::yield();
    }

    for (int i = 0; i < testCases; i++) {
        nes->cpu->cycle();
        
        // We can artificially limit processor speed by sleeping the main thread:
        // std::this_thread::sleep_until(awake_time());
    }

    nes->cpu->stop(cpuThread);

    #ifdef DEBUG
    auto end = now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Finished in " << diff.count() << " seconds" << std::endl;
    #endif

    std::cout << "Successfully ended execution." << std::endl;

    nes->cpu->logger.stop();

    return true;
}

int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);
    
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
    } else if (path == "LEGAL_OPCODES") {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                std::cout << CPU::isLegalOpcode((i << 4) | j) << ", ";
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
