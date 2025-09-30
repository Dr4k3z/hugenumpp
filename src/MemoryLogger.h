#ifndef MEMORYLOGGER_H
#define MEMORYLOGGER_H

#ifdef ENABLE_MEMORY_DIAGNOSTICS

#include <chrono>
#include <string>
#include <cstdio>
#include <iostream>
#include <mutex>

#define MAX_RECORDS 10'000
struct Record{
    void* ptr;
    size_t size;
    bool operator==(const Record& other) const {
        return ptr == other.ptr;
    }
};

class MemoryLogger{
public:
    static MemoryLogger& instance() {
        static MemoryLogger inst;
        return inst;
    }

    MemoryLogger() {
        logAlloc = std::fopen("../log/memoryALL.txt", "w");
        if (!logAlloc) {
            std::cerr << "MemoryLogger: Failed to open memoryALL.txt. Allocation logging disabled.\n";
        }

        logFree = std::fopen("../log/memoryDEL.txt", "w");
        if (!logFree) {
            std::cerr << "MemoryLogger: Failed to open memoryDEL.txt. Free logging disabled.\n";
        }
    }

    void log_allocation(std::size_t size, void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_); // Thread-safe access

        if (logAlloc) {
            std::fprintf(logAlloc, "[ALLOC] %zu bytes at %p - time: %lld\n", size, ptr, nowMillis());
            fflush(logAlloc); // Ensure logs are written immediately (can impact performance)
        }
        counter += 1;
        totalBytes += size;

        auto it = std::find_if(std::begin(records_), std::end(records_), [&](const Record& r) {
            return r.ptr == ptr;
        });
        if (it != std::end(records_)) {
            it->size = size; // overwrite re-used pointer
        }else {
            records_[counter] = Record{ptr, size};
        }
    }

    void log_free(void* ptr){
        std::lock_guard<std::mutex> lock(mutex_);

        if (logFree) {
            std::fprintf(logFree,"[FREE] at %p - time: %lld\n", ptr, nowMillis());
            fflush(logFree);
        }

        auto it = std::find(std::begin(records_), std::end(records_), Record{ptr,0});
        if (it != std::end(records_)) {
            it->size = 0;
        }else {
            throw std::runtime_error("trying to delete non-existent memory\n");
        }
    }

    [[nodiscard]] double getTotalBytes() const{
        return static_cast<double>(totalBytes)/kGiga;
    }

    [[nodiscard]] double getCurrentBytes() {
        std::lock_guard<std::mutex> lock(mutex_);
        double sum = 0.0;
        /*for (const auto& pair : records_) {
            sum += static_cast<double>(pair.second);
        }*/
        for (const auto& el : records_) {
            sum += static_cast<double>(el.size);
        }

        return sum/kGiga;
    }

private:
    ~MemoryLogger() {
        if (logAlloc) {
            std::fclose(logAlloc);
        }
        if (logFree) {
            std::fclose(logFree);
        }
    }

    static long long nowMillis() {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    }

    FILE* logAlloc = nullptr;
    FILE* logFree = nullptr;

    size_t counter = 0;
    size_t totalBytes = 0;
    Record records_[MAX_RECORDS] = {};
    //std::unordered_map<void*,size_t> records_;
    std::mutex mutex_;

    static constexpr double kGiga = 1'000'000'000.0;
};

class ScopedMemoryLogger {
public:
    explicit ScopedMemoryLogger(std::string scope)
        : scopeName(std::move(scope))
    {
        const std::string file = "../log/memory_" + scopeName + ".txt";
        logFile = std::fopen(file.c_str(), "w");
        if (!logFile) {
            std::cerr << "ScopedMemoryLogger: Failed to open " << file << ". Logging disabled for this scope.\n";
        }
        // No mutex for activeLogger, as it's thread_local, each thread has its own.
        activeLogger = this;
    }

    ~ScopedMemoryLogger() {
        if (logFile) {
            std::fclose(logFile);
        }
        if (activeLogger == this) { // Only clear if this is still the active logger for the thread
            activeLogger = nullptr;
        }
    }

    void log(std::size_t size, void* ptr){
        if (!logFile) return;
        auto now = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()).count();

        std::fprintf(logFile, "[ALLOC] %zu bytes at %p - time: %lld\n", size, ptr, ms);
        fflush(logFile); // Ensure logs are written immediately
        total += size;
    }

    static ScopedMemoryLogger* current() {
        return activeLogger;
    }

    [[nodiscard]] double getTotalBytes() const {
        constexpr long long giga = 1'000'000'000;
        return static_cast<double>(total)/giga;
    }

private:
    std::string scopeName;
    FILE* logFile = nullptr;
    size_t total = 0;
    static thread_local ScopedMemoryLogger* activeLogger;
};

#endif // ENABLE_MEMORY_DIAGNOSTICS

#endif