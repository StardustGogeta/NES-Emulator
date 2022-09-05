#include "cpu.h"
#include <bit>
#include <stdexcept>
#include <iostream>
#include <cstring>

CPU::CPU() : logger(*this) {
    running = false;
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
    cyclesRequested = 0;
    cyclesExecuted = 0;
    maxCycles = 0;
}

void CPU::setPC() {
    if (memory) {
        this->pc = memory->readWord(0xfffc);
    }
}

void CPU::setPC(CoreMemory::addr_t pc) {
    this->pc = pc;
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
CoreMemory::addr_t CPU::getAddress(addressingMode mode) {
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
    // std::thread ppuThread(&PPU::cycles, ppu, 3);

    #ifdef DEBUG
    std::cout << "Trying to run opcode 0x" << std::hex << std::setfill('0') << PAD2 << (int)opcode << " at position 0x" << PAD4 << pc - 1 << std::dec << std::endl;
    #endif

    addressingMode mode = getAddressingMode(opcode);
    instruction inst = getInstruction(opcode);

    if (logger.logging) {
        // Write PC and opcode to log
        logger.logOpcode(opcode, mode, inst);
    }
    
    CoreMemory::addr_t addr = 0;
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

    int cycleOffset = runInstruction(mode, inst, addr, argument, extraCycles[opcode]);

    logger.logPPU(ppu->scanline, ppu->cyclesOnLine);
    logger.logCycles(cyclesExecuted);

    int cycleCount = getCycleCount(opcode, cycleOffset);

    // PPU does 3 cycles for every CPU cycle
    ppu->cycles(cycleCount * 3);

    // TODO: Get actual cycle count
    cyclesExecuted += cycleCount;
    bool dying = waitForCycles(cycleCount);    

    if (dying) {
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

bool CPU::checkRunning() {
    return running;
}

/*
    Causes the current thread to wait until the controlling thread calls CPU::cycle().
    Returns true if execution of the thread should continue and false if the thread is being killed.
*/
bool CPU::waitForCycle() {
    std::unique_lock lck(cycleStatusMutex);

    bool caughtUp = cyclesRequested <= cyclesExecuted;
    notDone = cyclesExecuted < maxCycles || !maxCycles;

    #ifdef DEBUG
    std::cout << "Executed " << cyclesExecuted
        << " cycles of " << cyclesRequested
        << " requested and max " << maxCycles << std::endl;
    #endif

    if (running && caughtUp && notDone) {
        // We are caught up, so we need to wait until the next cycle is requested.
        cycleStatusCV.wait(lck, [this] {
            // Check if we have more cycles in the queue to execute.
            bool moreCycles = cyclesRequested > cyclesExecuted;
            // Check if we have already hit the maximum number of cycles.
            notDone = cyclesExecuted < maxCycles || !maxCycles;
            // Check if we have been manually killed.
            bool killed = !running;
            // If any are true, we need to stop waiting and move on.
            return killed || moreCycles || !notDone;
        });
    }

    return running && notDone;
}

/*
    Continually runs opcodes until stopped by CPU::stop().
*/
void CPU::start() {
    std::unique_lock lck(cycleStatusMutex);
    notDone = true;
    running = true;
    lck.unlock();

    while (running && notDone) {
        runOpcode(read());
    }
}

/*
    Stops the CPU from running by ending after the currently queued opcodes.
    Argument t is the thread running CPU::start() that needs to end.
*/
void CPU::stop(std::thread& t) {
    std::unique_lock lck(cycleStatusMutex);
    maxCycles = cyclesRequested;
    lck.unlock();

    // Stop waiting for the next cycle, if that is the current thread state.
    cycleStatusCV.notify_all();

    // Join the thread afterwards.
    if (t.joinable()) {
        t.join();
    }
}

/*
    Stops the CPU from running by ending after the current opcode.
    Causes the condition in CPU::start() to be false.
    Argument t is the thread running CPU::start() that needs to end.
    This is more forceful than CPU::stop(), which allows queued opcodes to finish.
*/
void CPU::kill(std::thread& t) {
    std::unique_lock lck(cycleStatusMutex);
    running = false;
    lck.unlock();

    // Stop waiting for the next cycle, if that is the current thread state.
    cycleStatusCV.notify_all();

    // Join the thread afterwards.
    if (t.joinable()) {
        t.join();
    }
}

/*
    Performs one cycle of the CPU.
*/
void CPU::cycle() {
    // std::cout << "Start of cycle\n";
    // Increment the requested CPU cycle count.
    std::unique_lock lck(cycleStatusMutex);
    cyclesRequested++;
    cycleStatusCV.notify_all();
}
