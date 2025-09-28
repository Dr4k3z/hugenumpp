#include <iostream>
#include "src/LargeNum.h"

LargeNum sqrt42(const uint64_t groupNum, const uint16_t groupSize, const int iters) {
    LargeNum lo = LargeNum::fromInt(6, groupNum, groupSize);
    LargeNum hi = LargeNum::fromInt(7, groupNum, groupSize);
    const LargeNum fortyTwo = LargeNum::fromInt(42, groupNum, groupSize);

    for (int t = 0; t < iters; ++t) {
        LargeNum mid = LargeNum::midpoint(lo, hi);

        if (const LargeNum mid2 = mid*mid; LargeNum::compare(mid2, fortyTwo) > 0) {
            // mid^2 > 42 -> root is below mid
            hi = mid;
        } else {
            // mid^2 <= 42 -> root is at/above mid
            lo = mid;
        }
    }
    return LargeNum::midpoint(lo, hi);
}

int main() {
    constexpr uint64_t groupNum = 12;
    constexpr uint16_t groupSize = 8;
    constexpr int iter = 250;

    const auto res = sqrt42(groupNum, groupSize, iter);
    const auto actual = LargeNum::readFromFile("../sqrt42_1000.txt", groupNum, groupSize);

    std::cout << "The two numbers match until the " << LargeNum::firstDigitDiff(res, actual) << "-th digit\n";
    return 0;
}