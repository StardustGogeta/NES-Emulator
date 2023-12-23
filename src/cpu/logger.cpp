#include "cpu.h"
#include <print>

CPU::Logger::Logger(CPU& cpu) : logging(false), reversePPU(false), cpu(cpu) { }

void CPU::Logger::start(std::string path, bool reversePPU /* = false */) {
    logFile.open(path, std::ios::out);
    logging = true;
    // Nintendulator and nestest seem to use reversed PPU cycle notations
    // Setting this to true will make it Nintendulator-compatible
    this->reversePPU = reversePPU;
    std::println("Started CPU logging.");
}

void CPU::Logger::stop() {
    logFile.close();
    std::println("Stopped CPU logging.");
}

void CPU::Logger::logOpcode(uint8_t opcode, addressingMode mode, instruction inst) {
    std::print(logFile, "{:04X}  {:02X}", cpu.pc - 1, opcode);
        
    // Write opcode arguments to log
    int count = addressingModeReadCount[mode];
    if (count) {
        std::print(logFile, " {:02X}", cpu.memory->read(cpu.pc));
        if (count > 1) {
            std::print(logFile, " {:02X}", cpu.memory->read(cpu.pc + 1));
        } else {
            std::print(logFile, "   ");
        }
    } else {
        std::print(logFile, "      ");
    }

    // Write opcode name to log
    std::string opcodeName = opcodeNames[inst];
    std::print(logFile, "{}", (isLegalOpcode(opcode) ? "  " : " *") + opcodeName + " ");
}

void CPU::Logger::logArgsAndRegisters(addressingMode mode, instruction inst, addr_t addr, uint8_t argument) {
    switch (mode) {
        case IMM:
            std::print(logFile, "#${:02X}                        ", argument);
            break;
        case ZPG:
            std::print(logFile, "${:02X} = {:02X}                    ", addr, argument);
            break;
        case ZPX:
            std::print(logFile, "${:02X},X @ {:02X} = {:02X}             ", cpu.cache, addr, argument);
            break;
        case ZPY:
            std::print(logFile, "${:02X},Y @ {:02X} = {:02X}             ", cpu.cache, addr, argument);
            break;
        case IZX:
            std::print(logFile, "(${:02X},X) @ {:02X} = {:04X} = {:02X}    ", cpu.precache, cpu.cache, addr, argument);
            break;
        case IZY:
            std::print(logFile, "(${:02X}),Y = {:04X} @ {:04X} = {:02X}  ", cpu.precache, cpu.cache, addr, argument);
            break;
        case ABS:
            std::print(logFile, "${:04X}", addr);
            if (inst == JMP || inst == JSR) {
                std::print(logFile, "                       ");
            } else {
                std::print(logFile, " = {:02X}                  ", argument);
            }
            break;
        case ABX:
            std::print(logFile, "${:04X},X @ {:04X} = {:02X}         ", cpu.cache, addr, argument);
            break;
        case ABY:
            std::print(logFile, "${:04X},Y @ {:04X} = {:02X}         ", cpu.cache, addr, argument);
            break;
        case IND:
            std::print(logFile, "(${:04X}) = {:04X}              ", cpu.cache, addr);
            break;
        case REL:
            std::print(logFile, "${:04X}                       ", addr);
            break;
        case NUL:
            if (inst == ASL || inst == LSR || inst == ROL || inst == ROR) {
                std::print(logFile, "A                           ");
            } else {
                std::print(logFile, "                            ");
            }
            break;
        default:
            std::print(logFile, "ERROR                           ");
            break;
    }

    std::print(logFile, "A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}", cpu.a, cpu.x, cpu.y, cpu.processorStatus(), cpu.sp);
}

void CPU::Logger::logPPU(int scanline, int cyclesOnLine) {
    int first = reversePPU ? cyclesOnLine : scanline;
    int second = reversePPU ? scanline : cyclesOnLine;

    std::print(logFile, " PPU:{:3},{:3}", first, second);
}

void CPU::Logger::logCycles(int cyclesExecuted) {
    std::println(logFile, " CYC:{}", cyclesExecuted);
}
