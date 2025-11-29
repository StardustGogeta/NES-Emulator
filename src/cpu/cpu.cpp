#include "cpu.h"
#include <bit>
#include <cstdint>
#include <stdexcept>
#include <print>
#include <cstring>

CPU::CPU() : logger(*this) {
    memory = nullptr;
    ppu = nullptr;
    reset();
}

void CPU::reset() {
    // Read the PC from the reset vector if possible
    this->pc = memory ? memory->readWord(0xfffc) : 0;
    sp = 0xfd;
    a = x = y = 0;
    memset(&p, 0, sizeof(p));
    p.b1 = p.i = 1;
    cyclesExecuted = 0;
}

void CPU::setPC(bool ignoreCycles /* = false */) {
    if (memory) {
        pc = memory->readWord(0xfffc);
        // std::cout << "Initial PC was found to be " << std::hex << pc << std::endl;
        
        // Going to the initial PC location seems to take 14 cycles, per Nintendulator
        // We have a flag to disable this if it's unwanted
        if (!ignoreCycles) {
            cyclesExecuted = 14;
        }
    }
}

void CPU::setPC(addr_t newPc) {
    pc = newPc;
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
    Fetches the address of the operand for an opcode based on addressing mode.
    Assumes the program counter is at the start of the arguments list.
    Moves the program counter to the end of the arguments list.
*/
addr_t CPU::getAddress(addressingMode mode) {
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
            precache = read();
            cache = (precache + x) % 0x100;
            return (memory->read((cache + 1) % 0x100) << 8) | memory->read(cache);
        case IZY:
            precache = read();
            cache = (memory->read((precache + 1) % 0x100) << 8) | memory->read(precache);
            return cache + y;
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

void CPU::setNZ(uint8_t val) {
    p.n = (val & 0x80) > 0;
    p.z = val == 0;
}

void CPU::stackPushU16(uint16_t val) {
    stackPush((val & 0xFF00) >> 8);
    stackPush(val & 0xFF);
}

void CPU::stackPush(uint8_t val) {
    memory->write(0x100 + (sp--), val);
}

uint8_t CPU::stackPop() {
    return memory->read(0x100 + (++sp));
}

uint8_t CPU::processorStatus() {
    return std::bit_cast<uint8_t>(p);
}

void CPU::setProcessorStatus(uint8_t status) {
    p = std::bit_cast<processorFlags>(status);
}

/*
    Runs a specified opcode
    ignoreCycles - true if the PPU should ignore the CPU cycles being run
*/
void CPU::runOpcode(uint8_t opcode, bool ignoreCycles /* = false */) {
    // This slows things down a lot:
    // std::println("Trying to run opcode {:#04x} at position {:#06x}", opcode, pc - 1);

    addressingMode mode = getAddressingMode(opcode);
    instruction inst = getInstruction(opcode);

    if (logger.logging) {
        // Write PC and opcode to log
        logger.logOpcode(opcode, mode, inst);
    }
    
    addr_t addr = 0;
    if (mode != NUL) {
        addr = getAddress(mode);
    }

    int cycleOffset = getCycleCountOffset(inst, addr, extraCycleCounts[opcode]);
    int cycleCount = getCycleCount(opcode, cycleOffset);

    // Grab the old PPU values before cycling
    int ppuScanLine = ppu->scanline;
    int ppuCyclesOnLine = ppu->cyclesOnLine;

    if (!ignoreCycles) {
        // PPU does 3 cycles for every CPU cycle
        ppu->cycles(cycleCount * 3);
    }

    uint8_t argument;
    if (mode != NUL) {
        // Read up to two bytes at the given address
        // TODO: Optimize by only reading argument if specific instruction requires it
        argument = memory->read(addr);
    }
    else {
        // If an opcode normally takes arguments, then the no-arg instruction uses the accumulator
        argument = a;
    }

    if (logger.logging) {
        // Write stylized opcode arguments to log and trailing spaces
        logger.logArgsAndRegisters(mode, inst, addr, argument);
    }

    runInstruction(mode, inst, addr, argument);

    if (logger.logging) {
        logger.logPPUInfo(ppuScanLine, ppuCyclesOnLine);
        logger.logCycles(cyclesExecuted);
    }

    if (!ignoreCycles) {
        cyclesExecuted += cycleCount;
    }
}

/*
    Runs 1 CPU instruction (potentially taking multiple cycles)
*/
void CPU::runNextInstruction() {
    if (ppu->checkNmiFlag()) {
        handleNonMaskableInterrupt();
    }
    runOpcode(read());
}

void CPU::handleNonMaskableInterrupt() {
    // TODO: Test this is correct
    // See https://www.nesdev.org/wiki/NMI
    stackPushU16(pc);
    p.b1 = 0;
    p.b2 = 1;
    stackPush(processorStatus());
    p.i = 1;
    uint16_t nmiHandlerAddress = memory->readWord(0xfffa);
    ppu->clearNmiFlag();
    pc = nmiHandlerAddress;
}