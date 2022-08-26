#include "rom.h"
#include "memory.h"
#include "cpu.h"

class NES {
    public:
        NES();
        void loadROM(ROM* rom);

        Memory* memory;
        CPU* cpu;
};
