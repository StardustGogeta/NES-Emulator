#include "cpu.h"
#include <cstdio>
#include <print>

CPU::Logger::Logger(CPU& cpu) : logging(false), reversePPU(false), cpu(cpu) { }

void CPU::Logger::start(std::string path, bool newReversePPU /* = false */) {
    logFile = fopen(path.c_str(), "w");
    logging = true;
    // Nintendulator and nestest seem to use reversed PPU cycle notations
    // Setting this to true will make it Nintendulator-compatible
    reversePPU = newReversePPU;
    std::println("Started CPU logging.");
}

void CPU::Logger::stop() {
    fclose(logFile);
    logFile = nullptr;
    std::println("Stopped CPU logging.");
}

CPU::Logger::~Logger() {
    if (logFile) {
        fclose(logFile);
        logFile = nullptr;
    }
}


void CPU::Logger::logOpcode(uint8_t opcode, addressingMode mode, instruction inst) {
    fprintf(logFile, "%04X  %02X", cpu.pc - 1, opcode);
        
    // Write opcode arguments to log
    int count = addressingModeReadCount[mode];
    if (count) {
        fprintf(logFile, " %02X", cpu.memory->read(cpu.pc));
        if (count > 1) {
            fprintf(logFile, " %02X", cpu.memory->read(cpu.pc + 1));
        } else {
            fprintf(logFile, "   ");
        }
    } else {
        fprintf(logFile, "      ");
    }

    // Write opcode name to log
    std::string opcodeName = opcodeNames[inst];
    fprintf(logFile, "%s", ((isLegalOpcode(opcode) ? "  " : " *") + opcodeName + " ").c_str());
}

void CPU::Logger::logArgsAndRegisters(addressingMode mode, instruction inst, addr_t addr, uint8_t argument) {
    // Nintendulator prints out FF for all PPU registers, no matter their true value
    if (addr >= 0x2000 && addr < 0x4000) {
        argument = 0xff;
    }
    // Nintendulator also prints out FF for all APU registers, regardless of true value
    else if (addr >= 0x4000 && addr < 0x4020) {
        argument = 0xff;
    }
    switch (mode) {
        case IMM:
            fprintf(logFile, "#$%02X                        ", argument);
            break;
        case ZPG:
            fprintf(logFile, "$%02X = %02X                    ", addr, argument);
            break;
        case ZPX:
            fprintf(logFile, "$%02X,X @ %02X = %02X             ", cpu.cache, addr, argument);
            break;
        case ZPY:
            fprintf(logFile, "$%02X,Y @ %02X = %02X             ", cpu.cache, addr, argument);
            break;
        case IZX:
            fprintf(logFile, "($%02X,X) @ %02X = %04X = %02X    ", cpu.precache, cpu.cache, addr, argument);
            break;
        case IZY:
            fprintf(logFile, "($%02X),Y = %04X @ %04X = %02X  ", cpu.precache, cpu.cache, addr, argument);
            break;
        case ABS:
            fprintf(logFile, "$%04X", addr);
            if (inst == JMP || inst == JSR) {
                fprintf(logFile, "                       ");
            } else {
                fprintf(logFile, " = %02X                  ", argument);
            }
            break;
        case ABX:
            fprintf(logFile, "$%04X,X @ %04X = %02X         ", cpu.cache, addr, argument);
            break;
        case ABY:
            fprintf(logFile, "$%04X,Y @ %04X = %02X         ", cpu.cache, addr, argument);
            break;
        case IND:
            fprintf(logFile, "($%04X) = %04X              ", cpu.cache, addr);
            break;
        case REL:
            fprintf(logFile, "$%04X                       ", addr);
            break;
        case NUL:
            if (inst == ASL || inst == LSR || inst == ROL || inst == ROR) {
                fprintf(logFile, "A                           ");
            } else {
                fprintf(logFile, "                            ");
            }
            break;
        default:
            fprintf(logFile, "ERROR                           ");
            break;
    }

    fprintf(logFile, "A:%02X X:%02X Y:%02X P:%02X SP:%02X", cpu.a, cpu.x, cpu.y, cpu.processorStatus(), cpu.sp);
}

/*
    Logs PPU cycle information.
*/
void CPU::Logger::logPPUInfo(int scanline, int cyclesOnLine) {
    int first = reversePPU ? cyclesOnLine : scanline;
    int second = reversePPU ? scanline : cyclesOnLine;

    fprintf(logFile, " PPU:%3d,%3d", first, second);
}

/*
    Writes an arbitrary string to the current log file.
*/
void CPU::Logger::logStr(std::string_view str) {
    // TODO: .data() is not always a c string
    fprintf(logFile, "%s", str.data());
}

void CPU::Logger::logCycles(int cyclesExecuted) {
    std::println(logFile, " CYC:{}", cyclesExecuted);
}
