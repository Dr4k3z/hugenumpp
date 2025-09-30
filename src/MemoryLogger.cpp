#include "MemoryLogger.h"
#include <cstdlib>

#ifdef ENABLE_MEMORY_DIAGNOSTICS

void* operator new(const size_t __sz){
    void* ptr = malloc(__sz);
    if (!ptr) throw std::bad_alloc();

    // Scoped Logging
    if (auto* logger = ScopedMemoryLogger::current()) {
        logger->log(__sz, ptr);
    }
    // General Logging
    MemoryLogger::instance().log_allocation(__sz, ptr);

    return ptr;
}

void operator delete(void* __p) noexcept{
    MemoryLogger::instance().log_free(__p);
    std::free(__p);
}

thread_local ScopedMemoryLogger* ScopedMemoryLogger::activeLogger = nullptr;

#else

void* operator new(size_t size){
    return malloc(size);
}

void operator delete(void* ptr) noexcept{
    free(ptr);
}

#endif