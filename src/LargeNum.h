//
// Created by Matteo Campagnoli on 26/09/25.
//

#ifndef LARGENUM_H
#define LARGENUM_H
#include <iomanip>
#include <memory>
#include <string>
#include <ostream>

class LargeNum {
public:
    LargeNum();
    LargeNum(uint64_t groupNum, uint16_t groupSize);
    LargeNum(const LargeNum& other);

    LargeNum &operator=(const LargeNum &other);

    LargeNum(LargeNum&& other) noexcept;
    LargeNum& operator=(LargeNum&& other) noexcept;

    LargeNum operator+(const LargeNum& other) const;
    LargeNum operator-(const LargeNum& other) const;
    LargeNum operator*(const LargeNum& other) const;
    LargeNum operator/(const LargeNum& other) const;
    LargeNum operator/(const int divisor) const;
    bool operator==(const LargeNum &) const;

    [[nodiscard]] auto getDigit(const size_t index) const;
    [[nodiscard]] auto getGroupNum()const { return m_groupNum; }
    [[nodiscard]] auto getGroupSize() const { return m_groupSize; }
    [[nodiscard]] auto getIntegerPart() const { return m_integerPart; }

    void setDigit(const size_t index, const int val);
    void setIntegerPart(const int val) { m_integerPart = val; }

    void print() const;
    void write(const std::string& filename);

    static std::unique_ptr<LargeNum> load(const std::string& filename);
private:
    const uint64_t m_groupNum;
    const uint16_t m_groupSize;
    std::unique_ptr<int[]> m_data;
    long int m_integerPart;
};



#endif //LARGENUM_H
