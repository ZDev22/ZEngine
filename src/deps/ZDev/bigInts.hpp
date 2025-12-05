// Current rival (and inspiration) https://github.com/SamHerts/BigInt/blob/main/bigint.h

#pragma once

#include <string.h>

template<unsigned long long bitCount>
struct bigInt {
    static constexpr unsigned int bytes = bitCount / 64;
    unsigned long long limbs[bytes] = {0};

    constexpr bigInt() = default;
    template<typename T> constexpr bigInt(const T num) { limbs[0] = num; }
    constexpr bigInt(const unsigned long long* init, unsigned int count) {
        unsigned int limit = count < bytes ? count : bytes;
        unsigned int i = 0;
        for (; i < limit; i++) { limbs[i] = init[i]; }
        memset(&limbs[i], 0, (bytes - i) * 8);
    }

    inline void setZero() { memset(limbs, 0, bytes); }
    inline bool isZero() const {
        static constexpr unsigned long long nothing[bytes] = {0};
        return memcmp(limbs, nothing, bytes * 8) == 0;
    }
    unsigned long long mod(unsigned long long num) {
        if (num == 0) { return 0; }
        __uint128_t remainder = 0;
        for (int i = bytes - 1; i >= 0; i--) {
            __uint128_t current = (remainder << 64) | limbs[i];
            limbs[i] = current / num;
            remainder = current % num;
        }
        return (unsigned long long)remainder;
    }
    int bitWidth() const {
        for (int i = bytes - 1; i >= 0; --i) {
            if (limbs[i] != 0) { return i * 64 + std::bit_width(limbs[i]); }
        }
        return 0;
    }

    void operator=(const bigInt& num) { memcpy(limbs, num.limbs, bytes); }
    template<typename T> void operator=(const T num) {
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
    template<typename T> bigInt operator+(const T num) const { 
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
    template<typename T> void operator+=(const T num) { *this = *this + num; }

    bigInt operator-(const bigInt& num) const {
        bigInt result;
        __uint128_t borrow = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            result.limbs[i] = limbs[i] - num.limbs[i] + borrow;
            borrow = (limbs[i] < num.limbs[i]) ? 1 : 0;
        }
        return result;
    }
    template<typename T> bigInt operator-(const T num) const {
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
    template<typename T> void operator-=(const T num) { *this = *this - num; }

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
    template<typename T> bigInt operator*(const T num) const {
        bigInt result;
        __uint128_t carry = 0;
        __uint128_t current = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            current = (__uint128_t)result.limbs[i] + (__uint128_t)limbs[i] * num + carry;
            result.limbs[i] = (unsigned long long)current;
            carry = current >> 64;
        }
        return result;
    }
    void operator*=(const bigInt& num) { *this = *this * num; }
    template<typename T> void operator*=(const T num) { *this = *this * num; }

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
    template<typename T> bigInt operator/(const T num) const { return *this / bigInt(num); }
    void operator/=(const bigInt& num) { *this = *this / num; }
    template<typename T> void operator/=(const T num) { *this = *this / bigInt(num); }

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
    template<typename T> bigInt operator%(const T num) const { return *this % bigInt(num); }
    void operator&=(const bigInt& num) const { *this = *this % num; }
    template<typename T> void operator%=(const T num) const { *this = *this % bigInt(num); }
    bigInt operator&(const bigInt& num) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = limbs[i] & num.limbs[i]; }
        return result;
    }
    template<typename T> bigInt operator&(const T num) const { return *this & bigInt(num); }
    void operator&=(const bigInt& num) { for (unsigned int i = 0; i < bytes; i++) limbs[i] &= num.limbs[i]; }
    template<typename T> void operator&=(const T num) { *this &= bigInt(num); }

    bigInt operator|(const bigInt& num) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = limbs[i] | num.limbs[i]; }
        return result;
    }
    template<typename T> bigInt operator|(const T num) const { return *this | bigInt(num); }
    void operator|=(const bigInt& num) { for (unsigned int i = 0; i < bytes; i++) limbs[i] |= num.limbs[i]; }
    template<typename T> void operator|=(const T num) { *this |= bigInt(num); }

    bigInt operator^(const bigInt& num) const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = limbs[i] ^ num.limbs[i]; }
        return result;
    }
    template<typename T> bigInt operator^(const T num) const { return *this ^ bigInt(num); }
    void operator^=(const bigInt& num) { for (unsigned int i = 0; i < bytes; i++) limbs[i] ^= num.limbs[i]; }
    template<typename T> void operator^=(const T num) { *this ^= bigInt(num); }

    bigInt operator~() const {
        bigInt result;
        for (unsigned int i = 0; i < bytes; i++) { result.limbs[i] = ~limbs[i]; }
        return result;
    }

    bool operator==(const bigInt& num) const { for (unsigned int i = 0; i < bytes; i++) { if (limbs[i] != num.limbs[i]) return false; } return true; }
    template<typename T> bool operator==(const T num) const { return *this == bigInt(num); }
    bool operator!=(const bigInt& num) const { return !(*this == num); }
    template<typename T> bool operator!=(const T num) const { return *this != bigInt(num); }
    bool operator>(const bigInt& num) const {
        for (int i = bytes - 1; i >= 0; i--) {
            if (limbs[i] > num.limbs[i]) { return true; }
            if (limbs[i] < num.limbs[i]) { return false; }
        }
        return false;
    }
    template<typename T> bool operator>(const T num) const { return *this > bigInt(num); }
    bool operator>=(const bigInt& num) const { return (*this > num) || (*this == num); }
    template<typename T> bool operator>=(const T num) const { return *this >= bigInt(num); }
    bool operator<(const bigInt& num) const { return num > *this; }
    template<typename T> bool operator<(const T num) const { return *this < bigInt(num); }
    bool operator<=(const bigInt& num) const { return !(*this > num); }
    template<typename T> bool operator<=(const T num) const { return *this <= bigInt(num); }

    bigInt operator<<=(unsigned int n) {
        if (n >= bitCount) { setZero(); return *this; }
        
        int limbshift = n / 64;
        int bitshift = n % 64;
        unsigned long long nw[bytes] = {0};

        for (int i = bytes - 1; i >= 0; i--) {
            int ni = i + limbshift;
            if (ni >= bytes) { continue; }
            unsigned long long part = limbs[i];

            nw[ni] |= bitshift == 0 ? part : part << bitshift;
            if (ni + 1 < bytes && bitshift != 0) { nw[ni + 1] |= part >> (64 - bitshift); }
        }
        memcpy(limbs, nw, bytes);
        return *this;
    }
    bigInt operator<<(int n) const { bigInt result = *this; result <<= n; return result; }

    bigInt operator>>=(int n) {
        if (n <= 0) { return *this; }
        if (n >= bitCount) { setZero(); return *this; }
        int limbshift = n / 64;
        int bitshift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (unsigned int i = 0; i < bytes; i++) {
            int ni = i - limbshift;
            if (ni < 0) { continue; }
            unsigned long long part = limbs[i];
            nw[ni] |= bitshift == 0 ? part : part >> bitshift;
            if (ni - 1 >= 0 && bitshift != 0) { nw[ni - 1] |= part << (64 - bitshift); }
        }
        for (unsigned int i = 0; i < bytes; i++) limbs[i] = nw[i];
        return *this;
    }
    bigInt operator>>(int n) const { bigInt result = *this; result >>= n; return result; }
    unsigned long long operator[](unsigned long long index) const { return limbs[index]; }


    const char* toString() {
        static char buffer[(bitCount * 2) + 2];
        bigInt<bitCount> temp = *this;
        int pos = 0;
        if (isZero()) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return buffer;
        }
        while (!temp.isZero()) { buffer[pos++] = '0' + (char)temp.mod(10); }
        for (int i = 0, j = pos - 1; i < j; i++, --j) {
            char t = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = t;
        }
        buffer[pos] = '\0';
        return buffer;
    }

    void swap(bigInt& num) {
        bigInt temp = *this;
        *this = num;
        num = temp;
    }

    inline bool even() { return !(limbs[0] % 1); }
    inline bool odd() { return limbs[0] % 2; }
};