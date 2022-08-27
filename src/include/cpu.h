#pragma once
#include "memory.h"
#include <cstdint>
#include <fstream>

enum addressingMode {
    // NUL is for opcodes that take no arguments
    IMM, ZP, ZPX, ZPY, IZX, IZY, ABS, ABX, ABY, IND, REL, NUL
};

enum instruction {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
    JSR, LDA, LDX, LDY, NOP, ORA, PHA, PHP, PLA, PLP, RTS, SEC, SED, SEI,
    STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA // TODO: Add the rest
};

class CPU {
    public:
        Memory* memory;

        CPU();
        void reset(Memory::addr_t pc=0xfffc);
        void cycle();
        void startLogging(std::string path);
        void stopLogging();
        uint8_t peek();
        uint16_t peekWord();
        uint8_t read();
        uint16_t readWord();

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
        Memory::addr_t pc;
        uint8_t sp, a, x, y;
        uint16_t cache, cache2;
        struct processorFlags {
            bool n : 1, v : 1, b1 : 1, b2 : 1, d : 1, i : 1, z : 1, c : 1;
        } p;
        int cycles; // Cycles left in instruction
        bool logging;
        std::ofstream logFile;

        addressingMode getAddressingMode(uint8_t opcode);
        Memory::addr_t getAddress(addressingMode mode);
        instruction getInstruction(uint8_t opcode);
        uint8_t processorStatus();
        void setProcessorStatus(uint8_t status);
        void setNZ(uint8_t val);
        void stackPush(uint8_t val);
        uint8_t stackPop();
        void runOpcode(uint8_t opcode);
};
