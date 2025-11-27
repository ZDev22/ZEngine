#pragma once

#include <string.h>

// Use unsigned long long for precise, large numbers
template<unsigned long long bitCount>
struct bigInt {
    static constexpr unsigned int bytes = bitCount / 64;
    unsigned long long limbs[bytes] = {0};

    constexpr bigInt() = default;
    template<typename T>
    constexpr bigInt(const T v) { limbs[0] = v; }
    constexpr bigInt(const unsigned long long* init, unsigned int count) {
        unsigned int limit = count < bytes ? count : bytes;
        unsigned int i = 0;
        for (; i < limit; i++) { limbs[i] = init[i]; }
        memset(&limbs[i], 0, (bytes - i) * 8);
    }

    inline bool isZero() const { for (unsigned int i = 0; i < bytes; i++) { if (limbs[i] != 0) return false; } return true; }
    inline void setZero() { memset(limbs, 0, bytes); }
    unsigned long long mod(unsigned long long m) {
        if (m == 0) { return 0; }
        __uint128_t rem = 0;
        for (int i = bytes - 1; i >= 0; i--) {
            __uint128_t cur = (rem << 64) | limbs[i];
            limbs[i] = cur / m;
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
    int bitWidth() const {
        for (int i = bytes - 1; i >= 0; i--) {
            if (limbs[i] != 0) {
                int width = 0;
                unsigned long long val = limbs[i];
                while (val) { ++width; val >>= 1; }
                return i * 64 + width;
            }
        }
        return 0;
    }

    void operator=(const bigInt& rhs) { memcpy(limbs, rhs.limbs, bytes); }
    template<typename T>
    void operator=(const T rhs) {
        setZero();
        limbs[0] = rhs;
    }

    bigInt operator+(const bigInt& rhs) const {
        bigInt result;
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            __uint128_t sum = static_cast<__uint128_t>(limbs[i]) + rhs.limbs[i] + carry;
            result.limbs[i] = sum;
            carry = sum >> 64;
        }
        return result;
    }
    template<typename T> bigInt operator+(const T rhs) const { 
        __uint128_t sum = static_cast<__uint128_t>(limbs[0] + rhs);
        bigInt result = (unsigned long long)sum;

        __uint128_t carry = sum >> 64;
        if (carry == 0) { return result; }
        for (unsigned int i = 1; i < bytes; i++) {
            sum = limbs[i] + carry;
            result.limbs[i] = (unsigned long long)sum;
            carry = sum >> 64;
            if (carry == 0) { break; }
        }
        return result;
    }
    void operator+=(const bigInt& rhs) { *this = *this + rhs; }
    template<typename T> void operator+=(const T rhs) { *this = *this + rhs; }

    bigInt operator-(const bigInt& rhs) const {
        bigInt result;
        __uint128_t borrow;
        for (unsigned int i = 0; i < bytes; i++) {
            unsigned long long tmp = limbs[i] - rhs.limbs[i] - borrow;
            borrow = static_cast<__uint128_t>(limbs[i]) < static_cast<__uint128_t>(rhs.limbs[i]) + borrow ? 1 : 0;
            result.limbs[i] = tmp;
        }
        return result;
    }
    template<typename T> bigInt operator-(const T rhs) const {
        bigInt result = *this;
        unsigned long long borrow = rhs;
        for (unsigned int i = 0; i < bytes; ++i) {
            unsigned long long old = result.limbs[i];
            if (old >= borrow) {
                result.limbs[i] = old - borrow;
                borrow = 0;
                break;
            }
            else {
                result.limbs[i] = old - borrow;
                borrow = 1;
            }
        }
        return result;
    }
    void operator-=(const bigInt& rhs) { *this = *this - rhs; }
    template<typename T> void operator-=(const T rhs) { *this = *this - rhs; }

    bigInt operator*(const bigInt& rhs) const {
        unsigned long long result[bytes] = {0};
        __uint128_t tmp[bytes * 2] = {0};
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; i++) { for (unsigned int j = 0; j < bytes; ++j) { if (i + j < bytes * 2) { tmp[i + j] += static_cast<__uint128_t>(limbs[i]) * rhs.limbs[j]; }}}
        for (unsigned int i = 0; i < bytes; i++) {
            tmp[i] += carry;
            result[i] = tmp[i];
            carry = tmp[i] >> 64;
        }
        return bigInt(result, bytes);
    }
    template<typename T> bigInt operator*(const T rhs) const { return *this * bigInt(rhs); }
    void operator*=(const bigInt& rhs) { *this = *this * rhs; }
    template<typename T> void operator*=(const T rhs) { *this = *this * bigInt(rhs); }

    bigInt operator/(const bigInt& divisor) const {
        if (divisor.isZero()) { return 0; }
        int bw = bitWidth();
        if (bw == 0) { return 0; }
        bigInt quotient = 0;
        bigInt remainder = 0;
        for (int i = bw - 1; i >= 0; i--) {
            remainder <<= 1;
            unsigned long long bit = (limbs[i / 64] >> (i % 64)) & 1;
            remainder.limbs[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                quotient.limbs[i / 64] |= 1ULL << (i % 64);
            }
        }
        return quotient;
    }
    template<typename T> bigInt operator/(const T rhs) const { return *this / bigInt(rhs); }
    void operator/=(const bigInt& rhs) { *this = *this / rhs; }
    template<typename T> void operator/=(const T rhs) { *this = *this / bigInt(rhs); }

    bigInt operator%(const bigInt& rhs) const {
        if (rhs.isZero()) { return *this; }
        int bw = bitWidth();
        if (bw == 0) { return 0; }
        bigInt remainder;
        for (int i = bw - 1; i >= 0; i--) {
            remainder <<= 1;
            unsigned long long bit = (limbs[i / 64] >> (i % 64)) & 1;
            remainder.limbs[0] |= bit;
            if (remainder >= rhs) remainder -= rhs;
        }
        return remainder;
    }
    template<typename T> bigInt operator%(const T rhs) const { return *this % bigInt(rhs); }
    void operator&=(const bigInt& rhs) const { *this = *this % rhs; }
    template<typename T> void operator%=(const T rhs) const { *this = *this % bigInt(rhs); }
    bigInt operator&(const bigInt& rhs) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) result.limbs[i] = limbs[i] & rhs.limbs[i];
        return result;
    }
    template<typename T> bigInt operator&(const T rhs) const { return *this & bigInt(rhs); }
    void operator&=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) limbs[i] &= rhs.limbs[i]; }
    template<typename T> void operator&=(const T rhs) { *this &= bigInt(rhs); }

    bigInt operator|(const bigInt& rhs) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) result.limbs[i] = limbs[i] | rhs.limbs[i];
        return result;
    }
    template<typename T> bigInt operator|(const T rhs) const { return *this | bigInt(rhs); }
    void operator|=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) limbs[i] |= rhs.limbs[i]; }
    template<typename T> void operator|=(const T rhs) { *this |= bigInt(rhs); }

    bigInt operator^(const bigInt& rhs) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) result.limbs[i] = limbs[i] ^ rhs.limbs[i];
        return result;
    }
    template<typename T> bigInt operator^(const T rhs) const { return *this ^ bigInt(rhs); }
    void operator^=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) limbs[i] ^= rhs.limbs[i]; }
    template<typename T> void operator^=(const T rhs) { *this ^= bigInt(rhs); }

    bigInt operator~() const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) result.limbs[i] = ~limbs[i];
        return result;
    }

    bool operator==(const bigInt& rhs) const { for (unsigned int i = 0; i < bytes; i++) { if (limbs[i] != rhs.limbs[i]) return false; } return true; }
    template<typename T> bool operator==(const T rhs) const { return *this == bigInt(rhs); }
    bool operator!=(const bigInt& rhs) const { return !(*this == rhs); }
    template<typename T> bool operator!=(const T rhs) const { return *this != bigInt(rhs); }
    bool operator>(const bigInt& rhs) const {
        for (int i = bytes - 1; i >= 0; i--) {
            if (limbs[i] > rhs.limbs[i]) { return true; }
            if (limbs[i] < rhs.limbs[i]) { return false; }
        }
        return false;
    }
    template<typename T> bool operator>(const T rhs) const { return *this > bigInt(rhs); }
    bool operator>=(const bigInt& rhs) const { return (*this > rhs) || (*this == rhs); }
    template<typename T> bool operator>=(const T rhs) const { return *this >= bigInt(rhs); }
    bool operator<(const bigInt& rhs) const { return rhs > *this; }
    template<typename T> bool operator<(const T rhs) const { return *this < bigInt(rhs); }
    bool operator<=(const bigInt& rhs) const { return !(*this > rhs); }
    template<typename T> bool operator<=(const T rhs) const { return *this <= bigInt(rhs); }

    bigInt& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= bitCount) { setZero(); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (int i = bytes - 1; i >= 0; i--) {
            int ni = i + word_shift;
            if (ni >= bytes) { continue; }
            unsigned long long part = limbs[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 < bytes && bit_shift != 0)
                nw[ni + 1] |= part >> (64 - bit_shift);
        }
        for (unsigned int i = 0; i < bytes; i++) limbs[i] = nw[i];
        return *this;
    }
    bigInt operator<<(int n) const { bigInt result = *this; result <<= n; return result; }

    bigInt& operator>>=(int n) {
        if (n <= 0) { return *this; }
        if (n >= bitCount) { setZero(); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (unsigned int i = 0; i < bytes; i++) {
            int ni = i - word_shift;
            if (ni < 0) { continue; }
            unsigned long long part = limbs[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) { nw[ni - 1] |= part << (64 - bit_shift); }
        }
        for (unsigned int i = 0; i < bytes; i++) limbs[i] = nw[i];
        return *this;
    }
    bigInt operator>>(int n) const { bigInt result = *this; result >>= n; return result; }
    unsigned long long operator[](unsigned long long index) const { return limbs[index]; }
};

template<unsigned long long bitCount>
inline const char* toString(const bigInt<bitCount>& v) {
    static char buf[(bitCount * 2) + 2];
    bigInt<bitCount> temp = v;
    int pos = 0;
    if (v.isZero()) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }
    while (!temp.isZero()) { buf[pos++] = '0' + static_cast<char>(temp.mod(10)); }
    for (int i = 0, j = pos - 1; i < j; i++, --j) {
        char t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
    }
    buf[pos] = '\0';
    return buf;
}