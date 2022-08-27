#include "cpu.h"
#include <bit>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstring>

CPU::CPU() {
    reset();
}

void CPU::reset(Memory::addr_t pc /* =0xfffc */) {
    this->pc = pc;
    sp = 0xfd;
    cycles = 0;
    memset(&p, 0, sizeof(p));
    p.d = p.b2 = 1;
    logging = false;
}

void CPU::startLogging(std::string path) {
    logFile.open(path, std::ios::out);
    logFile << std::hex << std::uppercase;
    logging = true;
    std::cout << "Started CPU logging." << std::endl;
}

void CPU::stopLogging() {
    logFile.close();
    std::cout << "Stopped CPU logging." << std::endl;
}

/*
    Reads the next byte at the program counter.
*/
uint8_t CPU::peek() {
    return memory->read(pc);
}

/*
    Reads the next word at the program counter.
*/
uint16_t CPU::peekWord() {
    return memory->readWord(pc);
}

/*
    Reads the next byte at the program counter and increments the PC.
*/
uint8_t CPU::read() {
    return memory->read(pc++);
}

/*
    Reads the next word at the program counter and increments the PC twice.
*/
uint16_t CPU::readWord() {
    uint16_t word = memory->readWord(pc);
    pc += 2;
    return word;
}

/*
    Returns the addressing mode for a particular opcode.
*/
addressingMode CPU::getAddressingMode(uint8_t opcode) {
    // TODO: Add the rest, convert to table indexing
    switch (opcode) {
        case 0x29:
        case 0x69:
        case 0xa0:
        case 0xa2:
        case 0xa9:
            return IMM;
        case 0x06:
        case 0x16:
        case 0x24:
        case 0x25:
        case 0x65:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xc6:
            return ZP;
        case 0x35:
        case 0x75:
        case 0x94:
        case 0x95:
        case 0xb4:
        case 0xb5:
        case 0xd6:
            return ZPX;
        case 0x96:
        case 0xb6:
            return ZPY;
        case 0x21:
        case 0x61:
        case 0x81:
        case 0xa1:
            return IZX;
        case 0x31:
        case 0x71:
        case 0x91:
        case 0xb1:
            return IZY;
        case 0x0e:
        case 0x20:
        case 0x2c:
        case 0x2d:
        case 0x4c:
        case 0x6d:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xce:
            return ABS;
        case 0x1e:
        case 0x3d:
        case 0x7d:
        case 0x9d:
        case 0xbc:
        case 0xbd:
        case 0xde:
            return ABX;
        case 0x39:
        case 0x79:
        case 0x99:
        case 0xb9:
        case 0xbe:
            return ABY;
        case 0x6c:
            return IND;
        case 0x10:
        case 0x30:
        case 0x50:
        case 0x70:
        case 0x90:
        case 0xb0:
        case 0xd0:
        case 0xf0:
            return REL;
        case 0x08:
        case 0x0a:
        case 0x18:
        case 0x28:
        case 0x38:
        case 0x48:
        case 0x58:
        case 0x60:
        case 0x68:
        case 0x78:
        case 0x88:
        case 0x98:
        case 0x9a:
        case 0xaa:
        case 0xb8:
        case 0xba:
        case 0xc8:
        case 0xca:
        case 0xd8:
        case 0xe8:
        case 0xea:
        case 0xf8:
            return NUL;
        default:
            throw std::runtime_error("Unsupported opcode in getAddressingMode.");
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
            return pc++;
        case ZP:
            return read();
        case ZPX:
            return (read() + x) % 0x100;
        case ZPY:
            return (read() + y) % 0x100;
        case IZX:
            return memory->readWord((read() + x) % 0x100);
        case IZY:
            return (memory->readWord(read()) << 8) | y;
        case ABS:
            return readWord();
        case ABX:
            return readWord() + x;
        case ABY:
            return readWord() + y;
        case IND:
            return memory->readWord(readWord());
        case REL:
            // TODO: Check if this has a small error with the PC
            return std::bit_cast<int8_t>(read()) + pc;
        default:
            throw std::runtime_error("Unsupported opcode in getAddress.");
    }
}

instruction CPU::getInstruction(uint8_t opcode) {
    // TODO: Add the rest, convert to table indexing
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
        case 0x18:
            return CLC;
        case 0xd8:
            return CLD;
        case 0x58:
            return CLI;
        case 0xb8:
            return CLV;
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
        case 0x4c:
        case 0x6c:
            return JMP;
        case 0x20:
            return JSR;
        case 0xa1:
        case 0xa5:
        case 0xa9:
        case 0xad:
        case 0xb1:
        case 0xb5:
        case 0xb9:
        case 0xbd:
            return LDA;
        case 0xa2:
        case 0xa6:
        case 0xae:
        case 0xb6:
        case 0xbe:
            return LDX;
        case 0xa0:
        case 0xa4:
        case 0xac:
        case 0xb4:
        case 0xbc:
            return LDY;
        case 0xea:
            return NOP;
        case 0x48:
            return PHA;
        case 0x08:
            return PHP;
        case 0x68:
            return PLA;
        case 0x28:
            return PLP;
        case 0x60:
            return RTS;
        case 0x38:
            return SEC;
        case 0xf8:
            return SED;
        case 0x78:
            return SEI;
        case 0x81:
        case 0x85:
        case 0x8d:
        case 0x91:
        case 0x95:
        case 0x99:
        case 0x9d:
            return STA;
        case 0x86:
        case 0x8e:
        case 0x96:
            return STX;
        case 0x84:
        case 0x8c:
        case 0x94:
            return STY;
        case 0xaa:
            return TAX;
        case 0xa8:
            return TAY;
        case 0xba:
            return TSX;
        case 0x8a:
            return TXA;
        case 0x9a:
            return TXS;
        case 0x98:
            return TYA;
        default:
            throw std::runtime_error("Unsupported opcode in getInstruction.");
    }
}

void CPU::setNZ(uint8_t val) {
    p.n = (val & 0x80) > 0;
    p.z = val == 0;
}

void CPU::stackPush(uint8_t val) {
    memory->writeDirect(0x100 + (sp--), val);
}

uint8_t CPU::stackPop() {
    return memory->readDirect(0x100 + (++sp));
}

uint8_t CPU::processorStatus() {
    return (((((((((((((p.n << 1) | p.v) << 1) | p.b1) << 1) | p.b2) << 1) | p.d) << 1) | p.i) << 1) | p.z) << 1) | p.c;
}

void CPU::setProcessorStatus(uint8_t status) {
    p.n = (status & 0x80) > 0;
    p.v = (status & 0x40) > 0;
    p.b1= (status & 0x20) > 0;
    p.b2= (status & 0x10) > 0;
    p.d = (status & 0x08) > 0;
    p.i = (status & 0x04) > 0;
    p.z = (status & 0x02) > 0;
    p.c = (status & 0x01) > 0;
}

void CPU::runOpcode(uint8_t opcode) {
    #ifdef DEBUG
    std::cout << "Trying to run opcode 0x" << std::hex << (int)opcode << " at position 0x" << pc - 1 << std::dec << std::endl;
    #endif

    // Write opcodes to a log file
    if (logging) {
        logFile << std::setfill('0') << std::setw(4) << pc - 1 << "  " << std::setfill('0') << std::setw(2) << (int)opcode << std::endl;
    }

    addressingMode mode = getAddressingMode(opcode);
    instruction inst = getInstruction(opcode);
    
    Memory::addr_t addr = 0;
    uint8_t argument; // , arg_word;
    
    if (mode != NUL) {
        addr = getAddress(mode);
        // Read up to two bytes at the given address
        // TODO: Optimize by only reading argument if specific instruction requires it
        argument = memory->read(addr);
        // arg_word = (memory->read(addr + 1) << 8) | argument;
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
        case BRK: {
            // TODO: Make sure this all works as intended
            pc += 2;
            stackPush((pc & 0xff00) >> 8);
            stackPush(pc & 0xff);
            p.b1 = p.b2 = 1;
            stackPush(processorStatus());
            p.i = 1;
            pc = 0xfffe;
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
        case CLC:
            p.c = 0;
            break;
        case CLD:
            p.d = 0;
            break;
        case CLI:
            p.i = 0;
            break;
        case CLV:
            p.v = 0;
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
        case JMP:
            pc = addr;
            break;
        case JSR:
            stackPush(((--pc) & 0xff00) >> 8);
            stackPush(pc & 0xff);
            pc = addr;
            break;
        case LDA:
            a = argument;
            setNZ(a);
            break;
        case LDX:
            x = argument;
            setNZ(x);
            break;
        case LDY:
            y = argument;
            setNZ(y);
            break;
        case NOP:
            break;
        case PHA:
            stackPush(a);
            break;
        case PHP:
            stackPush(processorStatus());
            break;
        case PLA:
            a = stackPop();
            setNZ(a);
            break;
        case PLP:
            setProcessorStatus(stackPop());
            break;
        case RTS:{
            pc = (stackPop() | (stackPop() << 8)) + 1;
            }
            break;
        case SEC:
            p.c = 1;
            break;
        case SED:
            p.d = 1;
            break;
        case SEI:
            p.i = 1;
            break;
        case STA:
            memory->write(addr, a);
            break;
        case STX:
            memory->write(addr, x);
            break;
        case STY:
            memory->write(addr, y);
            break;
        case TAX:
            x = a;
            setNZ(x);
            break;
        case TAY:
            y = a;
            setNZ(y);
            break;
        case TSX:
            x = sp;
            setNZ(x);
            break;
        case TXA:
            a = x;
            setNZ(a);
            break;
        case TXS:
            sp = x;
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
        runOpcode(read());
    }
    cycles--;
}
