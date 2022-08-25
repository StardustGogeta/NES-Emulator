#pragma once
#include "memory.h"
#include <cstdint>

enum addressingMode {
    IMM, ZP, ZPX, ZPY, IZX, IZY, ABS, ABX, ABY, IND, REL
};

enum instruction {
    ADC, AND, ASL, BIT, BRK // TODO: Add the rest
};

class CPU {
    public:
        Memory* memory;

        CPU();
        void reset();
        void cycle();

    private:
        /*
            Program counter stores position for execution
            Stack pointer stores offset from 0x100, grows downward
            Accumulator is used for arithmetic
            X and Y registers used for storage
            P register stores processor flags
            From left to right:
                Carry, zero, interrupt disable, BCD mode,
                BRK command, unused, overflow, negative
        */
        Memory::addr_t pc;
        uint8_t sp, a, x, y, p;
        int cycles; // Cycles left in instruction
        // Current instruction
        // Addressing mode

        uint8_t readNext();
        uint16_t readNextWord();
        addressingMode getAddressingMode(uint8_t opcode);
        uint8_t getArgument(addressingMode mode);
        instruction getInstruction(uint8_t opcode);
        void runOpcode(uint8_t opcode);
};
