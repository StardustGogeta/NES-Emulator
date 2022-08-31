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
    "LAX", "YYY"
};

// Table of addressing modes by opcode
const addressingMode addressingModesByOpcode[] = {
/*  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xa   xb   xc   xd   xe   xf   */
    NUL, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // 0x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPX, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABX, XXX, // 1x
    ABS, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // 2x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPX, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABX, XXX, // 3x
    NUL, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // 4x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPX, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABX, XXX, // 5x
    NUL, IZX, XXX, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, IND, ABS, ABS, XXX, // 6x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPX, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABX, XXX, // 7x
    IMM, IZX, IMM, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // 8x
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPY, XXX, NUL, ABY, NUL, XXX, XXX, ABX, XXX, XXX, // 9x
    IMM, IZX, IMM, IZX, ZPG, ZPG, ZPG, ZPG, NUL, IMM, NUL, IMM, ABS, ABS, ABS, ABS, // ax
    REL, IZY, XXX, IZY, ZPX, ZPX, ZPY, ZPY, NUL, ABY, NUL, XXX, ABX, ABX, ABY, ABY, // bx
    IMM, IZX, IMM, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // cx
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPX, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABX, XXX, // dx
    IMM, IZX, IMM, XXX, ZPG, ZPG, ZPG, XXX, NUL, IMM, NUL, XXX, ABS, ABS, ABS, XXX, // ex
    REL, IZY, XXX, XXX, ZPX, ZPX, ZPX, XXX, NUL, ABY, NUL, XXX, ABX, ABX, ABX, XXX, // fx
};

const instruction instructionsByOpcode[] = {
/*  x0   x1   x2   x3   x4   x5   x6   x7   x8   x9   xa   xb   xc   xd   xe   xf   */
    BRK, ORA, YYY, YYY, NOP, ORA, ASL, YYY, PHP, ORA, ASL, YYY, NOP, ORA, ASL, YYY, // 0x
    BPL, ORA, YYY, YYY, NOP, ORA, ASL, YYY, CLC, ORA, NOP, YYY, NOP, ORA, ASL, YYY, // 1x
    JSR, AND, YYY, YYY, BIT, AND, ROL, YYY, PLP, AND, ROL, YYY, BIT, AND, ROL, YYY, // 2x
    BMI, AND, YYY, YYY, NOP, AND, ROL, YYY, SEC, AND, NOP, YYY, NOP, AND, ROL, YYY, // 3x
    RTI, EOR, YYY, YYY, NOP, EOR, LSR, YYY, PHA, EOR, LSR, YYY, JMP, EOR, LSR, YYY, // 4x
    BVC, EOR, YYY, YYY, NOP, EOR, LSR, YYY, CLI, EOR, NOP, YYY, NOP, EOR, LSR, YYY, // 5x
    RTS, ADC, YYY, YYY, NOP, ADC, ROR, YYY, PLA, ADC, ROR, YYY, JMP, ADC, ROR, YYY, // 6x
    BVS, ADC, YYY, YYY, NOP, ADC, ROR, YYY, SEI, ADC, NOP, YYY, NOP, ADC, ROR, YYY, // 7x
    NOP, STA, NOP, YYY, STY, STA, STX, YYY, DEY, NOP, TXA, YYY, STY, STA, STX, YYY, // 8x
    BCC, STA, YYY, YYY, STY, STA, STX, YYY, TYA, STA, TXS, YYY, YYY, STA, YYY, YYY, // 9x
    LDY, LDA, LDX, LAX, LDY, LDA, LDX, LAX, TAY, LDA, TAX, LAX, LDY, LDA, LDX, LAX, // ax
    BCS, LDA, YYY, LAX, LDY, LDA, LDX, LAX, CLV, LDA, TSX, YYY, LDY, LDA, LDX, LAX, // bx
    CPY, CMP, NOP, YYY, CPY, CMP, DEC, YYY, INY, CMP, DEX, YYY, CPY, CMP, DEC, YYY, // cx
    BNE, CMP, YYY, YYY, NOP, CMP, DEC, YYY, CLD, CMP, NOP, YYY, NOP, CMP, DEC, YYY, // dx
    CPX, SBC, NOP, YYY, CPX, SBC, INC, YYY, INX, SBC, NOP, YYY, CPX, SBC, INC, YYY, // ex
    BEQ, SBC, YYY, YYY, NOP, SBC, INC, YYY, SED, SBC, NOP, YYY, NOP, SBC, INC, YYY, // fx
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

instruction CPU::getInstruction(uint8_t opcode) {
    instruction ret = instructionsByOpcode[opcode];
    if (ret == YYY) {
        char buf[3];
        sprintf(buf, "%02x", opcode);
        throw std::runtime_error("Unsupported opcode 0x" + std::string(buf) + " in getInstruction.");
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
        char buf[3];
        sprintf(buf, "%02x", opcode);
        throw std::runtime_error("Unsupported opcode 0x" + std::string(buf) + " in getAddressingMode.");
    } else {
        return ret;
    }
}

/*
    Returns whether a given opcode is a legal opcode for the NES' 6502 CPU.
*/
bool CPU::isLegalOpcode(uint8_t opcode) {
    return legalOpcodes[opcode];
}

void CPU::runInstruction(addressingMode mode, instruction inst, CoreMemory::addr_t addr, uint8_t argument) {
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
        case LAX:
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
}
