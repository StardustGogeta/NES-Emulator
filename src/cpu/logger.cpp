#include "cpu.h"
#include <iostream>
#include <iomanip>

CPU::Logger::Logger(CPU& cpu) : logging(false), cpu(cpu) { }

void CPU::Logger::formatLogger() {
    logFile << std::hex << std::uppercase << std::setfill('0');
}

void CPU::Logger::start(std::string path) {
    logFile.open(path, std::ios::out);
    formatLogger();
    logging = true;
    std::cout << "Started CPU logging." << std::endl;
}

void CPU::Logger::stop() {
    logFile.close();
    std::cout << "Stopped CPU logging." << std::endl;
}

void CPU::Logger::logOpcode(uint8_t opcode, addressingMode mode, instruction inst) {
    logFile << PAD4 << cpu.pc - 1 << "  " << PAD2 << (int)opcode;
        
    // Write opcode arguments to log
    int count = addressingModeReadCount[mode];
    if (count) {
        logFile << " " << PAD2 << (int)cpu.memory->read(cpu.pc);
        if (count > 1) {
            logFile << " " << PAD2 << (int)cpu.memory->read(cpu.pc + 1);
        } else {
            logFile << "   ";
        }
    } else {
        logFile << "      ";
    }

    // Write opcode name to log
    std::string opcodeName = opcodeNames[inst];
    logFile << (isLegalOpcode(opcode) ? "  " : " *") + opcodeName + " ";
}

void CPU::Logger::logArgsAndRegisters(addressingMode mode, instruction inst, addr_t addr, uint8_t argument) {
    switch (mode) {
        case IMM:
            logFile << "#$" << PAD2 << (int)argument;
            logFile << "                        ";
            break;
        case ZPG:
            logFile << "$" << PAD2 << addr << " = " << PAD2 << (int)argument;
            logFile << "                    ";
            break;
        case ZPX:
            logFile << "$" << PAD2 << cpu.cache << ",X @ " << PAD2 << addr << " = " << PAD2 << (int)argument;
            logFile << "             ";
            break;
        case ZPY:
            logFile << "$" << PAD2 << cpu.cache << ",Y @ " << PAD2 << addr << " = " << PAD2 << (int)argument;
            logFile << "             ";
            break;
        case IZX:
            logFile << "($" << PAD2 << cpu.precache << ",X) @ " << PAD2 << cpu.cache << " = " << PAD4 << addr << " = " << PAD2 << (int)argument;
            logFile << "    ";
            break;
        case IZY:
            logFile << "($" << PAD2 << cpu.precache << "),Y = " << PAD4 << cpu.cache << " @ " << PAD4 << addr << " = " << PAD2 << (int)argument;
            logFile << "  ";
            break;
        case ABS:
            logFile << "$" << PAD4 << addr;
            if (inst == JMP || inst == JSR) {
                logFile << "                       ";
            } else {
                logFile << " = " << PAD2 << (int)argument << "                  ";
            }
            break;
        case ABX:
            logFile << "$" << PAD4 << cpu.cache << ",X @ " << PAD4 << addr << " = " << PAD2 << (int)argument;
            logFile << "         ";
            break;
        case ABY:
            logFile << "$" << PAD4 << cpu.cache << ",Y @ " << PAD4 << addr << " = " << PAD2 << (int)argument;
            logFile << "         ";
            break;
        case IND:
            logFile << "($" << PAD4 << cpu.cache << ") = " << PAD4 << addr;
            logFile << "              ";
            break;
        case REL:
            logFile << "$" << PAD4 << addr;
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

    logFile << "A:" << PAD2 << (int)cpu.a
        << " X:" << PAD2 << (int)cpu.x
        << " Y:" << PAD2 << (int)cpu.y
        << " P:" << PAD2 << (int)cpu.processorStatus()
        << " SP:" << PAD2 << (int)cpu.sp;
}

void CPU::Logger::logPPU(int scanline, int cyclesOnLine) {
    logFile << std::dec << std::setfill(' ') << " PPU:" << std::setw(3) << scanline
        << "," << std::setw(3) << cyclesOnLine;
}

void CPU::Logger::logCycles(int cyclesExecuted) {
    logFile << " CYC:" << cyclesExecuted << std::endl;
    // Reset the logger format for the next line.
    formatLogger();
}
