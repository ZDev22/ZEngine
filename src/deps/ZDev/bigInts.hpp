#pragma once

// Use uint64 for precise, large numbers
template<unsigned long long bitCount>
requires (bitCount % 64 == 0)
struct bigInt {
    static constexpr unsigned int bytes = bitCount / 64;
    unsigned long long limbs[bytes] = {0};

    constexpr bigInt() = default;
    constexpr bigInt(unsigned long long v) { limbs[0] = v; }
    template<typename T>
    constexpr bigInt(const T v) { limbs[0] = v; }
    constexpr bigInt(const unsigned long long* init, unsigned int count) {
        unsigned int limit = count < bytes ? count : bytes;
        for (unsigned int i = 0; i < limit; i++) { limbs[i] = init[i]; }
        for (unsigned int i = limit; i < bytes; i++) { limbs[i] = 0; }
    }

    constexpr bool isZero() const { for (unsigned int i = 0; i < bytes; i++) { if (limbs[i] != 0) return false; } return true; }
    void setZero() { for (unsigned int i = 0; i < bytes; i++) { limbs[i] = 0; }}
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
    constexpr int bitWidth() const {
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

    constexpr void operator=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) { limbs[i] = rhs.limbs[i]; }}
    template<typename T>
    constexpr void operator=(const T rhs) {
        setZero();
        limbs[0] = rhs;
    }

    constexpr bigInt operator+(const bigInt& rhs) const {
        bigInt res;
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; i++) {
            __uint128_t sum = static_cast<__uint128_t>(limbs[i]) + rhs.limbs[i] + carry;
            res.limbs[i] = sum;
            carry = sum >> 64;
        }
        return res;
    }
    template<typename T> constexpr bigInt operator+(const T rhs) const { return *this + bigInt(rhs); }
    constexpr bigInt& operator+=(const bigInt& rhs) { *this = *this + rhs; return *this; }
    template<typename T> constexpr bigInt& operator+=(const T rhs) { *this = *this + bigInt(rhs); return *this; }

    constexpr bigInt operator-(const bigInt& rhs) const {
        bigInt res;
        __uint128_t borrow;
        for (unsigned int i = 0; i < bytes; i++) {
            unsigned long long tmp = limbs[i] - rhs.limbs[i] - borrow;
            borrow = static_cast<__uint128_t>(limbs[i]) < static_cast<__uint128_t>(rhs.limbs[i]) + borrow ? 1 : 0;
            res.limbs[i] = tmp;
        }
        return res;
    }
    template<typename T> constexpr bigInt operator-(const T rhs) const { return *this - bigInt(rhs); }
    constexpr bigInt& operator-=(const bigInt& rhs) { *this = *this - rhs; return *this; }
    template<typename T> constexpr bigInt& operator-=(const T rhs) { *this = *this - bigInt(rhs); return *this; }

    bigInt approxMultiply(const bigInt& rhs, unsigned int topLimbs) const {
        int this_top = -1, rhs_top = -1;
    
        for (unsigned int i = bytes - 1; i >= 0; --i) {
            if (limbs[i] != 0 && this_top == -1) { this_top = i; }
            if (rhs.limbs[i] != 0 && rhs_top == -1) { rhs_top = i; }
            if (this_top != -1 && rhs_top != -1) { break; }
        }
    
        if (this_top == -1 || rhs_top == -1) return 0;
    
        int this_count = topLimbs;
        int rhs_count = topLimbs;
        if (this_count > this_top + 1) { this_count = this_top + 1; }
        if (rhs_count > rhs_top + 1) { rhs_count = rhs_top + 1; }
    
        __uint128_t tmp[2 * bytes] = {0};

        for (int i = 0; i < this_count; ++i) { for (int j = 0; j < rhs_count; ++j) { tmp[i + j] += (__uint128_t)limbs[this_top - i] * (__uint128_t)rhs.limbs[rhs_top - j]; }}
    
        unsigned int shift = (this_top - (this_count - 1)) + (rhs_top - (rhs_count - 1));
    
        bigInt res;
        unsigned __int128 carry = 0;
        for (unsigned int i = 0; i < this_count + rhs_count; ++i) {
            __uint128_t val = tmp[i] + carry;
            unsigned int limb_idx = i + shift;
            if (limb_idx < bytes) { res.limbs[limb_idx] = (unsigned long long)val; }
            carry = val >> 64;
        }
        return res;
    }

    constexpr bigInt operator*(const bigInt& rhs) const {
        unsigned long long res_limbs[bytes] = {0};
        __uint128_t tmp[bytes * 2] = {0};
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; i++) { for (unsigned int j = 0; j < bytes; ++j) { if (i + j < bytes * 2) { tmp[i + j] += static_cast<__uint128_t>(limbs[i]) * rhs.limbs[j]; }}}
        for (unsigned int i = 0; i < bytes; i++) {
            tmp[i] += carry;
            res_limbs[i] = tmp[i];
            carry = tmp[i] >> 64;
        }
        return bigInt(res_limbs, bytes);
    }
    template<typename T> constexpr bigInt operator*(const T rhs) const { return *this * bigInt(rhs); }
    constexpr bigInt& operator*=(const bigInt& rhs) { *this = *this * rhs; return *this; }
    template<typename T> constexpr bigInt& operator*=(const T rhs) { *this = *this * bigInt(rhs); return *this; }

    constexpr bigInt operator/(const bigInt& divisor) const {
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

    template<typename T> constexpr bigInt operator/(const T rhs) const { return *this / bigInt(rhs); }
    constexpr bigInt& operator/=(const bigInt& rhs) { *this = *this / rhs; return *this; }
    template<typename T> constexpr bigInt& operator/=(const T rhs) { *this = *this / rhs; return *this; }

    constexpr bigInt operator%(const bigInt& rhs) const {
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
    template<typename T> constexpr bigInt operator%(const T rhs) const { return *this % bigInt(rhs); }
    constexpr bigInt operator&(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; i++) res.limbs[i] = limbs[i] & rhs.limbs[i];
        return res;
    }
    template<typename T> constexpr bigInt operator&(const T rhs) const { return *this & bigInt(rhs); }
    constexpr bigInt& operator&=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) limbs[i] &= rhs.limbs[i]; return *this; }
    template<typename T> constexpr bigInt& operator&=(const T rhs) { *this &= bigInt(rhs); return *this; }

    constexpr bigInt operator|(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; i++) res.limbs[i] = limbs[i] | rhs.limbs[i];
        return res;
    }
    template<typename T> constexpr bigInt operator|(const T rhs) const { return *this | bigInt(rhs); }
    constexpr bigInt& operator|=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) limbs[i] |= rhs.limbs[i]; return *this; }
    template<typename T> constexpr bigInt& operator|=(const T rhs) { *this |= bigInt(rhs); return *this; }

    constexpr bigInt operator^(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; i++) res.limbs[i] = limbs[i] ^ rhs.limbs[i];
        return res;
    }
    template<typename T> constexpr bigInt operator^(const T rhs) const { return *this ^ bigInt(rhs); }
    constexpr bigInt& operator^=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; i++) limbs[i] ^= rhs.limbs[i]; return *this; }
    template<typename T> constexpr bigInt& operator^=(const T rhs) { *this ^= bigInt(rhs); return *this; }

    constexpr bigInt operator~() const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; i++) res.limbs[i] = ~limbs[i];
        return res;
    }

    constexpr bool operator==(const bigInt& rhs) const { for (unsigned int i = 0; i < bytes; i++) { if (limbs[i] != rhs.limbs[i]) return false; } return true; }
    template<typename T> constexpr bool operator==(const T rhs) const { return *this == bigInt(rhs); }
    constexpr bool operator!=(const bigInt& rhs) const { return !(*this == rhs); }
    template<typename T> constexpr bool operator!=(const T rhs) const { return *this != bigInt(rhs); }
    constexpr bool operator>(const bigInt& rhs) const {
        for (int i = bytes - 1; i >= 0; i--) {
            if (limbs[i] > rhs.limbs[i]) { return true; }
            if (limbs[i] < rhs.limbs[i]) { return false; }
        }
        return false;
    }
    template<typename T> constexpr bool operator>(const T rhs) const { return *this > bigInt(rhs); }
    constexpr bool operator>=(const bigInt& rhs) const { return (*this > rhs) || (*this == rhs); }
    template<typename T> constexpr bool operator>=(const T rhs) const { return *this >= bigInt(rhs); }
    constexpr bool operator<(const bigInt& rhs) const { return rhs > *this; }
    template<typename T> constexpr bool operator<(const T rhs) const { return *this < bigInt(rhs); }
    constexpr bool operator<=(const bigInt& rhs) const { return !(*this > rhs); }
    template<typename T> constexpr bool operator<=(const T rhs) const { return *this <= bigInt(rhs); }

    constexpr bigInt& operator<<=(int n) {
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
    constexpr bigInt operator<<(int n) const { bigInt res = *this; res <<= n; return res; }

    constexpr bigInt& operator>>=(int n) {
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
    constexpr bigInt operator>>(int n) const { bigInt res = *this; res >>= n; return res; }
    constexpr unsigned long long operator[](unsigned long long index) const { return limbs[index]; }
};

template<unsigned long long bitCount>
requires (bitCount % 64 == 0)
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

// Use doubles for less precise, larger numbers
template<unsigned int limbCount>
struct bigDouble {
    double limbs[limbCount] = {0.0};

    constexpr bigDouble() {}
    constexpr bigDouble(double v) { setZero(); limbs[0] = v; }

    constexpr void setZero() { for (unsigned int i = 0; i < limbCount; i++) limbs[i] = 0.0; }
    constexpr bool isZero() const { for (unsigned int i = 0; i < limbCount; i++) if (limbs[i] != 0.0) return false; return true; }

    constexpr __float128 mod(__float128 m) const {
        if (m == 0.0Q) { return 0.0Q; }
        __float128 r = 0.0Q;
        const __float128 BASE = 9007199254740992.0Q;
        for (unsigned int i = limbCount; i-- > 0; ) {
            __float128 temp = r * BASE + static_cast<__float128>(limbs[i]);
            __float128 q = temp / m;
            __int128 qi = static_cast<__int128>(q);
            r = temp - m * static_cast<__float128>(qi);
        }
        return r;
    }

    constexpr void operator=(const bigDouble& rhs) { for (unsigned int i = 0; i < limbCount; i++) limbs[i] = rhs.limbs[i]; }
    template<typename T>
    constexpr void operator=(const T rhs) { setZero(); limbs[0] = static_cast<double>(rhs); }

    constexpr bigDouble operator+(const bigDouble& rhs) const {
        bigDouble res;
        __int128 carry = 0;
        for (unsigned int i = 0; i < limbCount; i++) {
            __float128 t = static_cast<__float128>(limbs[i]) + static_cast<__float128>(rhs.limbs[i]) + static_cast<__float128>(carry);
            __int128 c = static_cast<__int128>(t / 9007199254740992.0Q);
            __float128 rem = t - static_cast<__float128>(c) * 9007199254740992.0Q;
            res.limbs[i] = static_cast<double>(rem);
            carry = c;
        }
        return res;
    }

    constexpr __float128 operator/(__float128 m) {
            if (m == 0.0Q) { return 0.0Q; }
            __float128 carry = 0.0Q;
            const __float128 BASE = 9007199254740992.0Q;
            for (int i = limbCount - 1; i >= 0; --i) {
                __float128 cur = carry * BASE + static_cast<__float128>(limbs[i]);
                __int128 qint = static_cast<__int128>(cur / m);
                __float128 q = static_cast<__float128>(qint);
                __float128 r = cur - q * m;
                limbs[i] = static_cast<double>(q);
                carry = r;
            }
            return carry;
        }

    constexpr double operator[](unsigned int index) const { return limbs[index]; }
};

    template<unsigned int limbCount>
    inline const char* doubleToString(const bigDouble<limbCount>& v) {
    static char buf[limbCount * 20 + 64];
    int pos = 0;

    if (v.isZero()) {
        buf[pos++] = '0';
        buf[pos] = '\0';
        return buf;
    }

    bigDouble<limbCount> temp;
    temp = v;
    char int_digits[limbCount * 20 + 1];
    int digit_count = 0;
    const int max_digits = static_cast<int>(limbCount * 20);

    while (!temp.isZero() && digit_count < max_digits) {
        __float128 rem = temp / 10.0Q;
        int d = static_cast<int>(rem);
        if (d < 0) d = 0;
        if (d > 9) d = 9;
        int_digits[digit_count++] = '0' + d;
    }

    if (digit_count == 0) {
        buf[pos++] = '0';
    }
    else {
        for (int k = digit_count - 1; k >= 0; --k) { buf[pos++] = int_digits[k]; }
    }

    __float128 frac_part = v.mod(1.0Q);
    if (frac_part > 0.0Q) {
        buf[pos++] = '.';
        for (int k = 0; k < 50 && pos < (int)(limbCount * 20 + 63); ++k) {
            frac_part *= 10.0Q;
            int d = static_cast<int>(frac_part);
            if (d < 0) d = 0;
            if (d > 9) d = 9;
            buf[pos++] = '0' + d;
            frac_part -= static_cast<__float128>(d);
            if (frac_part == 0.0Q) break;
        }
    }

    buf[pos] = '\0';
    return buf;
}

// Custom bitset for storing large quantities of 1's and 0's effeciently.
template <unsigned long long N>
struct bitset {
    static constexpr unsigned long long limbs = (N + 63) / 64;
    unsigned long long data[limbs] = {0};

    constexpr bitset() {}

    constexpr void set(unsigned long long pos) { data[pos / 64] |= (1ULL << (pos % 64)); }
    constexpr void reset(unsigned long long pos) { data[pos / 64] &= ~(1ULL << (pos % 64)); }
    constexpr void flip(unsigned long long pos) { data[pos / 64] ^= (1ULL << (pos % 64)); }
    constexpr bool check(unsigned long long pos) const { return data[pos / 64] & (1ULL << (pos % 64)); }

    constexpr const char* toString() const {
        static char buffer[N + 1];
        for (unsigned long long i = 0; i < N; i++) { buffer[i] = check(N - 1 - i) ? '1' : '0'; }
        buffer[N] = '\0';
        return buffer;
    }
};