#include "core_memory.h"
#include <memory>

/*
    This class handles the creation of new memory objects.
*/
class MemoryFactory {
    public:
        static std::unique_ptr<CoreMemory> create(int mapper);
};
