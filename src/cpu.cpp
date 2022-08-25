#include "cpu.h"
#include <bit>

CPU::CPU() {
    reset();
}

void CPU::reset() {
    pc = 0xfffc;
    cycles = 0;
}

/*
    Reads the next byte at the program counter and increments the PC.
*/
uint8_t CPU::readNext() {
    return memory->read(pc++);
}

/*
    Reads the next byte at the program counter and increments the PC twice.
*/
uint16_t CPU::readNextWord() {
    uint16_t word = memory->readWord(pc);
    pc += 2;
    return word;
}

/*
    Returns the addressing mode for a particular opcode.
*/
addressingMode CPU::getAddressingMode(uint8_t opcode) {
    // TODO: Add the rest
    switch (opcode) {
        case 0x21:
            return IZX;
        case 0x25:
            return ZP;
        case 0x29:
            return IMM;
        case 0x2d:
            return ABS;
        case 0x31:
            return IZY;
        case 0x35:
            return ZPX;
        case 0x39:
            return ABY;
        case 0x3d:
            return ABX;
        default:
            throw;
    }
}

/*
    Fetches the argument for an opcode based on addressing mode.
    Assumes the program counter is at the start of the arguments list. 
*/
uint8_t CPU::getArgument(addressingMode mode) {
    switch (mode) {
        case IMM:
            return readNext();
        case ZP:
            return memory->read(readNext());
        case ZPX:
            return memory->read((readNext() + x) % 0x100);
        case ZPY:
            return memory->read((readNext() + y) % 0x100);
        case IZX:
            return memory->read(memory->readWord((readNext() + x) % 0x100));
        case IZY:
            return memory->read((memory->readWord(readNext()) << 8) | y);
        case ABS:
            return memory->read(readNextWord());
        case ABX:
            return memory->read(readNextWord() + x);
        case ABY:
            return memory->read(readNextWord() + y);
        case IND:
            return readNextWord();
        case REL:
            // TODO: Check if this has a small error with the PC
            return std::bit_cast<int8_t>(readNext()) + pc;
        default:
            throw;
    }
}

instruction CPU::getInstruction(uint8_t opcode) {
    // TODO: Add the rest
    switch (opcode) {
        case 0x21:
        case 0x25:
        case 0x29:
        case 0x2d:
        case 0x31:
        case 0x35:
        case 0x39:
        case 0x3d:
            return AND;
        default:
            throw;
    }
}

void CPU::runOpcode(uint8_t opcode) {
    addressingMode mode = getAddressingMode(opcode);
    uint8_t argument = getArgument(mode);
    instruction inst = getInstruction(opcode);
    switch (inst) {
        case AND:
            a &= argument;
            break;
    }
    // TODO: Get actual cycle count
    cycles = 2;
}

/*
    Performs one cycle of the CPU.
*/
void CPU::cycle() {
    if (cycles == 0) {
        // New opcode begins
        runOpcode(readNext());
    }
    cycles--;
}
