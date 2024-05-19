#pragma once
#include "core_memory.h"
#include <condition_variable>

class CPU;

class PPU {
    friend class NES;
    friend class CPU;

    public:
        PPU(CPU& cpu);

        uint8_t readRegister(addr_t address);
        void writeRegister(addr_t address, uint8_t data);

        void start();
        void cycle();
        void cycles(int n);
        bool checkRunning();
        
    private:
        bool renderingEnabled();
        bool oddFrame = false;

        std::shared_ptr<CoreMemory> memory;

        /*
            Control and status registers for the PPU.
        */
        std::array<uint8_t, 8> registers {};

        // Background-rendering shift registers
        uint8_t bg8sr0, bg8sr1;
        uint16_t bg16sr0, bg16sr1;

        // For synchronizing with CPU clock
        std::condition_variable cpuToPpuCV;
        std::mutex cpuToPpuMutex;
        int cyclesExecuted, scanline, cyclesOnLine;
        CPU& cpu;
        bool running;
};
