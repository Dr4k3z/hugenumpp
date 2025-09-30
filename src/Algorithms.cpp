#include "Algorithms.h"

LargeNum Algorithms::computePi(const uint64_t groupNum, const uint16_t groupSize) {
    LargeNum x{groupNum, groupSize}, y{groupNum, groupSize}, sum{groupNum, groupSize};

    x.setIntegerPart(80); y.setIntegerPart(956);

    for (int k=0; k<15*groupNum; k++) {
        x = x / 25; y = y / 57121;
        auto d = (x-y)/(2*k+1);

        if (k % 2 == 0) {
            if (sum == sum+d) {
                break;
            }
            sum = sum+d;
        }else {
            if (sum == sum+d) {
                break;
            }
            sum = sum - d;
        }
    }

    return sum;
}

LargeNum Algorithms::computeSQRT(const int target, const uint64_t groupNum, const uint16_t groupSize, const uint64_t iters) {
    const LargeNum target_num = LargeNum::fromInt(target, groupNum, groupSize);
    LargeNum lo = LargeNum::fromInt(0, groupNum, groupSize);
    LargeNum hi = LargeNum::fromInt(std::max(1, target), groupNum, groupSize);

    for (int t = 0; t < iters; ++t) {
        LargeNum mid = LargeNum::midpoint(lo, hi);

        if (const LargeNum mid2 = mid*mid; LargeNum::compare(mid2, target_num) > 0) {
            hi = mid;
        } else {
            lo = mid;
        }
    }
    return LargeNum::midpoint(lo, hi);
}

LargeNum Algorithms::computeE(const uint64_t groupNum, const uint16_t groupSize, const uint64_t iters) {
    LargeNum sum{groupNum, groupSize};
    LargeNum term{groupNum, groupSize};
    LargeNum new_sum{groupNum, groupSize};

    term.setIntegerPart(1);

    const uint64_t MAX_ITERS = groupNum * groupSize + 10;

    for (uint64_t n = 0; n < std::min(MAX_ITERS, iters); ++n) {
        new_sum = sum + term;

        if (new_sum == sum) break;
        sum = new_sum;

        term = term / static_cast<int>(n + 1);
    }

    return sum;
}

LargeNum Algorithms::computePhi(const uint64_t groupNum, const uint16_t groupSize, const uint64_t iters) {
    const auto sqrt5 = computeSQRT(5, groupNum, groupSize, iters);
    auto one = LargeNum{groupNum, groupSize}; one.setIntegerPart(1);
    const auto numerator = one+sqrt5;
    return numerator/2;
}
