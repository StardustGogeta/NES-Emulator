#include "cpu.h"
#include <bit>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cstring>

// Pad output to given width using zeros
#define ZPAD2 std::setfill('0') << std::setw(2)
#define ZPAD4 std::setfill('0') << std::setw(4)

const std::string addressingModeNames[] = {
    "IMM", "ZPG", "ZPX", "ZPY", "IZX", "IZY", "ABS", "ABX", "ABY", "IND", "REL", "NUL", "XXX"
};

// Number of bytes that must be read after each addressing mode
const int addressingModeReadCount[] = {
    1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 0
};

const std::string opcodeNames[] = {
    "ADC", "AND", "ASL", "BCC", "BCS", "BEQ", "BIT", "BMI", "BNE", "BPL", "BRK", "BVC", "BVS", "CLC",
    "CLD", "CLI", "CLV", "CMP", "CPX", "CPY", "DEC", "DEX", "DEY", "EOR", "INC", "INX", "INY", "JMP",
    "JSR", "LDA", "LDX", "LDY", "LSR", "NOP", "ORA", "PHA", "PHP", "PLA", "PLP", "ROL", "ROR", "RTI",
    "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA", "YYY"
};

// Table of addressing modes by opcode
const addressingMode addressingModesByOpcode[] = {
/*  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xa   xb   xc   xd   xe   xf   */
    NUL, IZX, XXX, XXX, XXX, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, XXX, ABS, ABS, XXX, // 0x
    REL, IZY, XXX, XXX, XXX, ZPX, ZPX, XXX, NUL, ABY, XXX, XXX, XXX, ABX, ABX, XXX, // 1x
    ABS, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // 2x
    REL, IZY, XXX, XXX, XXX, ZPX, ZPX, XXX, NUL, ABY, XXX, XXX, XXX, ABX, ABX, XXX, // 3x
    NUL, IZX, XXX, XXX, XXX, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // 4x
    REL, IZY, XXX, XXX, XXX, ZPX, ZPX, XXX, NUL, ABY, XXX, XXX, XXX, ABX, ABX, XXX, // 5x
    NUL, IZX, XXX, XXX, XXX, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, IND, ABS, ABS, XXX, // 6x
    REL, IZY, XXX, XXX, XXX, ZPX, ZPX, XXX, NUL, ABY, XXX, XXX, XXX, ABX, ABX, XXX, // 7x
    XXX, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, XXX, NUL, XXX, ABS, ABS, ABS, XXX, // 8x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPY, XXX, NUL, ABY, NUL, XXX, XXX, ABX, XXX, XXX, // 9x
    IMM, IZX, IMM, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // ax
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPY, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABY, XXX, // bx
    IMM, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // cx
    REL, IZY, XXX, XXX, XXX, ZPX, ZPX, XXX, NUL, ABY, XXX, XXX, XXX, ABX, ABX, XXX, // dx
    IMM, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // ex
    REL, IZY, XXX, XXX, XXX, ZPX, ZPX, XXX, NUL, ABY, XXX, XXX, XXX, ABX, ABX, XXX, // fx
};

const instruction instructionsByOpcode[] = {
/*  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xa   xb   xc   xd   xe   xf   */
    BRK, ORA, YYY, YYY, YYY, ORA, ASL, YYY, PHP, ORA, ASL, YYY, YYY, ORA, ASL, YYY, // 0x
    BPL, ORA, YYY, YYY, YYY, ORA, ASL, YYY, CLC, ORA, YYY, YYY, YYY, ORA, ASL, YYY, // 1x
    JSR, AND, YYY, YYY, BIT, AND, ROL, YYY, PLP, AND, ROL, YYY, BIT, AND, ROL, YYY, // 2x
    BMI, AND, YYY, YYY, YYY, AND, ROL, YYY, SEC, AND, YYY, YYY, YYY, AND, ROL, YYY, // 3x
    RTI, EOR, YYY, YYY, YYY, EOR, LSR, YYY, PHA, EOR, LSR, YYY, JMP, EOR, LSR, YYY, // 4x
    BVC, EOR, YYY, YYY, YYY, EOR, LSR, YYY, CLI, EOR, YYY, YYY, YYY, EOR, LSR, YYY, // 5x
    RTS, ADC, YYY, YYY, YYY, ADC, ROR, YYY, PLA, ADC, ROR, YYY, JMP, ADC, ROR, YYY, // 6x
    BVS, ADC, YYY, YYY, YYY, ADC, ROR, YYY, SEI, ADC, YYY, YYY, YYY, ADC, ROR, YYY, // 7x
    YYY, STA, YYY, YYY, STY, STA, STX, YYY, DEY, YYY, TXA, YYY, STY, STA, STX, YYY, // 8x
    BCC, STA, YYY, YYY, STY, STA, STX, YYY, TYA, STA, TXS, YYY, YYY, STA, YYY, YYY, // 9x
    LDY, LDA, LDX, YYY, LDY, LDA, LDX, YYY, TAY, LDA, TAX, YYY, LDY, LDA, LDX, YYY, // ax
    BCS, LDA, YYY, YYY, LDY, LDA, LDX, YYY, CLV, LDA, TSX, YYY, LDY, LDA, LDX, YYY, // bx
    CPY, CMP, YYY, YYY, CPY, CMP, DEC, YYY, INY, CMP, DEX, YYY, CPY, CMP, DEC, YYY, // cx
    BNE, CMP, YYY, YYY, YYY, CMP, DEC, YYY, CLD, CMP, YYY, YYY, YYY, CMP, DEC, YYY, // dx
    CPX, SBC, YYY, YYY, CPX, SBC, INC, YYY, INX, SBC, NOP, YYY, CPX, SBC, INC, YYY, // ex
    BEQ, SBC, YYY, YYY, YYY, SBC, INC, YYY, SED, SBC, YYY, YYY, YYY, SBC, INC, YYY, // fx
};

CPU::CPU() : logger(*this) {
    reset();
    running = false;
}

CPU::Logger::Logger(CPU& cpu) : cpu(cpu) { }

void CPU::reset(Memory::addr_t pc /* =0xfffc */) {
    this->pc = pc;
    sp = 0xfd;
    a = x = y = 0;
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
    addressingMode ret = addressingModesByOpcode[opcode];
    if (ret == XXX) {
        char buf[3];
        sprintf(buf, "%02x", opcode);
        throw std::runtime_error("Unsupported opcode 0x" + std::string(buf) + " in getAddressingMode.");
    } else {
        return ret;
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
        case ZPG:
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
    instruction ret = instructionsByOpcode[opcode];
    if (ret == YYY) {
        char buf[2];
        sprintf(buf, "%02x", opcode);
        throw std::runtime_error("Unsupported opcode 0x" + std::string(buf) + " in getInstruction.");
    } else {
        return ret;
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
        case ZPG:
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
    std::cout << "Trying to run opcode 0x" << std::hex << ZPAD2 << (int)opcode << " at position 0x" << pc - 1 << std::dec << std::endl;
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
        case YYY:
            throw std::runtime_error("Unsupported opcode in runOpcode.");
            break;
    }

    // TODO: Get actual cycle count
    if (!waitForCycles(2)) {
        return; 
    };
}

/*
    Causes the current thread to wait until the controlling thread calls CPU::cycle() n times.
    Returns true if execution of the thread should continue and false if the thread is being killed.
*/
bool CPU::waitForCycles(int n) {
    for (int i = 0; i < n; i++) {
        if (!waitForCycle()) {
            return false;
        }
    }
    return true;
}

/*
    Causes the current thread to wait until the controlling thread calls CPU::cycle().
    Returns true if execution of the thread should continue and false if the thread is being killed.
*/
bool CPU::waitForCycle() {
    // Tell that this last cycle is finished.
    std::unique_lock lck(cycleStatusMutex);
    cycleStatus = 2;
    cycleStatusCV.notify_all();

    // Wait until the next cycle is triggered.
    cycleStatusCV.wait(lck, [this] {return cycleStatus == 0;});
    cycleStatus = 1;

    return running;
}

/*
    Continually runs opcodes until stopped by CPU::stop().
*/
void CPU::start() {
    running = true;
    cycleStatus = 0;

    while (running) {
        runOpcode(read());
    }
}

/*
    Stops the CPU from running by ending after the current opcode.
    Causes the condition in CPU::start() to be false.
    Argument t is the thread running CPU::start() that needs to end.
*/
void CPU::stop(std::thread& t) {
    running = false;

    // TODO: May need to issue one last cycleStatus=0 and notify_all to finish the opcode.

    // Join the thread afterwards.
    if (t.joinable()) {
        t.join();
    }
}

/*
    Performs one cycle of the CPU.
*/
void CPU::cycle() {
    // Wait until the last cycle finished.
    std::unique_lock lck(cycleStatusMutex);
    cycleStatusCV.wait(lck, [this] {return cycleStatus == 2;});

    // Start a new cycle.
    cycleStatus = 0;
    cycleStatusCV.notify_all();
}
