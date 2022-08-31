#include "cpu.h"
#include <iostream>
#include <iomanip>

// Pad output to given width using zeros
#define ZPAD2 std::setfill('0') << std::setw(2)
#define ZPAD4 std::setfill('0') << std::setw(4)

CPU::Logger::Logger(CPU& cpu) : cpu(cpu) { }

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
    logFile << (isLegalOpcode(opcode) ? "  " : " *") + opcodeName + " ";
}

void CPU::Logger::logArgsAndRegisters(addressingMode mode, instruction inst, CoreMemory::addr_t addr, uint8_t argument) {
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
