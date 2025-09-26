//
// Created by Matteo Campagnoli on 26/09/25.
//

#include "LargeNum.h"

LargeNum::LargeNum() : m_groupNum(1), m_groupSize(1), m_integerPart(0) {
    m_data = std::make_unique<int[]>(1);
}

LargeNum::LargeNum(const uint64_t groupNum, const uint16_t groupSize) : m_groupNum(groupNum), m_groupSize(groupSize), m_integerPart(0) {
    m_data = std::make_unique<int[]>(groupNum);
    std::fill_n(m_data.get(), groupNum, 0);
}

LargeNum::LargeNum(const LargeNum &other) : m_groupNum(other.m_groupNum), m_groupSize(other.m_groupSize), m_integerPart(other.m_integerPart) {
    m_data = std::make_unique<int[]>(m_groupNum);
    std::copy_n(other.m_data.get(), m_groupNum, m_data.get());
}

bool checkSizes(const LargeNum& l1, const LargeNum& l2) {
    if (l1.getGroupNum() != l2.getGroupNum() || l1.getGroupSize() != l2.getGroupSize()) {
        return false;
    }
    return true;
}

LargeNum& LargeNum::operator=(const LargeNum& other) {
    if (this == &other) { return *this; }

    if (!checkSizes(*this, other)){
        throw std::runtime_error("Mismatched dimensions"); // TODO is this really a run time error? Can't it be compiled time!
    }

    m_integerPart = other.m_integerPart;
    std::copy_n(other.m_data.get(), m_groupNum, m_data.get());
    return *this;
}

LargeNum LargeNum::operator+(const LargeNum& other) const {
    if (!checkSizes(*this, other)) {
        throw std::runtime_error("Mismatched dimensions"); // TODO handle this better may create custom exception class
    }

    LargeNum res(m_groupNum, m_groupSize);
    const int base = static_cast<int>(std::pow(10, m_groupSize));
    int carry = 0;

    for (auto i = static_cast<int>(m_groupNum-1); i >= 0; --i) {
        const int sum = m_data[i] + other.m_data[i] + carry;
        res.m_data[i] = sum % base;
        carry = sum/base;
    }

    res.m_integerPart = m_integerPart + other.m_integerPart + carry;
    return res;
}

LargeNum LargeNum::operator-(const LargeNum &other) const {
    if (!checkSizes(*this, other)) {
        throw std::runtime_error("Mismatched dimensions"); // TODO handle this better with custom exception class
    }

    LargeNum res(m_groupNum, m_groupSize);
    const int base = static_cast<int>(std::pow(10, m_groupSize));
    int borrow = 0;

    for (int i = static_cast<int>(m_groupNum)-1; i >= 0; --i) {
        const int lhs = m_data[i];

        if (const int rhs = m_data[i]; lhs < rhs) {
            res.m_data[i] = lhs + base - rhs;
            borrow = 1;
        } else {
            res.m_data[i] = lhs - rhs;
            borrow = 0;
        }
    }

    res.m_integerPart = m_integerPart - other.m_integerPart - borrow;
    return res;
}

class NotImplementedError : public std::logic_error {
public:
    NotImplementedError() : std::logic_error("Function not yet implemented") {}
};

LargeNum LargeNum::operator*(const LargeNum &other) const {
    throw NotImplementedError{};
}

LargeNum LargeNum::operator/(const LargeNum &other) const {
    throw NotImplementedError{};
}

LargeNum LargeNum::operator/(int divisor) const {
    if (divisor == 0) {
        throw std::runtime_error("Division by zero");
    }
    LargeNum result(m_groupNum, m_groupSize);
    int base = static_cast<int>(std::pow(10, m_groupSize));
    int64_t carry = m_integerPart;

    result.m_integerPart = static_cast<int>(carry/divisor);
    carry = carry & divisor;

    for (size_t i=0; i<m_groupNum; i++) {
        carry = carry*base + m_data[i];
        result.m_data[i] = static_cast<int>(carry/divisor);
        carry %= divisor;
    }

    return result;
}

bool LargeNum::operator==(const LargeNum& other) const {
    if (!checkSizes(*this, other)) {
        // throw std::runtime_error("Dimensions not matched"); // TODO EXCEPTION
        return false;
    }
    for (size_t i = 0; i<m_groupNum; i++) {
        if (other.m_data[i] != m_data[i]) {
            return false;
        }
    }
    return true;
}
