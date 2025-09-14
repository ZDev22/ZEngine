#pragma once

// Use uint64 for precise, large numbers
template<unsigned long long bitCount>
    requires (bitCount % 64 == 0)
struct bigInt {
    static constexpr unsigned int bytes = bitCount / 64;
    unsigned long long limbs[bytes] = {0};

    constexpr bigInt() = default;
    constexpr explicit bigInt(unsigned long long v) { limbs[0] = v; }
    constexpr bigInt(const unsigned long long* initlimbs, unsigned int count) {
        unsigned int limit = count < bytes ? count : bytes;
        for (unsigned int i = 0; i < limit; ++i) limbs[i] = initlimbs[i];
        for (unsigned int i = limit; i < bytes; ++i) limbs[i] = 0;
    }

    constexpr bool is_zero() const { for (unsigned int i = 0; i < bytes; ++i) { if (limbs[i] != 0) return false; } return true; }
    void zero_limbs() { for (unsigned int i = 0; i < bytes; ++i) limbs[i] = 0; }
    unsigned long long div_mod_bigInt64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = bytes - 1; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | limbs[i];
            limbs[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
    constexpr int bit_width() const {
        for (int i = bytes - 1; i >= 0; --i) {
            if (limbs[i] != 0) {
                int width = 0;
                unsigned long long val = limbs[i];
                while (val) { ++width; val >>= 1; }
                return i * 64 + width;
            }
        }
        return 0;
    }

    constexpr void operator=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) { limbs[i] = rhs.limbs[i]; }}
    template<typename T>
    constexpr void operator=(const T rhs) {
        limbs = {0};
        limbs[0] = rhs;
    }

    constexpr bigInt operator+(const bigInt& rhs) const {
        bigInt res;
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; ++i) {
            __uint128_t sum = static_cast<__uint128_t>(limbs[i]) + rhs.limbs[i] + carry;
            res.limbs[i] = static_cast<unsigned long long>(sum);
            carry = sum >> 64;
        }
        return res;
    }
    template<typename T> constexpr bigInt operator+(const T rhs) const { return *this + bigInt(rhs); }
    constexpr bigInt& operator+=(const bigInt& rhs) { *this = *this + rhs; return *this; }
    template<typename T> constexpr bigInt& operator+=(const T rhs) { *this = *this + rhs; return *this; }

    constexpr bigInt operator-(const bigInt& rhs) const {
        bigInt res;
        __uint128_t borrow = 0;
        for (unsigned int i = 0; i < bytes; ++i) {
            unsigned long long tmp = limbs[i] - rhs.limbs[i] - borrow;
            borrow = (static_cast<__uint128_t>(limbs[i]) < static_cast<__uint128_t>(rhs.limbs[i]) + borrow) ? 1 : 0;
            res.limbs[i] = tmp;
        }
        return res;
    }
    template<typename T> constexpr bigInt operator-(const T rhs) const { return *this - bigInt(rhs); }
    constexpr bigInt& operator-=(const bigInt& rhs) { *this = *this - rhs; return *this; }
    template<typename T> constexpr bigInt& operator-=(const T rhs) { *this = *this - rhs; return *this; }

    constexpr bigInt operator*(const bigInt& rhs) const {
        unsigned long long res_limbs[bytes] = {0};
        __uint128_t tmp[bytes * 2] = {0};
        __uint128_t carry = 0;
        for (unsigned int i = 0; i < bytes; ++i) { for (unsigned int j = 0; j < bytes; ++j) { if (i + j < bytes * 2) { tmp[i + j] += static_cast<__uint128_t>(limbs[i]) * rhs.limbs[j]; }}}
        for (unsigned int i = 0; i < bytes; ++i) {
            tmp[i] += carry;
            res_limbs[i] = static_cast<unsigned long long>(tmp[i]);
            carry = tmp[i] >> 64;
        }
        return bigInt(res_limbs, bytes);
    }
    template<typename T> constexpr bigInt operator*(const T rhs) const { return *this * bigInt(rhs); }
    constexpr bigInt& operator*=(const bigInt& rhs) { *this = *this * rhs; return *this; }
    template<typename T> constexpr bigInt& operator*=(const T rhs) { *this = *this * rhs; return *this; }

    constexpr bigInt operator/(const bigInt& divisor) const {
        if (divisor.is_zero()) return bigInt(0);
        bigInt quotient(0), remainder(0);
        int bw = bit_width();
        if (bw == 0) return bigInt(0);
        for (int i = bw - 1; i >= 0; --i) {
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
        if (rhs.is_zero()) return *this;
        bigInt remainder(0);
        int bw = bit_width();
        if (bw == 0) return bigInt(0);
        for (int i = bw - 1; i >= 0; --i) {
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
        for (unsigned int i = 0; i < bytes; ++i) res.limbs[i] = limbs[i] & rhs.limbs[i];
        return res;
    }
    template<typename T> constexpr bigInt operator&(const T rhs) const { return *this & bigInt(rhs); }
    constexpr bigInt& operator&=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) limbs[i] &= rhs.limbs[i]; return *this; }
    template<typename T> constexpr bigInt& operator&=(const T rhs) { *this &= bigInt(rhs); return *this; }

    constexpr bigInt operator|(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.limbs[i] = limbs[i] | rhs.limbs[i];
        return res;
    }
    template<typename T> constexpr bigInt operator|(const T rhs) const { return *this | bigInt(rhs); }
    constexpr bigInt& operator|=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) limbs[i] |= rhs.limbs[i]; return *this; }
    template<typename T> constexpr bigInt& operator|=(const T rhs) { *this |= bigInt(rhs); return *this; }

    constexpr bigInt operator^(const bigInt& rhs) const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.limbs[i] = limbs[i] ^ rhs.limbs[i];
        return res;
    }
    template<typename T> constexpr bigInt operator^(const T rhs) const { return *this ^ bigInt(rhs); }
    constexpr bigInt& operator^=(const bigInt& rhs) { for (unsigned int i = 0; i < bytes; ++i) limbs[i] ^= rhs.limbs[i]; return *this; }
    template<typename T> constexpr bigInt& operator^=(const T rhs) { *this ^= bigInt(rhs); return *this; }

    constexpr bigInt operator~() const {
        bigInt res;
        for (unsigned int i = 0; i < bytes; ++i) res.limbs[i] = ~limbs[i];
        return res;
    }

    constexpr bool operator==(const bigInt& rhs) const { for (unsigned int i = 0; i < bytes; ++i) { if (limbs[i] != rhs.limbs[i]) return false; } return true; }
    template<typename T> constexpr bool operator==(const T rhs) const { return *this == bigInt(rhs); }
    constexpr bool operator!=(const bigInt& rhs) const { return !(*this == rhs); }
    template<typename T> constexpr bool operator!=(const T rhs) const { return *this != bigInt(rhs); }
    constexpr bool operator>(const bigInt& rhs) const {
        for (int i = bytes - 1; i >= 0; --i) {
            if (limbs[i] > rhs.limbs[i]) return true;
            if (limbs[i] < rhs.limbs[i]) return false;
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
        if (n >= bitCount) { zero_limbs(); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (int i = bytes - 1; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni >= static_cast<int>(bytes)) continue;
            unsigned long long part = limbs[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 < static_cast<int>(bytes) && bit_shift != 0) 
                nw[ni + 1] |= part >> (64 - bit_shift);
        }
        for (unsigned int i = 0; i < bytes; ++i) limbs[i] = nw[i];
        return *this;
    }
    constexpr bigInt operator<<(int n) const { bigInt res = *this; res <<= n; return res; }

    constexpr bigInt& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= bitCount) { zero_limbs(); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (unsigned int i = 0; i < bytes; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = limbs[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) 
                nw[ni - 1] |= part << (64 - bit_shift);
        }
        for (unsigned int i = 0; i < bytes; ++i) limbs[i] = nw[i];
        return *this;
    }
    constexpr bigInt operator>>(int n) const { bigInt res = *this; res >>= n; return res; }
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

template <unsigned long long N>
struct bitset {
    static constexpr unsigned long long num_words = (N + 63) / 64;
    unsigned long long data[num_words] = {0};

    constexpr bitset() : data{0} {}
    constexpr bitset(const unsigned char* bytes, unsigned long long byte_count) {
        unsigned long long bytes_to_copy = (N + 7) / 8;
        if (byte_count < bytes_to_copy) bytes_to_copy = byte_count;
        for (unsigned long long i = 0; i < bytes_to_copy; ++i) {
            unsigned long long word_index = i / 8;
            unsigned long long byte_offset = i % 8;
            data[word_index] |= (bytes[i] << (byte_offset * 8));
        }
    }

    constexpr void set(unsigned long long pos) { data[pos / 64] |= (1ULL << (pos % 64)); }
    constexpr void reset(unsigned long long pos) { data[pos / 64] &= ~(1ULL << (pos % 64)); }
    constexpr void flip(unsigned long long pos) { data[pos / 64] ^= (1ULL << (pos % 64)); }
    constexpr bool test(unsigned long long pos) const { return data[pos / 64] & (1ULL << (pos % 64)); }

    constexpr unsigned long long count() const {
        unsigned long long total = 0;
        for (unsigned long long i = 0; i < num_words; ++i) {
            unsigned long long x = data[i];
            while (x) { total += x & 1; x >>= 1; }
        }
        return total;
    }

    constexpr void flip() {
        for (unsigned long long i = 0; i < num_words; ++i) { data[i] = ~data[i]; }
        unsigned long long extra_bits = N % 64;
        if (extra_bits) { data[num_words - 1] &= (1ULL << extra_bits) - 1; }
    }

    constexpr const char* toString() const {
        static char buffer[N + 1];
        for (unsigned long long i = 0; i < N; ++i) { buffer[i] = test(N - 1 - i) ? '1' : '0'; }
        buffer[N] = '\0';
        return buffer;
    }
};











/* Use doubles for less precise, larger numbers
template<unsigned long long limbCount>
    requires (limbCount > 0)
struct bigDouble {
    double limbs[limbCount] = {0.0};

    constexpr bigDouble() = default;
    constexpr explicit bigDouble(double& v) { limbs[0] = v; }

    constexpr void operator=(bigDouble& rhs) { for (unsigned long long i = 0; i < limbs; i++) { limbs[i] = rhs[i]; } }
    template<typename T>
    constexpr void operator=(const T rhs) {
        for (unsigned long long i = 0; i < limbCount; i++) { limbs[i] = 0.0; }
        limbs[0] = static_cast<double>(rhs);
    }
}; */