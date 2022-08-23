#include "rom.h"
#include "memory.h"
#include "cpu.h"

class NES {
    public:
        NES();
        void loadROM(ROM* rom);
    private:
        Memory* memory;
        CPU* cpu;
};
