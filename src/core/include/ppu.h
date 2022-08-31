#pragma once
#include <condition_variable>

class CPU;

class PPU {
    friend class CPU;

    public:
        PPU(CPU& cpu);
        void start();
        void cycle();
        void cycles(int n);
        bool checkRunning();
        
    private:
        // For synchronizing with CPU clock
        std::condition_variable cpuToPpuCV;
        std::mutex cpuToPpuMutex;
        int cyclesExecuted, scanline, cyclesOnLine;
        CPU& cpu;
        bool running;
};
