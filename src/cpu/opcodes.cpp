#include "opcodes.h"
#include "cpu.h"

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
    "RTS", "SBC", "SEC", "SED", "SEI", "STA", "STX", "STY", "TAX", "TAY", "TSX", "TXA", "TXS", "TYA",
    "DCP", "ISB", "LAX", "RLA", "RRA", "SAX", "SLO", "SRE", "YYY"
};

// Table of addressing modes by opcode
const addressingMode addressingModesByOpcode[] = {
/*  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xa   xb   xc   xd   xe   xf   */
    NUL, IZX, XXX, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, XXX, ABS, ABS, ABS, ABS, // 0x
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPX, ZPX, NUL, ABY, NUL, ABY, ABX, ABX, ABX, ABX, // 1x
    ABS, IZX, XXX, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, XXX, ABS, ABS, ABS, ABS, // 2x
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPX, ZPX, NUL, ABY, NUL, ABY, ABX, ABX, ABX, ABX, // 3x
    NUL, IZX, XXX, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, XXX, ABS, ABS, ABS, ABS, // 4x
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPX, ZPX, NUL, ABY, NUL, ABY, ABX, ABX, ABX, ABX, // 5x
    NUL, IZX, XXX, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, XXX, IND, ABS, ABS, ABS, // 6x
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPX, ZPX, NUL, ABY, NUL, ABY, ABX, ABX, ABX, ABX, // 7x
    IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, XXX, ABS, ABS, ABS, ABS, // 8x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPY, ZPY, NUL, ABY, NUL, XXX, XXX, ABX, XXX, XXX, // 9x
    IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, IMM, ABS, ABS, ABS, ABS, // ax
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPY, ZPY, NUL, ABY, NUL, XXX, ABX, ABX, ABY, ABY, // bx
    IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, XXX, ABS, ABS, ABS, ABS, // cx
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPX, ZPX, NUL, ABY, NUL, ABY, ABX, ABX, ABX, ABX, // dx
    IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, IMM, ABS, ABS, ABS, ABS, // ex
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPX, ZPX, NUL, ABY, NUL, ABY, ABX, ABX, ABX, ABX, // fx
};

const instruction instructionsByOpcode[] = {
/*  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xa   xb   xc   xd   xe   xf   */
    BRK, ORA, YYY, SLO, NOP, ORA, ASL, SLO, PHP, ORA, ASL, YYY, NOP, ORA, ASL, SLO, // 0x
    BPL, ORA, YYY, SLO, NOP, ORA, ASL, SLO, CLC, ORA, NOP, SLO, NOP, ORA, ASL, SLO, // 1x
    JSR, AND, YYY, RLA, BIT, AND, ROL, RLA, PLP, AND, ROL, YYY, BIT, AND, ROL, RLA, // 2x
    BMI, AND, YYY, RLA, NOP, AND, ROL, RLA, SEC, AND, NOP, RLA, NOP, AND, ROL, RLA, // 3x
    RTI, EOR, YYY, SRE, NOP, EOR, LSR, SRE, PHA, EOR, LSR, YYY, JMP, EOR, LSR, SRE, // 4x
    BVC, EOR, YYY, SRE, NOP, EOR, LSR, SRE, CLI, EOR, NOP, SRE, NOP, EOR, LSR, SRE, // 5x
    RTS, ADC, YYY, RRA, NOP, ADC, ROR, RRA, PLA, ADC, ROR, YYY, JMP, ADC, ROR, RRA, // 6x
    BVS, ADC, YYY, RRA, NOP, ADC, ROR, RRA, SEI, ADC, NOP, RRA, NOP, ADC, ROR, RRA, // 7x
    NOP, STA, NOP, SAX, STY, STA, STX, SAX, DEY, NOP, TXA, YYY, STY, STA, STX, SAX, // 8x
    BCC, STA, YYY, YYY, STY, STA, STX, SAX, TYA, STA, TXS, YYY, YYY, STA, YYY, YYY, // 9x
    LDY, LDA, LDX, LAX, LDY, LDA, LDX, LAX, TAY, LDA, TAX, LAX, LDY, LDA, LDX, LAX, // ax
    BCS, LDA, YYY, LAX, LDY, LDA, LDX, LAX, CLV, LDA, TSX, YYY, LDY, LDA, LDX, LAX, // bx
    CPY, CMP, NOP, DCP, CPY, CMP, DEC, DCP, INY, CMP, DEX, YYY, CPY, CMP, DEC, DCP, // cx
    BNE, CMP, YYY, DCP, NOP, CMP, DEC, DCP, CLD, CMP, NOP, DCP, NOP, CMP, DEC, DCP, // dx
    CPX, SBC, NOP, ISB, CPX, SBC, INC, ISB, INX, SBC, NOP, SBC, CPX, SBC, INC, ISB, // ex
    BEQ, SBC, YYY, ISB, NOP, SBC, INC, ISB, SED, SBC, NOP, ISB, NOP, SBC, INC, ISB, // fx
};

const bool legalOpcodes[] = {
/*  x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xa xb xc xd xe xf  */
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, // 0x
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, // 1x
    1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // 2x
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, // 3x
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // 4x
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, // 5x
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // 6x
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, // 7x
    0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, // 8x
    1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0, // 9x
    1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // ax
    1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // bx
    1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // cx
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, // dx
    1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, // ex
    1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, // fx
};

const int cycleCounts[] = {
/*  x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xa xb xc xd xe xf  */
    7, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6, // 0x
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 1x
    6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6, // 2x
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 3x
    6, 6, 0, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6, // 4x
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 5x
    6, 6, 0, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6, // 6x
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // 7x
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, // 8x
    2, 6, 0, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5, // 9x
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4, // ax
    2, 5, 0, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4, // bx
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, // cx
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // dx
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6, // ex
    2, 5, 0, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7, // fx
};

const int extraCycleCounts[] = {
/*  x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xa xb xc xd xe xf  */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, // 1x
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2x
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, // 3x
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 4x
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, // 5x
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 6x
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, // 7x
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8x
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9x
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // ax
    1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, // bx
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // cx
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, // dx
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // ex
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, // fx
};

instruction CPU::getInstruction(uint8_t opcode) {
    instruction ret = instructionsByOpcode[opcode];
    if (ret == YYY) {
        throw std::runtime_error(std::format("Unsupported opcode #{:04x} in getInstruction.", opcode));
    } else {
        return ret;
    }
}

/*
    Returns the addressing mode for a particular opcode.
*/
addressingMode CPU::getAddressingMode(uint8_t opcode) {
    addressingMode ret = addressingModesByOpcode[opcode];
    if (ret == XXX) {
        throw std::runtime_error(std::format("Unsupported opcode #{:04x} in getAddressingMode.", opcode));
    } else {
        return ret;
    }
}

int CPU::getCycleCount(uint8_t opcode, int cycleOffset) {
    return cycleCounts[opcode] + cycleOffset;
}

/*
    Returns whether a given opcode is a legal opcode for the NES' 6502 CPU.
*/
bool CPU::isLegalOpcode(uint8_t opcode) {
    return legalOpcodes[opcode];
}

int CPU::getCycleCountOffset(instruction inst, addr_t addr, bool extraCycles) {
    int ret = 0;
    switch (inst) {
    case BCC:
        if (!p.c) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BCS:
        if (p.c) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BEQ:
        if (p.z) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BNE:
        if (!p.z) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BMI:
        if (p.n) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BPL:
        if (!p.n) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BVC:
        if (!p.v) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case BVS:
        if (p.v) {
            // Account for crossing page boundary
            ret = 1 + (pc / 0x100 != addr / 0x100);
        }
        break;
    case LAX: // LDA + LDX
    case LDA:
    case LDX:
    case LDY:
    case NOP:
        // Account for crossing page boundary
        ret = (cache / 0x100 != addr / 0x100) * extraCycles;
        break;
    default:
        // No cycles to add
        break;
    }
    return ret;
}

void CPU::runInstruction(addressingMode mode, instruction inst, addr_t addr, uint8_t argument) {
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
            pc = memory->readWord(0xfffe);
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
        case DCP: // DEC + CMP
            // TODO: Fix cycle accuracy by mixing both
            runInstruction(mode, DEC, addr, argument);
            runInstruction(mode, CMP, addr, argument - 1);
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
        case ISB: // INC + SBC
            // TODO: Fix cycle accuracy by mixing both
            runInstruction(mode, INC, addr, argument);
            runInstruction(mode, SBC, addr, argument + 1);
            break;
        case JMP:
            pc = addr;
            break;
        case JSR:
            stackPush(((--pc) & 0xff00) >> 8);
            stackPush(pc & 0xff);
            pc = addr;
            break;
        case LAX: // LDA + LDX
            // TODO: Fix cycle accuracy by mixing both
            runInstruction(mode, LDA, addr, argument);
            runInstruction(mode, LDX, addr, argument);
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
            }
            break;
        case RLA: {// ROL + AND
            uint8_t result = (argument << 1) | p.c;
            p.c = (argument & 0x80) > 0;
            setNZ(result);
            if (mode == NUL) {
                a = result;
            } else {
                memory->write(addr, result);
            }
            a &= result;
            setNZ(a);
            break;
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
        case ROR: {
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
        case RRA: { // ROR + ADC
            uint8_t result = (argument >> 1) | (p.c << 7);
            p.c = argument & 1;
            setNZ(result);
            if (mode == NUL) {
                a = result;
            } else {
                memory->write(addr, result);
            }
            // Use a type large enough to detect carry
            uint16_t result2 = a + p.c + result;
            p.c = result2 > 0xff;
            p.v = ((a ^ result2) & (result ^ result2) & 0x80) != 0;
            a = result2 & 0xff;
            setNZ(a);
            }
            break;
        case RTI: {
            // We ignore changes to the B and I flags
            // See https://www.masswerk.at/6502/6502_instruction_set.html#PLP
            processorFlags oldP = p;
            setProcessorStatus(stackPop());
            p.b1 = oldP.b1;
            p.b2 = oldP.b2;
            p.i = oldP.i;
            uint8_t first = stackPop();
            pc = first | (stackPop() << 8);
            }
            break;
        case RTS: {
            uint8_t first = stackPop();
            pc = (first | (stackPop() << 8)) + 1;
            }
            break;
        case SAX:
            memory->write(addr, a & x);
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
        case SLO: // ASL + ORA
            // TODO: Fix cycle accuracy by mixing both
            runInstruction(mode, ASL, addr, argument);
            runInstruction(mode, ORA, addr, argument << 1);
            break;
        case SRE: // LSR + EOR
            // TODO: Fix cycle accuracy by mixing both
            runInstruction(mode, LSR, addr, argument);
            runInstruction(mode, EOR, addr, argument >> 1);
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
}
