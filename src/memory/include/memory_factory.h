#include "core_memory.h"

/*
    This class handles the creation of new memory objects.
*/
class MemoryFactory {
    public:
        static CoreMemory* create(int mapper);
};
