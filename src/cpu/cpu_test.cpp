#include "nes.h"
#include <iostream>
#include <chrono>

const int DEFAULT_NESTEST_CASES = 26555;

auto now() {
    return std::chrono::high_resolution_clock::now();
}
 
auto awake_time() {
    using std::chrono::operator""us;
    return now() + 50us;
}

void runNesTest(int testCases) {
    std::cout << "Running nestest...\n";
    if (!testCases) {
        testCases = DEFAULT_NESTEST_CASES;
    }

    ROM rom;
    rom.setPath("../test/nestest.nes");

    std::unique_ptr<NES> nes = std::make_unique<NES>();
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

    nes->cpu->setPC((addr_t)0xc000); // Override initial program counter

    nes->cpu->logger.start("../test/nestestLog.txt");

    std::thread cpuThread(&CPU::start, nes->cpu.get());
    
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
}

void runBlarggCpuTest5Official() {
    std::cout << "Running blargg CPU test 5 (official opcodes only)...\n";

    ROM rom;
    rom.setPath("../test/blargg_cpu_test5_official.nes");

    NES* nes = new NES();
    nes->loadROM(rom);

    nes->cpu->runOpcode(0x48, true); // Execute PHA x 3
    nes->cpu->runOpcode(0x48, true); // Synchronizes stack pointer with Nintendulator
    nes->cpu->runOpcode(0x48, true); // Must keep PPU cycles from logging

    nes->cpu->setPC();

    nes->cpu->logger.start("../test/blargg5Log.txt", true);

    #ifdef DEBUG
    auto start = now();
    #endif

    std::thread cpuThread(&CPU::start, nes->cpu.get());
    
    // Wait until the CPU starts up.
    while (!nes->cpu->checkRunning()) {
        std::this_thread::yield();
    }

    for (int i = 0; i < 22000; i++) {
        nes->cpu->cycle();
    }

    nes->cpu->stop(cpuThread);

    nes->cpu->logger.stop();

    #ifdef DEBUG
    auto end = now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Finished in " << diff.count() << " seconds" << std::endl;
    #endif

    std::cout << "Successfully ended execution." << std::endl;

    std::cout << "Error log output:" << std::endl;
    for (int i = 0; i < 20; i++) {
        std::cout << std::hex << (int)nes->memory->read(0x6000 | i);
    }
    std::cout << std::endl;
}

void printOpcodeProperties(std::string mapping(int)) {
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            std::cout << mapping((i << 4) | j) << ", ";
        }
        std::cout << std::endl;
    }
}

void runCpuTest(std::string testName) {
    if (testName == "nestest") {
        runNesTest(0);
    } else if (testName == "blargg5official") {
        runBlarggCpuTest5Official();
    } else if (testName == "addressing_modes") {
        printOpcodeProperties([] (int x) { return addressingModeNames[addressingModesByOpcode[x]]; });
    } else if (testName == "instructions") {
        printOpcodeProperties([] (int x) { return opcodeNames[instructionsByOpcode[x]]; });
    } else if (testName == "legal_opcodes") {
        printOpcodeProperties([] (int x) { return std::to_string((int)legalOpcodes[x]); });
    }
}
