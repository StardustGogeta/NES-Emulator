#include "ppu.h"
#include "cpu.h"

PPU::PPU(CPU& cpu) : cyclesExecuted(0), cpu(cpu) { }

void PPU::start() {
    running = true;

    while (running) {
        std::unique_lock lck(cpuToPpuMutex);
        cpuToPpuCV.wait(lck);
        
        // We want 3 PPU cycles for every CPU cycle.
        for (int i = 0; i < 3; i++) {
            cycle();
        }

        cpu.ppuToCpuCV.notify_one();
    }
}

void PPU::cycle() {
    cyclesExecuted++;
    scanline = cyclesExecuted / 341;
    cyclesOnLine = cyclesExecuted % 341;
}

void PPU::cycles(int n) {
    for (int i = 0; i < n; i++) {
        cycle();
    }
}

bool PPU::checkRunning() {
    return running;
}
