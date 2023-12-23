#pragma once
#include "core_memory.h"
#include "ppu.h"
#include "opcodes.h"
#include <cstdint>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <semaphore>

class CPU {
    friend class NES;
    friend class PPU;

    public:
        class Logger {
            friend CPU;

            public:
                void start(std::string path, bool reversePPU=false);
                void stop();

            private:
                Logger(CPU& cpu);
                void logOpcode(
                    uint8_t opcode,
                    addressingMode mode, 
                    instruction inst
                );
                void logArgsAndRegisters(
                    addressingMode mode,
                    instruction inst,
                    addr_t addr,
                    uint8_t argument
                );
                void logPPU(int scanline, int cyclesOnLine);
                void logCycles(int cyclesExecuted);
                bool logging, reversePPU;
                std::ofstream logFile;
                CPU& cpu;
        };

        std::shared_ptr<PPU> ppu;
        Logger logger;

        CPU();
        void reset();
        void setPC(bool ignoreCycles=false);
        void setPC(addr_t pc);
        void start();
        void stop(std::thread& t);
        void kill(std::thread& t);
        void cycle();
        uint8_t peek();
        uint16_t peekWord();
        uint8_t read();
        uint16_t readWord();
        bool checkRunning();
        void runOpcode(uint8_t opcode, bool ignoreCycles=false);

        static addressingMode getAddressingMode(uint8_t opcode);
        static instruction getInstruction(uint8_t opcode);
        static int getCycleCount(uint8_t opcode, int cycleOffset);
        static bool isLegalOpcode(uint8_t opcode);

    private:
        /*
            Program counter stores position for execution
            Stack pointer stores offset from 0x100, grows downward
            Accumulator is used for arithmetic
            X and Y registers used for storage
            P register stores processor flags
            From right to left:
                Carry, zero, interrupt disable, BCD mode,
                BRK command (unused), unused, overflow, negative
            Also written: NVbbDIZC
        */
        std::shared_ptr<CoreMemory> memory;
        addr_t pc;
        uint8_t sp, a, x, y;
        uint16_t cache, precache;
        struct processorFlags {
            bool n : 1, v : 1, b1 : 1, b2 : 1, d : 1, i : 1, z : 1, c : 1;
        } p;
        
        addr_t getAddress(addressingMode mode);
        uint8_t processorStatus();
        void setProcessorStatus(uint8_t status);
        void setNZ(uint8_t val);
        void stackPush(uint8_t val);
        uint8_t stackPop();
        int runInstruction(
            addressingMode mode,
            instruction inst,
            addr_t addr,
            uint8_t argument,
            bool extraCycles
        );
        bool waitForCycles(int n);
        bool waitForCycle();
        std::atomic<bool> running;
        std::atomic<bool> notDone;

        int maxCycles;
        int cyclesRequested; // uses cycleStatusMutex
        std::atomic<int> cyclesExecuted;
        std::mutex cycleStatusMutex;
        std::condition_variable cycleStatusCV;

        // For synchronizing with PPU clock
        std::condition_variable ppuToCpuCV;
        std::mutex ppuToCpuMutex;

        CPU(const CPU&) = delete;
        CPU& operator=(const CPU&) = delete;
};
