//
// Created by Matteo Campagnoli on 26/09/25.
//

#include "LargeNum.h"

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <cassert>

class NotImplementedError final : public std::logic_error {
public:
    NotImplementedError() : std::logic_error("Function not yet implemented") {}
};

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

LargeNum& LargeNum::operator=(LargeNum&& other) noexcept {
    assert(checkSizes(*this, other) && "move-assign requires same dimensions");

    if (this == &other) return *this;

    m_integerPart = other.m_integerPart;
    m_data = std::move(other.m_data);
    return *this;
}

bool LargeNum::checkSizes(const LargeNum& a, const LargeNum& b) {
    if (a.m_groupNum != b.m_groupNum || a.m_groupSize != b.m_groupSize) {
        return false;
    }
    return true;
}

int LargeNum::compare(const LargeNum& a, const LargeNum& b) {
    if (!LargeNum::checkSizes(a, b)) {
        throw std::runtime_error("compare: mismatched dimensions");
    }
    if (a.m_integerPart < b.m_integerPart) return -1;
    if (a.m_integerPart > b.m_integerPart) return 1;

    for (size_t i = 0; i < a.m_groupNum; ++i) {
        if (a.m_data[i] < b.m_data[i]) return -1;
        if (a.m_data[i] > b.m_data[i]) return 1;
    }
    return 0;
}

LargeNum LargeNum::fromInt(const uint64_t n, const uint64_t groupNum, const uint16_t groupSize) {
    LargeNum a(groupNum, groupSize);
    a.m_integerPart = static_cast<long int>(n);
    return a;
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

LargeNum& LargeNum::operator=(const LargeNum& other) {
    if (this == &other) { return *this; }

    if (!checkSizes(*this, other)){
        throw std::runtime_error("Mismatched dimensions"); // TODO is this really a run time error? Can't it be compiled time!
    }

    m_integerPart = other.m_integerPart;
    std::copy_n(other.m_data.get(), m_groupNum, m_data.get());
    return *this;
}

LargeNum LargeNum::operator-(const LargeNum &other) const {
    if (!checkSizes(*this, other)) throw std::runtime_error("Mismatched dimensions");
    LargeNum res(m_groupNum, m_groupSize);
    const int base = static_cast<int>(std::pow(10, m_groupSize));
    int borrow = 0;

    for (int i = static_cast<int>(m_groupNum)-1; i >= 0; --i) {
        long long lhs = static_cast<long long>(m_data[i]) - borrow;
        const long long rhs = other.m_data[i];
        if (lhs < rhs) {
            lhs += base;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res.m_data[i] = static_cast<int>(lhs - rhs);
    }

    // integer part with final borrow
    res.m_integerPart = m_integerPart - other.m_integerPart - borrow;
    return res;
}

LargeNum LargeNum::operator*(const LargeNum &other) const {
    if (!checkSizes(*this, other)) {
        throw std::runtime_error("mul: mismatched dimensions");
    }
    const int B = baseFor(*this);
    const size_t F = m_groupNum;
    const size_t L = F + 1;

    std::vector<long long> A(L), C(L), D(2 * L, 0);
    for (size_t i = 0; i < L; ++i) {
        A[i] = (i == 0) ? m_integerPart : other.m_data[i - 1];
        C[i] = (i == 0) ? m_integerPart : other.m_data[i - 1];
    }

    // Convolution in base-B digits (fixed point)
    for (size_t i = 0; i < L; ++i) {
        for (size_t j = 0; j < L; ++j) {
            D[i + j] += A[i] * C[j];
        }
    }

    for (int k = static_cast<int>(D.size()) - 1; k > 0; --k) {
        const long long carry = D[k] / B;
        D[k] %= B;
        D[k - 1] += carry;
    }

    LargeNum z(m_groupNum, other.m_groupSize);
    z.m_integerPart = static_cast<long int>(D[0]);
    for (size_t i = 0; i < F; ++i) {
        z.m_data[i] = static_cast<int>(D[i + 1]);
    }
    return z;
}

LargeNum LargeNum::operator/(const int divisor) const {
    if (divisor == 0) {
        throw std::runtime_error("Division by zero");
    }
    LargeNum result(m_groupNum, m_groupSize);
    const int base = static_cast<int>(std::pow(10, m_groupSize));
    int64_t carry = m_integerPart;

    result.m_integerPart = static_cast<int>(carry/divisor);
    carry %= divisor;

    for (size_t i=0; i<m_groupNum; ++i) {
        carry = carry*base + m_data[i];
        result.m_data[i] = static_cast<int>(carry/divisor);
        carry %= divisor;
    }

    return result;
}

LargeNum LargeNum::operator/(const LargeNum &other) const {
    throw NotImplementedError{};
}


bool LargeNum::operator==(const LargeNum& other) const {
    if (!checkSizes(*this, other)) {
        // throw std::runtime_error("Dimensions not matched"); // TODO EXCEPTION
        return false;
    }
    if (m_integerPart != other.m_integerPart) {
        return false;
    }
    for (size_t i = 0; i<m_groupNum; i++) {
        if (other.m_data[i] != m_data[i]) {
            return false;
        }
    }
    return true;
}

std::string LargeNum::to_string() const {
    std::string str = std::to_string(m_integerPart) + ".";
    for (size_t i = 0; i < m_groupNum; i++) {
        str += std::to_string(m_data[i]);
    }
    return str;
}


void LargeNum::print() const {
    std::cout << this->to_string() << std::endl;
}

void LargeNum::writeToFile(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Cannot open file" + filename);

    out << to_string();
}


LargeNum LargeNum::readFromFile(const std::string& filename, const uint64_t groupNum, const uint16_t groupSize) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Cannot open file: " + filename);

    if (groupSize > 9) throw std::runtime_error("groupSize > 9 is not supported with int-based groups. "
                                                "Use groupSize <= 9, or change LargeNum.m_data to 64-bit."); // TODO: I dont like this limitation

    std::string s;
    in >> s;
    auto dotPos = s.find('.');
    if (dotPos == std::string::npos) {
        throw std::runtime_error("No decimal point found");
    }

    LargeNum out(groupNum, groupSize);

    // integer part
    out.m_integerPart = std::stol(s.substr(0, dotPos));

    // fractional part as a plain string of digits
    std::string frac = s.substr(dotPos + 1);

    // pad with zeros if not long enough for all groups
    if (const size_t needed = groupNum * groupSize; frac.size() < needed) {
        frac.append(needed - frac.size(), '0');
    }

    for (size_t g = 0; g < groupNum; ++g) {
        size_t start = g * groupSize;
        std::string chunk = frac.substr(start, groupSize);
        out.m_data[g] = static_cast<int>(std::stoi(chunk));
    }

    return out;
}


size_t LargeNum::firstDigitDiff(const LargeNum& a, const LargeNum& b){
    if (!checkSizes(a, b)) {
        throw std::runtime_error("Mismatched LargeNum dimensions");
    }

    // Convert both to strings with zero-padded groups so comparison is per digit
    std::ostringstream sa, sb;
    sa << a.m_integerPart << ".";
    sb << b.m_integerPart << ".";
    for (size_t i = 0; i < a.m_groupNum; ++i) {
        sa << std::setw(a.m_groupSize) << std::setfill('0') << a.m_data[i];
        sb << std::setw(b.m_groupSize) << std::setfill('0') << b.m_data[i];
    }

    std::string sa_str = sa.str();
    std::string sb_str = sb.str();

    std::erase(sa_str, '.');
    std::erase(sb_str, '.');

    size_t n = std::min(sa_str.size(), sb_str.size());
    for (size_t i = 0; i < n; ++i) {
        if (sa_str[i] != sb_str[i]) {
            return static_cast<int>(i + 1);
        }
    }

    return a.m_groupNum*a.m_groupSize;
}


size_t LargeNum::firstDigitDiff(const std::string &filename) const {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("pi.txt not found or couldn't be opened");
    }

    std::string other;
    std::getline(file, other);
    if (other.empty()) {
       throw std::runtime_error("Invalid txt format! Can't read the number");
    }

    auto dot = other.find('.');
    // TODO: check also integer PART
    other = other.substr(dot+1);

    std::string valStr;
    for (int i=0; i < m_groupNum; i++) {
        int groupVal = this->getDigit(i);
        std::ostringstream oss;
        oss << std::setw(m_groupSize) << std::setfill('0') << groupVal;
        valStr += oss.str();
    }

    int matched = 0;
    for (size_t i = 0; i < valStr.size() && i < other.size(); ++i) {
        if (valStr[i] == other[i]) {
            ++matched;
        } else {
            // return idx mismatch
            return i;
        }
    }

    return std::min(valStr.size(), other.size());
}

size_t LargeNum::firstDigitDiff(const LargeNum &other) const {
    std::string other_str = other.to_string();

    const auto dot = other_str.find('.');
    // TODO: check also integer PART
    other_str = other_str.substr(dot+1);

    std::string valStr;
    for (int i=0; i < m_groupNum; i++) {
        const int groupVal = this->getDigit(i);
        std::ostringstream oss;
        oss << std::setw(m_groupSize) << std::setfill('0') << groupVal;
        valStr += oss.str();
    }

    int matched = 0;
    for (size_t i = 0; i < valStr.size() && i < other_str.size(); ++i) {
        if (valStr[i] == other_str[i]) {
            ++matched;
        } else {
            // return idx mismatch
            return i;
        }
    }

    return std::min(valStr.size(), other_str.size());
}
