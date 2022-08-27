#pragma once
#include "memory.h"
#include <cstdint>

enum addressingMode {
    // NUL is for opcodes that take no arguments
    IMM, ZP, ZPX, ZPY, IZX, IZY, ABS, ABX, ABY, IND, REL, NUL
};

enum instruction {
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, DEC, DEX, DEY, INX, INY, JMP, JSR, LDA, LDX, LDY, NOP, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TXA, TYA // TODO: Add the rest
};

class CPU {
    public:
        Memory* memory;

        CPU();
        void reset(Memory::addr_t pc=0xfffc);
        void cycle();
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
        struct processorFlags {
            bool n : 1, v : 1, b1 : 1, b2 : 1, d : 1, i : 1, z : 1, c : 1;
        } p;
        int cycles; // Cycles left in instruction
        // Current instruction
        // Addressing mode

        addressingMode getAddressingMode(uint8_t opcode);
        Memory::addr_t getAddress(addressingMode mode);
        instruction getInstruction(uint8_t opcode);
        uint8_t processorStatus();
        void setNZ(uint8_t val);
        void stackPush(uint8_t val);
        void runOpcode(uint8_t opcode);
};
