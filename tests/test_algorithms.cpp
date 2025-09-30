#include "gtest/gtest.h"
#include "../src/Algorithms.h"

TEST(Compute_PI, thirtyDigits) {
    constexpr uint64_t groupNum = 8;
    constexpr uint16_t groupSize = 4;

    const auto pi = Algorithms::computePi(groupNum, groupSize);
    const auto true_pi = LargeNum::readFromFile("../assets/pi_1000.txt", groupNum, groupSize);

    const size_t idx = LargeNum::firstDigitDiff(pi, true_pi);

    ASSERT_GT(idx, 30);
}

TEST(COMPUTE_SQRT, seventyDigits) {
    constexpr uint64_t groupNum = 12;
    constexpr uint16_t groupSize = 8;

    const auto sqrt = Algorithms::computeSQRT(42, groupNum, groupSize, 250);
    const auto true_sqrt = LargeNum::readFromFile("../assets/sqrt42_1000.txt", groupNum, groupSize);

    const size_t idx = LargeNum::firstDigitDiff(sqrt, true_sqrt);

    ASSERT_GT(idx, 70);
}

TEST(COMPUTE_SQRT, fiftyDigits) {
    constexpr uint64_t groupNum = 12;
    constexpr uint16_t groupSize = 8;

    const auto sqrt = Algorithms::computeSQRT(2, groupNum, groupSize, 180);
    const auto true_sqrt = LargeNum::readFromFile("../assets/sqrt2_1000.txt", groupNum, groupSize);

    const size_t idx = LargeNum::firstDigitDiff(sqrt, true_sqrt);

    ASSERT_GT(idx, 50);
}

TEST(COMPUTE_SQRT, twentyDigits) {
    constexpr uint64_t groupNum = 12;
    constexpr uint16_t groupSize = 8;

    const auto sqrt = Algorithms::computeSQRT(11, groupNum, groupSize, 100);
    const auto true_sqrt = LargeNum::readFromFile("../assets/sqrt11_1000.txt", groupNum, groupSize);

    const size_t idx = LargeNum::firstDigitDiff(sqrt, true_sqrt);

    ASSERT_GT(idx, 20);
}