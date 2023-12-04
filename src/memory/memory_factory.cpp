#include "memory_factory.h"
#include "core_memory.h"
#include "mapper000.h"
#include "mapper001.h"
#include <stdexcept>
#include <string>

std::unique_ptr<CoreMemory> MemoryFactory::create(int mapper) {
    switch (mapper) {
        case 000:
            return std::make_unique<Mapper000>();
        case 001:
            return std::make_unique<Mapper001>();
        default:
            throw std::runtime_error("Unsupported mapper " + std::to_string(mapper) + " encountered."); 
    }
}
