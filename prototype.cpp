#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

struct  LargeNum {
    LargeNum(const uint64_t groupNum, const uint16_t groupSize): m_groupNum(groupNum), m_groupSize(groupSize), m_integerPart(0) {
        m_data = std::make_unique<int[]>(groupNum);
        std::fill_n(m_data.get(), groupNum, 0);
    }

    LargeNum(const LargeNum &other) : m_groupNum(other.m_groupNum), m_groupSize(other.m_groupSize), m_integerPart(other.m_integerPart) {
        m_data = std::make_unique<int[]>(m_groupNum);
        std::copy_n(other.m_data.get(), m_groupNum, m_data.get());
    }

    LargeNum& operator=(const LargeNum& other) {
        if (this == &other) { return *this; }

        if (!checkSizes(*this, other)){
            throw std::runtime_error("Mismatched dimensions");
        }

        m_integerPart = other.m_integerPart;
        std::copy_n(other.m_data.get(), m_groupNum, m_data.get());
        return *this;
    }

    static int compare(const LargeNum& a, const LargeNum& b) {
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

    static LargeNum midpoint(const LargeNum& lo, const LargeNum& hi) {
        return (lo + hi) / 2;
    }

    static LargeNum from_int(const uint64_t n, const uint64_t groupNum, const uint16_t groupSize) {
        LargeNum a(groupNum, groupSize);
        a.m_integerPart = static_cast<long int>(n);
        return a;
    }

    static bool checkSizes(const LargeNum& a, const LargeNum& b) {
        if (a.m_groupNum != b.m_groupNum || a.m_groupSize != b.m_groupSize) {
            return false;
        }
        return true;
    }

    static int base_for(const LargeNum& x) {
        return static_cast<int>(std::pow(10, x.m_groupSize));
    }

    LargeNum operator+(const LargeNum& other) const {
        if (!checkSizes(*this, other)) {
            throw std::runtime_error("Mismatched dimensions");
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

    LargeNum operator-(const LargeNum& other) const {
        if (!checkSizes(*this, other)) {
            throw std::runtime_error("Mismatched dimensions");
        }

        LargeNum res(m_groupNum, m_groupSize);
        const int base = static_cast<int>(std::pow(10, m_groupSize));
        int borrow = 0;

        for (int i = static_cast<int>(m_groupNum)-1; i >= 0; --i) {
            const int lhs = m_data[i];

            if (const int rhs = other.m_data[i]; lhs < rhs) {
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

    LargeNum operator*(const LargeNum& other) const {
        if (!checkSizes(*this, other)) {
            throw std::runtime_error("mul: mismatched dimensions");
        }
        const int B = base_for(*this);
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

    LargeNum operator/(const int divisor) const {
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

    [[nodiscard]] std::string to_string() const {
        std::string str = std::to_string(m_integerPart) + ".";
        for (size_t i = 0; i < m_groupNum; i++) {
            str += std::to_string(m_data[i]);
        }
        return str;
    }

    void print() const {
        std::cout << this->to_string() << std::endl;
    }

    const uint64_t m_groupNum;
    const uint16_t m_groupSize;
    std::unique_ptr<int[]> m_data;
    long int m_integerPart;
};

LargeNum sqrt42(const uint64_t groupNum, const uint16_t groupSize, const int iters = 200) {
    LargeNum lo = LargeNum::from_int(6, groupNum, groupSize);
    LargeNum hi = LargeNum::from_int(7, groupNum, groupSize);
    const LargeNum fortyTwo = LargeNum::from_int(42, groupNum, groupSize);

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

    const LargeNum r = sqrt42(groupNum, groupSize, 250);
    // r matches the sqrt(42) up the 72-th digit
    r.print();
    return 0;
}
