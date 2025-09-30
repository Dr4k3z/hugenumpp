//
// Created by Matteo Campagnoli on 26/09/25.
//

#ifndef LARGENUM_H
#define LARGENUM_H
#include <memory>
#include <string>
#include <ostream>

class LargeNum {
public:
    LargeNum();
    LargeNum(uint64_t groupNum, uint16_t groupSize);
    LargeNum(const LargeNum& other);

    LargeNum &operator=(const LargeNum &other);

    LargeNum(LargeNum&& other) noexcept = default;
    LargeNum& operator=(LargeNum&& other) noexcept;

    static int compare(const LargeNum& a, const LargeNum& b);
    static LargeNum midpoint(const LargeNum& a, const LargeNum& b){ return (a + b) / 2; }
    static LargeNum fromInt(uint64_t n, uint64_t groupNum, uint16_t groupSize);
    static bool checkSizes(const LargeNum& a, const LargeNum& b);
    static int baseFor(const LargeNum& x){ return static_cast<int>(std::pow(10, x.m_groupSize)); }

    static LargeNum fromInt(const int num, const uint64_t groupNum, const uint16_t groupSize) {
        LargeNum x{groupNum, groupSize}; x.setIntegerPart(num);
        return x;
    }

    LargeNum operator+(const LargeNum& other) const;
    LargeNum operator-(const LargeNum& other) const;
    LargeNum operator*(const LargeNum& other) const;
    LargeNum operator/(int divisor) const;
    LargeNum operator/(const LargeNum& other) const;
    bool operator==(const LargeNum &) const;

    [[nodiscard]] auto getDigit(const size_t idx) const{ return m_data[idx]; }
    [[nodiscard]] auto getGroupNum()const { return m_groupNum; }
    [[nodiscard]] auto getGroupSize() const { return m_groupSize; }
    [[nodiscard]] auto getIntegerPart() const { return m_integerPart; }
    [[nodiscard]] std::string to_string() const;

    void setIntegerPart(const int val) { m_integerPart = val; }

    void print() const;
    void writeToFile(const std::string& filename) const;
    static LargeNum readFromFile(const std::string& filename, uint64_t groupNum, uint16_t groupSize);

    static size_t firstDigitDiff(const LargeNum& a, const LargeNum& b);
    [[nodiscard]] size_t firstDigitDiff(const std::string& filename) const;
    [[nodiscard]] size_t firstDigitDiff(const LargeNum& other) const;
private:
    const uint64_t m_groupNum;
    const uint16_t m_groupSize;
    std::unique_ptr<int[]> m_data;
    long int m_integerPart;
};

#endif //LARGENUM_H
