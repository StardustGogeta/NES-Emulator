#include "core_memory.h"
#include "rom.h"
#include "nes.h"
#include "display.h"
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

void printOpcodeProperties(std::string mapping(int)) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            std::cout << mapping((i << 4) | j) << ", ";
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]) {
    std::cout << "Hello world!\n";
    std::string path;
    
    if (argc > 1) {
        path = argv[1];
        std::cout << "File argument: " << path << std::endl;
    } else {
        path = "mario.nes";
        std::cout << "Defaulting to " << path << std::endl;
    }

    if (path == "NESTEST") {
        int testCases = argc > 2 ? atoi(argv[2]) : 0;
        runNesTest(testCases);
    } else if (path == "DISPLAY_TEST") {
        std::string testType = argc > 2 ? argv[2] : "rectangle";
        runDisplayTest(testType);
    } else if (path == "ADDRESSING_MODES") {
        printOpcodeProperties([] (int x) { return addressingModeNames[addressingModesByOpcode[x]]; });
    } else if (path == "INSTRUCTIONS") {
        printOpcodeProperties([] (int x) { return opcodeNames[instructionsByOpcode[x]]; });
    } else if (path == "LEGAL_OPCODES") {
        printOpcodeProperties([] (int x) { return std::to_string((int)legalOpcodes[x]); });
    } else {
        ROM* rom = new ROM(); // Prepare to load ROM file
        rom->setPath(path); // Decode header into ROM object, parse header
        
        NES* nes = new NES();
        nes->loadROM(rom);
    }

    return 0;
}
