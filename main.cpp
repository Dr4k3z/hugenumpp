#include "src/LargeNum.h"
#include "src/Algorithms.h"
#include "src/Timer.h"
#include "src/MemoryLogger.h"

int main() {
    {
        Timer t("computePi_timer");

        #ifdef ENABLE_MEMORY_DIAGNOSTICS
        const ScopedMemoryLogger logger("computePi_memory");
        #endif

        constexpr uint64_t groupNum = 200;
        constexpr uint16_t groupSize = 9;

        const auto pi = Algorithms::computePi(groupNum,groupSize);

        #ifdef ENABLE_MEMORY_DIAGNOSTICS
        std::cout << "Total gigabytes allocated: " << logger.getTotalBytes() << std::endl;
        #endif
    }
    {
        Timer t("computeE_timer");
        #ifdef ENABLE_MEMORY_DIAGNOSTICS
        const ScopedMemoryLogger logger("computeE_memory");
        #endif

        constexpr uint64_t groupNum = 200;
        constexpr uint16_t groupSize = 9;

        auto e = Algorithms::computeE(groupNum,groupSize, 250);

        #ifdef ENABLE_MEMORY_DIAGNOSTICS
        std::cout << "Total gigabytes allocated: " << logger.getTotalBytes() << std::endl;
        #endif
    }
    return 0;
}