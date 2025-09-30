//
// Created by Matteo Campagnoli on 26/09/25.
//

#include "gtest/gtest.h"
#include "../src/LargeNum.h"

TEST(SampleTest, DefaultConstructor) {
    LargeNum l;
    ASSERT_EQ(l.getGroupNum(), 1);
    ASSERT_EQ(l.getGroupSize(), 1);
    ASSERT_EQ(l.getIntegerPart(), 0);
}

TEST(SampleTest, Constructor) {
    LargeNum l(4,5);
    ASSERT_EQ(l.getGroupNum(), 4);
    ASSERT_EQ(l.getGroupSize(), 5);
    ASSERT_EQ(l.getIntegerPart(), 0);
}

TEST(SampleTest, Summation) {
    LargeNum l(1,8);
    LargeNum m(1,8);

    l.setIntegerPart(355);
    m.setIntegerPart(113);

    const auto sum = l+m;
    ASSERT_EQ(sum.getIntegerPart(), 468);
}

static LargeNum make_zero(std::uint64_t groups, std::uint16_t groupSize) {
    return LargeNum(groups, groupSize);
}

TEST(LargeNum_Constructors, DefaultCtorIsZeroShaped1x1) {
    LargeNum a;
    LargeNum b;                       // another default
    EXPECT_TRUE(a == b);              // equality works on zeros
}

TEST(LargeNum_Constructors, SizedCtorZeroInitialized) {
    LargeNum a(4, 3);                 // 4 groups, groupSize=3
    LargeNum b(4, 3);
    EXPECT_TRUE(a == b);              // both zero
}

TEST(LargeNum_CopyMove, CopyConstructorCopies) {
    LargeNum a(8, 2);
    LargeNum b(a);                    // copy-construct
    EXPECT_TRUE(a == b);
}

TEST(LargeNum_CopyMove, CopyAssignmentCopies) {
    LargeNum a(8, 2);
    LargeNum b(8, 2);
    b = a;                            // copy-assign
    EXPECT_TRUE(a == b);
}

TEST(LargeNum_Shape, AssignmentThrowsOnMismatchedDimensions) {
    LargeNum a(8, 2);
    LargeNum b(4, 2);                 // different groupNum
    EXPECT_THROW(b = a, std::runtime_error);
}

TEST(LargeNum_Shape, PlusThrowsOnMismatchedDimensions) {
    LargeNum a(8, 2);
    LargeNum b(8, 3);                 // different groupSize
    EXPECT_THROW((void)(a + b), std::runtime_error);
}

TEST(LargeNum_Shape, MinusThrowsOnMismatchedDimensions) {
    LargeNum a(6, 4);
    LargeNum b(5, 4);
    EXPECT_THROW((void)(a - b), std::runtime_error);
}

TEST(LargeNum_Arithmetic, ZeroPlusZeroIsZero) {
    LargeNum a = make_zero(5, 2);
    LargeNum b = make_zero(5, 2);
    LargeNum c = a + b;
    EXPECT_TRUE(c == a);
    EXPECT_TRUE(c == b);
}

TEST(LargeNum_Arithmetic, ZeroMinusZeroIsZero) {
    LargeNum a = make_zero(7, 1);
    LargeNum b = make_zero(7, 1);
    LargeNum c = a - b;
    EXPECT_TRUE(c == a);
    EXPECT_TRUE(c == b);
}

TEST(LargeNum_Arithmetic, MulAndDivLargeNumThrowNotImplemented) {
    LargeNum a(3, 3), b(3, 3);
    EXPECT_THROW((void)(a / b), std::logic_error);
}

TEST(LargeNum_DivisionByInt, DivisionByZeroThrows) {
    LargeNum a(4, 1);
    EXPECT_THROW((void)(a / 0), std::runtime_error);
}

TEST(LargeNum_DivisionByInt, ZeroDivKIsZero) {
    LargeNum a(4, 3);                 // zero value
    LargeNum q = a / 7;
    EXPECT_TRUE(q == a);              // still zero
}

TEST(LargeNum_DiviosnByInt, IntDivision) {
    LargeNum a(10, 8);
    a.setIntegerPart(10);
    auto res = a/2;
    EXPECT_TRUE(res.getIntegerPart() ==  5);
}

bool string_comparison() {
    // remove trailing zeros....
    return false;
}

TEST(LargeNum_DivisionByInt, FloatDivision) {
    LargeNum a(10, 8);
    a.setIntegerPart(10);
    const auto res = a/4;

    EXPECT_TRUE(res.getIntegerPart() == 2);
    ASSERT_EQ(res.to_string(), std::string("2.50000000000000000"));
}