/* licensed under GPL v3.0 - see https://github.com/ZDev22/Vulkan-Engine/README.md for current license
Current rival (and inspiration) - https://github.com/SamHerts/BigInt/blob/main/bigint.h

v2.13.6

bigInts.hpp is a lightweight cross-platform single-header cpp library for creating INTENSELY LARGE NUMBERS!
Currently still in development with many more features and optimizations to come!
See current preformance benchmarks: https://github.com/ZDev22/Vulkan-Engine/issues/4

Requires the GCC compiler and support for __uint128_t

HOW TO USE: bigInt<128> var;
MATH: var *= 5;
PRINT OUT THE VARIABLE: printf(name.toSting());
*/

#pragma once

#include <string.h>

template<unsigned int bitCount>
struct bigInt {
    static constexpr unsigned int bytes = bitCount / 64;
    unsigned long long limbs[bytes] = {0};

    constexpr bigInt() = default;
    constexpr bigInt(const unsigned long long num) { limbs[0] = num; }
    constexpr bigInt(const unsigned long long* init, unsigned int count) {
        unsigned int i = 0;
        for (; i < count < bytes ? count : bytes; i++) { limbs[i] = init[i]; }
        memset(&limbs[i], 0, (bytes - i) * 8);
    }

    inline void setZero() { memset(&limbs, 0, bytes); }
    inline bool isZero() const {
        static constexpr unsigned long long nothing[bytes] = {0};
        return memcmp(&limbs, &nothing, bytes * 8) == 0;
    }
    unsigned long long mod(unsigned long long num) {
        if (num == 0) { return 0; }
        __uint128_t remainder = 0;
        for (auto limb : limbs) {
            __uint128_t current = (remainder << 64) | limb;
            limb = current / num;
            remainder = current % num;
        }
        return (unsigned long long)remainder;
    }
    unsigned int bitWidth() const {
        for (unsigned int i = 0; i < bytes; i++) {
            if (limbs[i] != 0) { return i * 64 + __builtin_clzll(limbs[i]); }
        }
        return 0;
    }

    void operator=(const bigInt& num) { memcpy(&limbs, &num.limbs, bytes); }
    void operator=(const unsigned long long num) {
        setZero();
        limbs[0] = num;
    }

    bigInt operator+(const bigInt& num) const {
        bigInt result;
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            __uint128_t sum = (__uint128_t)limbs[i] + num.limbs[i] + carry;
            result.limbs[i] = sum;
            carry = sum >> 64;
        }
        return result;
    }
    bigInt operator+(const unsigned long long num) const {
        __uint128_t sum = (__uint128_t)(limbs[0] + num);
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
    void operator+=(const bigInt& num) { *this = *this + num; }
    void operator+=(const unsigned long long num) { *this = *this + num; }
    void operator++() { *this = *this + 1; }

    bigInt operator-(const bigInt& num) const {
        bigInt result;
        __uint128_t borrow = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            result.limbs[i] = limbs[i] - num.limbs[i] + borrow;
            borrow = (limbs[i] < num.limbs[i]) ? 1 : 0;
        }
        return result;
    }
    bigInt operator-(const unsigned long long num) const {
        bigInt result = *this;
        unsigned long long borrow = num;
        for (unsigned int i = 0; i < bytes; i++) {
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
    void operator-=(const bigInt& num) { *this = *this - num; }
    void operator-=(const unsigned long long num) { *this = *this - num; }
    void operator--() { *this = *this - 1; }

    bigInt operator*(const bigInt& num) const {
        bigInt result;
        __uint128_t carry = 0;
        __uint128_t current = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            for (unsigned j = 0; j + i < bytes; j++) {
                current = (__uint128_t)result.limbs[i + j] + (__uint128_t)limbs[i] * num.limbs[j] + carry;
                result.limbs[i + j] = (unsigned long long)current;
                carry = current >> 64;
            }
            carry = 0;
        }
        return result;
    }
    bigInt operator*(const unsigned long long num) const {
        bigInt result;
        __uint128_t carry = 0;
        __uint128_t current = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            current = (__uint128_t)result.limbs[i] + ((__uint128_t)limbs[i] * num) + carry;
            result.limbs[i] = (unsigned long long)current;
            carry = current >> 64;
        }
        return result;
    }
    void operator*=(const bigInt& num) { *this = *this * num; }
    void operator*=(const unsigned long long num) { *this = *this * num; }

    bigInt operator/(const bigInt& num) const {
        if (num.isZero()) { return 0; }
        unsigned int bw = bitWidth();
        if (bw == 0) { return 0; }

        bigInt result = 0;
        bigInt remainder = 0;
        for (unsigned int i = bw - 1; i >= 0; i--) {
            remainder <<= 1;
            unsigned long long bit = (limbs[i / 64] >> (i % 64)) & 1;
            remainder.limbs[0] |= bit;
            if (remainder >= num) {
                remainder -= num;
                result.limbs[i / 64] |= 1ULL << (i % 64);
            }
        }
        return result;
    }
    bigInt operator/(const unsigned long long num) const { return *this / bigInt(num); }
    void operator/=(const bigInt& num) { *this = *this / num; }
    void operator/=(const unsigned long long num) { *this = *this / bigInt(num); }

    bigInt operator%(const bigInt& num) const {
        if (num.isZero()) { return *this; }
        int bitwidth = bitWidth();
        if (bitwidth == 0) { return 0; }
        bigInt remainder;
        for (int i = bitwidth - 1; i >= 0; i--) {
            remainder <<= 1;
            remainder.limbs[0] |= ((limbs[i / 64] >> (i % 64)) & 1);
            if (remainder >= num) { remainder -= num; }
        }
        return remainder;
    }
    bigInt operator%(const unsigned long long num) const { return *this % bigInt(num); }
    void operator&=(const bigInt& num) const { *this = *this % num; }
    void operator%=(const unsigned long long num) const { *this = *this % bigInt(num); }
    bigInt operator&(const bigInt& num) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = limbs[i] & num.limbs[i]; }
        return result;
    }
    bigInt operator&(const unsigned long long num) const { return *this & bigInt(num); }
    void operator&=(const bigInt& num) { for (unsigned int i = 0; i < bytes; i++) limbs[i] &= num.limbs[i]; }
    void operator&=(const unsigned long long num) { *this &= bigInt(num); }

    bigInt operator|(const bigInt& num) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = limbs[i] | num.limbs[i]; }
        return result;
    }
    bigInt operator|(const unsigned long long num) const { return *this | bigInt(num); }
    void operator|=(const bigInt& num) { for (unsigned int i = 0; i < bytes; i++) { limbs[i] |= num.limbs[i]; } }
    void operator|=(const unsigned long long num) { *this |= bigInt(num); }

    bigInt operator^(const bigInt& num) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = limbs[i] ^ num.limbs[i]; }
        return result;
    }
    bigInt operator^(const unsigned long long num) const { return *this ^ bigInt(num); }
    void operator^=(const bigInt& num) { for (unsigned int i = 0; i < bytes; i++) { limbs[i] ^= num.limbs[i]; }}
    void operator^=(const unsigned long long num) { *this ^= bigInt(num); }

    bigInt operator~() const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = ~limbs[i]; }
        return result;
    }

    bool operator==(const bigInt& num) const { for (unsigned int i = 0; i < bytes; i++) { if (limbs[i] != num.limbs[i]) return false; } return true; }
    bool operator==(const unsigned long long num) const { return *this == bigInt(num); }
    bool operator!=(const bigInt& num) const { return !(*this == num); }
    bool operator!=(const unsigned long long num) const { return *this != bigInt(num); }
    bool operator>(const bigInt& num) const {
        for (int i = bytes - 1; i >= 0; i--) {
            if (limbs[i] > num.limbs[i]) { return true; }
            if (limbs[i] < num.limbs[i]) { return false; }
        }
        return false;
    }
    bool operator>(const unsigned long long num) const { return *this > bigInt(num); }
    bool operator>=(const bigInt& num) const { return (*this > num) || (*this == num); }
    bool operator>=(const unsigned long long num) const { return *this >= bigInt(num); }
    bool operator<(const bigInt& num) const { return num > *this; }
    bool operator<(const unsigned long long num) const { return *this < bigInt(num); }
    bool operator<=(const bigInt& num) const { return !(*this > num); }
    bool operator<=(const unsigned long long num) const { return *this <= bigInt(num); }

    bigInt operator<<=(unsigned int num) {
        if (num >= bitCount) { setZero(); return *this; }

        int limbshift = num / 64;
        int bitshift = num % 64;
        unsigned long long nw[bytes] = {0};

        for (int i = bytes - 1; i >= 0; i--) {
            int ni = i + limbshift;
            if (ni >= bytes) { continue; }
            unsigned long long part = limbs[i];

            nw[ni] |= bitshift == 0 ? part : part << bitshift;
            if (ni + 1 < bytes && bitshift != 0) { nw[ni + 1] |= part >> (64 - bitshift); }
        }
        memcpy(&limbs, &nw, bytes);
        return *this;
    }
    bigInt operator<<(int num) const { bigInt result = *this; result <<= num; return result; }

    bigInt operator>>=(int num) {
        if (num <= 0) { return *this; }
        if (num >= bitCount) { setZero(); return *this; }
        int limbshift = num / 64;
        int bitshift = num % 64;
        unsigned long long nw[bytes] = {0};
        for (unsigned int i = 0; i < bytes; i++) {
            int ni = i - limbshift;
            if (ni < 0) { continue; }
            unsigned long long part = limbs[i];
            nw[ni] |= bitshift == 0 ? part : part >> bitshift;
            if (ni - 1 >= 0 && bitshift != 0) { nw[ni - 1] |= part << (64 - bitshift); }
        }
        for (unsigned int i = 0; i < bytes; i++) { limbs[i] = nw[i]; }
        return *this;
    }
    bigInt operator>>(int num) const { bigInt result = *this; result >>= num; return result; }
    unsigned long long operator[](unsigned int index) const { return limbs[index]; }


    const char* toString() const {
        if (isZero()) {
            char buffer[2];
            buffer[0] = '0';
            buffer[1] = '\0';
            return buffer;
        }

        static char buffer[(bitCount * 2) + 2];
        bigInt temp = *this;
        int position = 0;
        char t = '0';

        while (!temp.isZero()) { buffer[position++] = '0' + (char)temp.mod(10); }
        for (unsigned int i = 0, j = position - 1; i < j; i++, --j) {
            t = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = t;
        }

        buffer[position] = '\0';
        return buffer;
    }

    unsigned long long at(unsigned int pos) {
        if (pos < bytes) { return limbs[pos]; }
        return 0;
    }

    void swap(bigInt& num) {
        bigInt temp = *this;
        *this = num;
        num = temp;
    }

    inline bool odd() const { return limbs[0] % 2; }
};