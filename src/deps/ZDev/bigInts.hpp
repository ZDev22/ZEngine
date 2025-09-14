#pragma once

template<unsigned long long bitCount>
    requires (bitCount % 64 == 0)
struct bigInt {
    static constexpr unsigned int bytes = bitCount / 64;
    unsigned long long words[bytes] = {0};

    constexpr bigInt() = default;
    constexpr explicit bigInt(unsigned long long v) { 
        words[0] = v; 
        for (unsigned int i = 1; i < bytes; ++i) words[i] = 0; 
    }
    constexpr bigInt(const unsigned long long* initWords, unsigned int count) {
        unsigned int limit = count < bytes ? count : bytes;
        for (unsigned int i = 0; i < limit; ++i) words[i] = initWords[i];
        for (unsigned int i = limit; i < bytes; ++i) words[i] = 0;
    }

    constexpr bool is_zero() const { 
        for (unsigned int i = 0; i < bytes; ++i) 
            if (words[i] != 0) return false; 
        return true; 
    }
    void zero_words() { for (unsigned int i = 0; i < bytes; ++i) words[i] = 0; }
    unsigned long long div_mod_bigInt64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = bytes - 1; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
    constexpr int bit_width() const {
        for (int i = bytes - 1; i >= 0; --i) {
            if (words[i] != 0) {
                int width = 0;
                unsigned long long val = words[i];
                while (val) { ++width; val >>= 1; }
                return i * 64 + width;
            }
        }
        return 0;
    }

    constexpr bigInt operator+(const bigInt& rhs) const {
        bigInt res;
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; ++i) {
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + carry;
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = sum >> 64;
        }
        return res;
    }
    template<typename T> constexpr bigInt operator+(T rhs) const { return *this + bigInt(rhs); }
    constexpr bigInt& operator+=(const bigInt& rhs) { *this = *this + rhs; return *this; }
    template<typename T> constexpr bigInt& operator+=(T rhs) { *this = *this + rhs; return *this; }

    constexpr bigInt operator-(const bigInt& rhs) const {
        bigInt res;
        __uint128_t borrow = 0;
        for (unsigned int i = 0; i < bytes; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - borrow;
            borrow = (static_cast<__uint128_t>(words[i]) < static_cast<__uint128_t>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }
    template<typename T> constexpr bigInt operator-(T rhs) const { return *this - bigInt(rhs); }
    constexpr bigInt& operator-=(const bigInt& rhs) { *this = *this - rhs; return *this; }
    template<typename T> constexpr bigInt& operator-=(T rhs) { *this = *this - rhs; return *this; }

    constexpr bigInt operator*(const bigInt& rhs) const {
        unsigned long long res_words[bytes] = {0};
        __uint128_t tmp[bytes * 2] = {0};
        for (unsigned int i = 0; i < bytes; ++i)
            for (unsigned int j = 0; j < bytes; ++j)
                if (i + j < bytes * 2)
                    tmp[i + j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = tmp[i] >> 64;
        }
        return bigInt(res_words, bytes);
    }
    template<typename T> constexpr bigInt operator*(T rhs) const { return *this * bigInt(rhs); }
    constexpr bigInt& operator*=(const bigInt& rhs) { *this = *this * rhs; return *this; }
    template<typename T> constexpr bigInt& operator*=(T rhs) { *this = *this * rhs; return *this; }

    constexpr bigInt operator/(const bigInt& divisor) const {
        if (divisor.is_zero()) return bigInt(0);
        bigInt quotient(0), remainder(0);
        int bw = bit_width();
        if (bw == 0) return bigInt(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                quotient.words[i / 64] |= 1ULL << (i % 64);
            }
        }
        return quotient;
    }
    template<typename T> constexpr bigInt operator/(T rhs) const { return *this / bigInt(rhs); }
    constexpr bigInt& operator/=(const bigInt& rhs) { *this = *this / rhs; return *this; }
    template<typename T> constexpr bigInt& operator/=(T rhs) { *this = *this / rhs; return *this; }

    constexpr bigInt operator%(const bigInt& rhs) const {
        if (rhs.is_zero()) return *this;
        bigInt remainder(0);
        int bw = bit_width();
        if (bw == 0) return bigInt(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) remainder -= rhs;
        }
        return remainder;
    }
    template<typename T> constexpr bigInt operator%(T rhs) const { return *this % bigInt(rhs); }

    constexpr bigInt operator&(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.words[i] = words[i] & rhs.words[i];
        return res;
    }
    template<typename T> constexpr bigInt operator&(T rhs) const { return *this & bigInt(rhs); }
    constexpr bigInt& operator&=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) words[i] &= rhs.words[i]; return *this; }
    template<typename T> constexpr bigInt& operator&=(T rhs) { *this &= bigInt(rhs); return *this; }

    constexpr bigInt operator|(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.words[i] = words[i] | rhs.words[i];
        return res;
    }
    template<typename T> constexpr bigInt operator|(T rhs) const { return *this | bigInt(rhs); }
    constexpr bigInt& operator|=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) words[i] |= rhs.words[i]; return *this; }
    template<typename T> constexpr bigInt& operator|=(T rhs) { *this |= bigInt(rhs); return *this; }

    constexpr bigInt operator^(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.words[i] = words[i] ^ rhs.words[i];
        return res;
    }
    template<typename T> constexpr bigInt operator^(T rhs) const { return *this ^ bigInt(rhs); }
    constexpr bigInt& operator^=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) words[i] ^= rhs.words[i]; return *this; }
    template<typename T> constexpr bigInt& operator^=(T rhs) { *this ^= bigInt(rhs); return *this; }

    constexpr bigInt operator~() const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.words[i] = ~words[i];
        return res;
    }

    constexpr bigInt& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= bitCount) { zero_words(); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (int i = bytes - 1; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni >= static_cast<int>(bytes)) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 < static_cast<int>(bytes) && bit_shift != 0) 
                nw[ni + 1] |= part >> (64 - bit_shift);
        }
        for (unsigned int i = 0; i < bytes; ++i) words[i] = nw[i];
        return *this;
    }
    constexpr bigInt operator<<(int n) const { bigInt res = *this; res <<= n; return res; }

    constexpr bigInt& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= bitCount) { zero_words(); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (unsigned int i = 0; i < bytes; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) 
                nw[ni - 1] |= part << (64 - bit_shift);
        }
        for (unsigned int i = 0; i < bytes; ++i) words[i] = nw[i];
        return *this;
    }
    constexpr bigInt operator>>(int n) const { bigInt res = *this; res >>= n; return res; }

    constexpr bool operator==(const bigInt& rhs) const { for (unsigned int i = 0; i < bytes; ++i) { if (words[i] != rhs.words[i]) return false; } return true; }
    template<typename T> constexpr bool operator==(T rhs) const { return *this == bigInt(rhs); }

    constexpr bool operator!=(const bigInt& rhs) const { return !(*this == rhs); }
    template<typename T> constexpr bool operator!=(T rhs) const { return *this != bigInt(rhs); }

    constexpr bool operator>(const bigInt& rhs) const {
        for (int i = bytes - 1; i >= 0; --i) {
            if (words[i] > rhs.words[i]) return true;
            if (words[i] < rhs.words[i]) return false;
        }
        return false;
    }
    template<typename T> constexpr bool operator>(T rhs) const { return *this > bigInt(rhs); }
    constexpr bool operator>=(const bigInt& rhs) const { return (*this > rhs) || (*this == rhs); }
    template<typename T> constexpr bool operator>=(T rhs) const { return *this >= bigInt(rhs); }
    constexpr bool operator<(const bigInt& rhs) const { return rhs > *this; }
    template<typename T> constexpr bool operator<(T rhs) const { return *this < bigInt(rhs); }
    constexpr bool operator<=(const bigInt& rhs) const { return !(*this > rhs); }
    template<typename T> constexpr bool operator<=(T rhs) const { return *this <= bigInt(rhs); }
};

template<unsigned long long bitCount>
requires (bitCount % 64 == 0)
inline const char* toString(const bigInt<bitCount>& v) {
    static char buf[bitCount * 2 + 2];
    if (v.is_zero()) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    bigInt<bitCount> temp = v;
    int pos = 0;
    while (!temp.is_zero()) {
        unsigned long long digit = temp.div_mod_bigInt64(10);
        buf[pos++] = '0' + static_cast<char>(digit);
    }

    for (int i = 0, j = pos - 1; i < j; ++i, --j) {
        char t = buf[i];
        buf[i] = buf[j];
        buf[j] = t;
    }

    buf[pos] = '\0';
    return buf;
}