#pragma once
#include "memory.h"
#include <cstdint>

class CPU {
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
        uint16_t pc;
        uint8_t sp, a, x, y, p;
};
