#pragma once
#include <string>

enum addressingMode {
    // NUL is for opcodes that take no arguments
    // XXX is for unimplemented / unknown
    IMM, ZPG, ZPX, ZPY, IZX, IZY, ABS, ABX, ABY, IND, REL, NUL, XXX
};

enum instruction {
    // YYY is for unimplemented / unknown
    ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC,
    CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, INC, INX, INY, JMP,
    JSR, LDA, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, ROR, RTI,
    RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA, YYY // TODO: Add the rest
};

extern const std::string addressingModeNames[];
extern const std::string opcodeNames[];
extern const int addressingModeReadCount[];
extern const addressingMode addressingModesByOpcode[];
extern const instruction instructionsByOpcode[];
extern const bool legalOpcodes[];
