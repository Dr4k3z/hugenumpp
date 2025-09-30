//
// Created by Matteo Campagnoli on 30/09/25.
//

#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include "LargeNum.h"

namespace Algorithms {
    LargeNum computePi(uint64_t groupNum, uint16_t groupSize);
    LargeNum computeE(uint64_t groupNum, uint16_t groupSize, uint64_t iters);
    LargeNum computeSQRT(int target, uint64_t groupNum, uint16_t groupSize, uint64_t iters);
    LargeNum computePhi(uint64_t groupNum, uint16_t groupSize, uint64_t iters);
}

#endif //ALGORITHMS_H
