#include "cpu.h"
#include <bit>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstring>

// Pad output to given width using zeros
#define ZPAD2 std::setfill('0') << std::setw(2)
#define ZPAD4 std::setfill('0') << std::setw(4)

// Number of bytes that must be read after each addressing mode
int addressingModeReadCount[] = {
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 0
};

std::string opcodeNames[] = {
    "ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC",
    "CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP",
    "JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI",
    "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA"
};

CPU::CPU() : logger(*this) {
    reset();
}

CPU::Logger::Logger(CPU& cpu) : cpu(cpu) { }

void CPU::reset(Memory::addr_t pc /* =0xfffc */) {
    this->pc = pc;
    sp = 0xfd;
    cycles = a = x = y = 0;
    memset(&p, 0, sizeof(p));
    p.b1 = p.i = 1;
}

void CPU::Logger::start(std::string path) {
    logFile.open(path, std::ios::out);
    logFile << std::hex << std::uppercase;
    logging = true;
    std::cout << "Started CPU logging." << std::endl;
}

void CPU::Logger::stop() {
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
        case 0x09:
        case 0x29:
        case 0x49:
        case 0x69:
        case 0xa0:
        case 0xa2:
        case 0xa9:
        case 0xc0:
        case 0xc9:
        case 0xe0:
        case 0xe9:
            return IMM;
        case 0x05:
        case 0x06:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x45:
        case 0x46:
        case 0x65:
        case 0x66:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0xa4:
        case 0xa5:
        case 0xa6:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xe4:
        case 0xe5:
        case 0xe6:
            return ZP;
        case 0x15:
        case 0x16:
        case 0x35:
        case 0x36:
        case 0x55:
        case 0x56:
        case 0x75:
        case 0x76:
        case 0x94:
        case 0x95:
        case 0xb4:
        case 0xb5:
        case 0xd5:
        case 0xd6:
        case 0xf5:
        case 0xf6:
            return ZPX;
        case 0x96:
        case 0xb6:
            return ZPY;
        case 0x01:
        case 0x21:
        case 0x41:
        case 0x61:
        case 0x81:
        case 0xa1:
        case 0xc1:
        case 0xe1:
            return IZX;
        case 0x11:
        case 0x31:
        case 0x51:
        case 0x71:
        case 0x91:
        case 0xb1:
        case 0xd1:
        case 0xf1:
            return IZY;
        case 0x0d:
        case 0x0e:
        case 0x20:
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x4c:
        case 0x4d:
        case 0x4e:
        case 0x6d:
        case 0x6e:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0xac:
        case 0xad:
        case 0xae:
        case 0xcc:
        case 0xcd:
        case 0xce:
        case 0xec:
        case 0xed:
        case 0xee:
            return ABS;
        case 0x1d:
        case 0x1e:
        case 0x3d:
        case 0x3e:
        case 0x5d:
        case 0x5e:
        case 0x7d:
        case 0x7e:
        case 0x9d:
        case 0xbc:
        case 0xbd:
        case 0xdd:
        case 0xde:
        case 0xfd:
        case 0xfe:
            return ABX;
        case 0x19:
        case 0x39:
        case 0x59:
        case 0x79:
        case 0x99:
        case 0xb9:
        case 0xbe:
        case 0xd9:
        case 0xf9:
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
        case 0x00:
        case 0x08:
        case 0x0a:
        case 0x18:
        case 0x28:
        case 0x2a:
        case 0x38:
        case 0x40:
        case 0x48:
        case 0x4a:
        case 0x58:
        case 0x60:
        case 0x68:
        case 0x6a:
        case 0x78:
        case 0x88:
        case 0x8a:
        case 0x98:
        case 0x9a:
        case 0xa8:
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
            char buf[2];
            itoa(opcode, buf, 16);
            throw std::runtime_error("Unsupported opcode 0x" + std::string(buf) + " in getAddressingMode.");
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
            cache = read();
            return (cache + x) % 0x100;
        case ZPY:
            cache = read();
            return (cache + y) % 0x100;
        case IZX:
            cache = read();
            cache2 = (cache + x) % 0x100;
            return (memory->read((cache2 + 1) % 0x100) << 8) | memory->read(cache2);
        case IZY:
            cache = read();
            cache2 = (memory->read((cache + 1) % 0x100) << 8) | memory->read(cache);
            return cache2 + y;
        case ABS:
            return readWord();
        case ABX:
            cache = readWord();
            return cache + x;
        case ABY:
            cache = readWord();
            return cache + y;
        case IND:
            cache = readWord();
            return memory->readWord(cache, true);
        case REL:
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
        case 0x00:
            return BRK;
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
        case 0xc1:
        case 0xc5:
        case 0xc9:
        case 0xcd:
        case 0xd1:
        case 0xd5:
        case 0xd9:
        case 0xdd:
            return CMP;
        case 0xe0:
        case 0xe4:
        case 0xec:
            return CPX;
        case 0xc0:
        case 0xc4:
        case 0xcc:
            return CPY;
        case 0xc6:
        case 0xce:
        case 0xd6:
        case 0xde:
            return DEC;
        case 0xca:
            return DEX;
        case 0x88:
            return DEY;
        case 0x41:
        case 0x45:
        case 0x49:
        case 0x4d:
        case 0x51:
        case 0x55:
        case 0x59:
        case 0x5d:
            return EOR;
        case 0xe6:
        case 0xee:
        case 0xf6:
        case 0xfe:
            return INC;
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
        case 0x46:
        case 0x4a:
        case 0x4e:
        case 0x56:
        case 0x5e:
            return LSR;
        case 0xea:
            return NOP;
        case 0x01:
        case 0x05:
        case 0x09:
        case 0x0d:
        case 0x11:
        case 0x15:
        case 0x19:
        case 0x1d:
            return ORA;
        case 0x48:
            return PHA;
        case 0x08:
            return PHP;
        case 0x68:
            return PLA;
        case 0x28:
            return PLP;
        case 0x26:
        case 0x2a:
        case 0x2e:
        case 0x36:
        case 0x3e:
            return ROL;
        case 0x66:
        case 0x6a:
        case 0x6e:
        case 0x76:
        case 0x7e:
            return ROR;
        case 0x40:
            return RTI;
        case 0x60:
            return RTS;
        case 0xe1:
        case 0xe5:
        case 0xe9:
        case 0xed:
        case 0xf1:
        case 0xf5:
        case 0xf9:
        case 0xfd:
            return SBC;
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

void CPU::Logger::logOpcode(uint8_t opcode, addressingMode mode, instruction inst) {
    logFile << std::setfill('0') << std::setw(4) << cpu.pc - 1 << "  " << ZPAD2 << (int)opcode;
        
    // Write opcode arguments to log
    int count = addressingModeReadCount[mode];
    if (count) {
        logFile << " " << ZPAD2 << (int)cpu.memory->read(cpu.pc);
        if (count > 1) {
            logFile << " " << ZPAD2 << (int)cpu.memory->read(cpu.pc + 1);
        } else {
            logFile << "   ";
        }
    } else {
        logFile << "      ";
    }

    // Write opcode name to log
    std::string opcodeName = opcodeNames[inst];
    logFile << "  " + opcodeName + " ";
}

void CPU::Logger::logArgsAndRegisters(addressingMode mode, instruction inst, Memory::addr_t addr, uint8_t argument) {
    switch (mode) {
        case IMM:
            logFile << "#$" << ZPAD2 << (int)argument;
            logFile << "                        ";
            break;
        case ZP:
            logFile << "$" << ZPAD2 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "                    ";
            break;
        case ZPX:
            logFile << "$" << ZPAD2 << cpu.cache << ",X @ " << ZPAD2 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "             ";
            break;
        case ZPY:
            logFile << "$" << ZPAD2 << cpu.cache << ",Y @ " << ZPAD2 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "             ";
            break;
        case IZX:
            logFile << "($" << ZPAD2 << cpu.cache << ",X) @ " << ZPAD2 << cpu.cache2 << " = " << ZPAD4 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "    ";
            break;
        case IZY:
            logFile << "($" << ZPAD2 << cpu.cache << "),Y = " << ZPAD4 << cpu.cache2 << " @ " << ZPAD4 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "  ";
            break;
        case ABS:
            logFile << "$" << ZPAD4 << addr;
            if (inst == JMP || inst == JSR) {
                logFile << "                       ";
            } else {
                logFile << " = " << ZPAD2 << (int)argument << "                  ";
            }
            break;
        case ABX:
            logFile << "$" << ZPAD4 << cpu.cache << ",X @ " << ZPAD4 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "         ";
            break;
        case ABY:
            logFile << "$" << ZPAD4 << cpu.cache << ",Y @ " << ZPAD4 << addr << " = " << ZPAD2 << (int)argument;
            logFile << "         ";
            break;
        case IND:
            logFile << "($" << ZPAD4 << cpu.cache << ") = " << ZPAD4 << addr;
            logFile << "              ";
            break;
        case REL:
            logFile << "$" << ZPAD4 << addr;
            logFile << "                       ";
            break;
        case NUL:
            if (inst == ASL || inst == LSR || inst == ROL || inst == ROR) {
                logFile << "A                           ";
            } else {
                logFile << "                            ";
            }
            break;
        default:
            logFile << "ERROR                           ";
            break;
    }

    logFile << "A:" << ZPAD2 << (int)cpu.a
        << " X:" << ZPAD2 << (int)cpu.x
        << " Y:" << ZPAD2 << (int)cpu.y
        << " P:" << ZPAD2 << (int)cpu.processorStatus()
        << " SP:" << ZPAD2 << (int)cpu.sp;

    logFile << std::endl;
}

void CPU::runOpcode(uint8_t opcode) {
    #ifdef DEBUG
    std::cout << "Trying to run opcode 0x" << std::hex << ZPAD2 << (int)opcode << " at position 0x" << pc - 1 << " and A=0x" << ZPAD2 << (int)a << std::dec << std::endl;
    #endif

    addressingMode mode = getAddressingMode(opcode);
    instruction inst = getInstruction(opcode);

    if (logger.logging) {
        // Write PC and opcode to log
        logger.logOpcode(opcode, mode, inst);
    }
    
    Memory::addr_t addr = 0;
    uint8_t argument;
    
    if (mode != NUL) {
        addr = getAddress(mode);
        // Read up to two bytes at the given address
        // TODO: Optimize by only reading argument if specific instruction requires it
        argument = memory->read(addr);
    } else {
        // If an opcode normally takes arguments, then the no-arg instruction uses the accumulator
        argument = a;
    }

    if (logger.logging) {
        // Write stylized opcode arguments to log and trailing spaces
        logger.logArgsAndRegisters(mode, inst, addr, argument);
    }

    switch (inst) {
        case ADC: {
            // Use a type large enough to detect carry
            uint16_t result = a + p.c + argument;
            p.c = result > 0xff;
            p.v = ((a ^ result) & (argument ^ result) & 0x80) != 0;
            a = result & 0xff;
            setNZ(a);
            }
            break;
        case AND:
            a &= argument;
            setNZ(a);
            break;
        case ASL: {
            // TODO: Make sure this is correct interpretation, same with other shifts
            p.c = (argument & 0x80) > 0;
            uint8_t result = argument << 1;
            setNZ(result);
            if (mode == NUL) {
                a = result;
            } else {
                memory->write(addr, result);
            }
            }
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
            p.z = (a & argument) == 0;
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
        case BRK:
            // TODO: Make sure this all works as intended
            pc += 2;
            stackPush((pc & 0xff00) >> 8);
            stackPush(pc & 0xff);
            p.b1 = p.b2 = 1;
            stackPush(processorStatus());
            p.i = 1;
            pc = 0xfffe;
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
        case CMP: {
            uint8_t result = a - argument;
            p.n = (result & 0x80) > 0;
            p.z = result == 0;
            p.c = a >= argument;
            }
            break;
        case CPX: {
            uint8_t result = x - argument;
            p.n = (result & 0x80) > 0;
            p.z = result == 0;
            p.c = x >= argument;
            }
            break;
        case CPY: {
            uint8_t result = y - argument;
            p.n = (result & 0x80) > 0;
            p.z = result == 0;
            p.c = y >= argument;
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
        case EOR:
            a ^= argument;
            setNZ(a);
            break;
        case INC:
            memory->write(addr, argument + 1);
            setNZ(argument + 1);
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
        case LSR: {
            p.c = argument & 1;
            uint8_t result = argument >> 1;
            setNZ(result);
            if (mode == NUL) {
                a = result;
            } else {
                memory->write(addr, result);
            }
            }
            break;
        case NOP:
            break;
        case ORA:
            a |= argument;
            setNZ(a);
            break;
        case PHA:
            stackPush(a);
            break;
        case PHP:
            // The B and I flags must be set to 1 in the copy on the stack
            // See https://www.masswerk.at/6502/6502_instruction_set.html#PHP
            stackPush(processorStatus() | 0x34);
            break;
        case PLA:
            a = stackPop();
            setNZ(a);
            break;
        case PLP: {
            // We ignore changes to the B and I flags
            // See https://www.masswerk.at/6502/6502_instruction_set.html#PLP
            processorFlags oldP = p;
            setProcessorStatus(stackPop());
            p.b1 = oldP.b1;
            p.b2 = oldP.b2;
            p.i = oldP.i;
            }
            break;
        case ROL: {
            uint8_t result = (argument << 1) | p.c;
            p.c = (argument & 0x80) > 0;
            setNZ(result);
            if (mode == NUL) {
                a = result;
            } else {
                memory->write(addr, result);
            }
            }
            break;
        case ROR:{
            uint8_t result = (argument >> 1) | (p.c << 7);
            p.c = argument & 1;
            setNZ(result);
            if (mode == NUL) {
                a = result;
            } else {
                memory->write(addr, result);
            }
            }
            break;
        case RTI:{
            // We ignore changes to the B and I flags
            // See https://www.masswerk.at/6502/6502_instruction_set.html#PLP
            processorFlags oldP = p;
            setProcessorStatus(stackPop());
            p.b1 = oldP.b1;
            p.b2 = oldP.b2;
            p.i = oldP.i;
            pc = stackPop() | (stackPop() << 8);
            }
            break;
        case RTS:
            pc = (stackPop() | (stackPop() << 8)) + 1;
            break;
        case SBC: {
            // Like ADC but with inverted argument
            // See https://stackoverflow.com/questions/29193303/6502-emulation-proper-way-to-implement-adc-and-sbc
            uint16_t result = a + p.c + ~argument;
            // The carry flag is the inverse of ADC
            p.c = result <= 0xff;
            p.v = ((a ^ result) & (~argument ^ result) & 0x80) != 0;
            a = result & 0xff;
            setNZ(a);
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
