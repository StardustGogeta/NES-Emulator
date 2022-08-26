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
        case 0x29:
        case 0x69:
            return IMM;
        case 0x06:
        case 0x16:
        case 0x24:
        case 0x25:
        case 0x65:
        case 0xc6:
            return ZP;
        case 0x35:
        case 0x75:
        case 0xd6:
            return ZPX;
        case 0x21:
        case 0x61:
            return IZX;
        case 0x31:
        case 0x71:
            return IZY;
        case 0x0e:
        case 0x2c:
        case 0x2d:
        case 0x6d:
        case 0xce:
            return ABS;
        case 0x1e:
        case 0x3d:
        case 0x7d:
        case 0xde:
            return ABX;
        case 0x39:
        case 0x79:
            return ABY;
        case 0x10:
        case 0x30:
        case 0x50:
        case 0x70:
        case 0x90:
        case 0xb0:
        case 0xd0:
        case 0xf0:
            return REL;
        case 0x0a:
        case 0x88:
        case 0x98:
        case 0xaa:
        case 0xc8:
        case 0xca:
        case 0xe8:
        case 0xea:
            return NUL;
        default:
            throw;
    }
}

/*
    Fetches the address of the operand for an opcode based on addressing mode.
    Assumes the program counter is at the start of the arguments list.
    Moves the program counter to the end of the arguments list.
*/
Memory::addr_t CPU::getAddress(addressingMode mode) {
    switch (mode) {
        case IMM:
        case IND:
            return pc;
        case ZP:
            return readNext();
        case ZPX:
            return (readNext() + x) % 0x100;
        case ZPY:
            return (readNext() + y) % 0x100;
        case IZX:
            return memory->readWord((readNext() + x) % 0x100);
        case IZY:
            return (memory->readWord(readNext()) << 8) | y;
        case ABS:
            return readNextWord();
        case ABX:
            return readNextWord() + x;
        case ABY:
            return readNextWord() + y;
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
        case 0x61:
        case 0x65:
        case 0x69:
        case 0x6d:
        case 0x71:
        case 0x75:
        case 0x79:
        case 0x7d:
            return ADC;
        case 0x21:
        case 0x25:
        case 0x29:
        case 0x2d:
        case 0x31:
        case 0x35:
        case 0x39:
        case 0x3d:
            return AND;
        case 0x06:
        case 0x0a:
        case 0x0e:
        case 0x16:
        case 0x1e:
            return ASL;
        case 0x90:
            return BCC;
        case 0xb0:
            return BCS;
        case 0xf0:
            return BEQ;
        case 0x24:
        case 0x2c:
            return BIT;
        case 0x30:
            return BMI;
        case 0xd0:
            return BNE;
        case 0x10:
            return BPL;
        case 0x50:
            return BVC;
        case 0x70:
            return BVS;
        case 0xc6:
        case 0xce:
        case 0xd6:
        case 0xde:
            return DEC;
        case 0xca:
            return DEX;
        case 0x88:
            return DEY;
        case 0xe8:
            return INX;
        case 0xc8:
            return INY;
        case 0xea:
            return NOP;
        case 0xaa:
            return TAX;
        case 0xa8:
            return TAY;
        case 0x8a:
            return TXA;
        case 0x98:
            return TYA;
        default:
            throw;
    }
}

void CPU::setNZ(uint8_t val) {
    p.n = val & 0x80 > 0;
    p.z = val == 0;
}

void CPU::runOpcode(uint8_t opcode) {
    addressingMode mode = getAddressingMode(opcode);
    instruction inst = getInstruction(opcode);
    
    Memory::addr_t addr;
    uint8_t argument, arg_word;
    
    if (mode != NUL) {
        addr = getAddress(mode);
        // Read up to two bytes at the given address
        // TODO: Optimize by only reading argument if specific instruction requires it
        argument = memory->read(addr);
        arg_word = (memory->read(addr + 1) << 8) | argument;
    } else {
        // If an opcode normally takes arguments, then the no-arg instruction uses the accumulator
        argument = a;
    }

    switch (inst) {
        case ADC: {
            // Use a type large enough to detect overflow
            uint16_t carry = p.c;
            p.c = carry + a + argument > 0xff;
            a += argument + carry;
            setNZ(a);
            p.v = p.c ^ carry;
            }
            break;
        case AND:
            a &= argument;
            setNZ(a);
            break;
        case ASL:
            // TODO: Make sure this is correct interpretation
            p.c = (a & 0x80) > 0;
            a = argument << 1;
            p.n = (a & 0x80) > 0;
            p.z = a == 0;
            break;
        case BCC:
            if (!p.c) {
                pc = addr;
            }
            break;
        case BCS:
            if (p.c) {
                pc = addr;
            }
            break;
        case BEQ:
            if (p.z) {
                pc = addr;
            }
            break;
        case BIT:
            p.n = (argument & 0x80) > 0;
            p.v = (argument & 0x40) > 0;
            p.z = (a | argument) == 0;
            break;
        case BMI:
            if (p.n) {
                pc = addr;
            }
            break;
        case BNE:
            if (!p.z) {
                pc = addr;
            }
            break;
        case BPL:
            if (!p.n) {
                pc = addr;
            }
            break;
        case BVC:
            if (!p.v) {
                pc = addr;
            }
            break;
        case BVS:
            if (p.v) {
                pc = addr;
            }
            break;
        case DEC:
            memory->write(addr, argument - 1);
            setNZ(argument - 1);
            break;
        case DEX:
            x--;
            setNZ(x);
            break;
        case DEY:
            y--;
            setNZ(y);
            break;
        case INX:
            x++;
            setNZ(x);
            break;
        case INY:
            y++;
            setNZ(y);
            break;
        case NOP:
            break;
        case TAX:
            x = a;
            setNZ(x);
            break;
        case TAY:
            y = a;
            setNZ(y);
            break;
        case TXA:
            a = x;
            setNZ(a);
            break;
        case TYA:
            a = y;
            setNZ(a);
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
