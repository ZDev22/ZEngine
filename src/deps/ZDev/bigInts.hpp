#pragma once

#include <string>
#include <ostream>
#include <cstring>
#include <algorithm>

constexpr int bit_width64(unsigned long long x) {
    if (x == 0) return 0;
    return 64 - __builtin_clzll(x);
}

constexpr unsigned long long mul_hi(unsigned long long a, unsigned long long b) {
#if defined(__GNUC__) && defined(__x86_64__)
    return static_cast<unsigned long long>((static_cast<__uint128_t>(a) * b) >> 64);
#else
    return (((((a & 0xFFFFFFFFu) * (b & 0xFFFFFFFFu)) >> 32) + (((a >> 32) * (b & 0xFFFFFFFFu)) & 0xFFFFFFFFu) + (((a & 0xFFFFFFFFu) * (b >> 32)) & 0xFFFFFFFFu)) >> 32) + (((a >> 32) * (b & 0xFFFFFFFFu)) >> 32) + (((a & 0xFFFFFFFFu) * (b >> 32)) >> 32) + ((a >> 32) * (b >> 32));
#endif
}

struct uint128_t {
    unsigned long long low = 0;
    unsigned long long high = 0;

    constexpr uint128_t() = default;
    constexpr explicit uint128_t(unsigned long long v) : low(v), high(0) {}
    constexpr uint128_t(unsigned long long lo, unsigned long long hi) : low(lo), high(hi) {}

    constexpr uint128_t operator+(const uint128_t& rhs) const { return uint128_t(low + rhs.low, high + rhs.high + ((low + rhs.low < low) ? 1 : 0)); }
    constexpr uint128_t& operator+=(const uint128_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint128_t operator-(const uint128_t& rhs) const { return uint128_t(low - rhs.low, high - rhs.high - ((low < rhs.low) ? 1 : 0)); }
    constexpr uint128_t& operator-=(const uint128_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint128_t operator*(const uint128_t& rhs) const { return uint128_t(low * rhs.low, mul_hi(low, rhs.low) + (low * rhs.high) + (rhs.low * high)); }
    constexpr uint128_t& operator*=(const uint128_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint128_t operator/(uint128_t divisor) const {
        if (divisor == uint128_t(0)) { return uint128_t(0); }
        uint128_t quotient(0);
        uint128_t remainder(0);

        for (int i = 127; i >= 0; --i) {
            remainder <<= 1;
            remainder.low |= ((*this >> i).low & 1);

            if (remainder >= divisor) {
                remainder -= divisor;
                quotient |= uint128_t(1) << i;
            }
        }

        return quotient;
    }

    constexpr uint128_t& operator/=(const uint128_t& rhs) { *this = *this / rhs; return *this; }

    constexpr uint128_t operator^(const uint128_t& rhs) const { return uint128_t(low ^ rhs.low, high ^ rhs.high); }
    constexpr uint128_t& operator^=(const uint128_t& rhs) { *this = *this ^ rhs; return *this; }
    constexpr uint128_t operator%(const uint128_t& rhs) const { if (rhs == uint128_t(0)) { return *this; } return *this - (*this / rhs) * rhs; }

    constexpr uint128_t operator<<(int n) const {
        uint128_t tmp = *this;
        tmp <<= n;
        return tmp;
    }

    constexpr uint128_t& operator<<=(int n) {
        if (n <= 0 || n >= 128) {
            low = 0;
            high = 0;
        }
        else if (n >= 64) {
            high = low << (n - 64);
            low = 0;
        }
        else {
            high = (high << n) | (low >> (64 - n));
            low <<= n;
        }
        return *this;
    }

    constexpr uint128_t operator>>(int n) const {
        uint128_t tmp = *this;
        tmp >>= n;
        return tmp;
    }
    constexpr uint128_t& operator>>=(int n) {
        if (n <= 0 || n >= 128) {
            low = 0;
            high = 0;
        }
        else if (n >= 64) {
            low = high >> (n - 64);
            high = 0;
        }
        else {
            low = (low >> n) | (high << (64 - n));
            high >>= n;
        }
        return *this;
    }

    constexpr uint128_t operator|(const uint128_t& rhs) const { return uint128_t(low | rhs.low, high | rhs.high); }
    constexpr uint128_t& operator|=(const uint128_t& rhs) {
        low |= rhs.low;
        high |= rhs.high;
        return *this;
    }

    constexpr bool operator>(const uint128_t& rhs) const { return (high > rhs.high) || (high == rhs.high && low > rhs.low); }
    constexpr bool operator>=(const uint128_t& rhs) const { return (high > rhs.high) || (high == rhs.high && low >= rhs.low); }
    constexpr bool operator==(const uint128_t& rhs) const { return low == rhs.low && high == rhs.high; }


    friend std::ostream& operator<<(std::ostream& os, const uint128_t& v) {
        if (v.high == 0) { return os << v.low; }

        char buf[40];
        int i = 39;

        uint128_t temp = v;
        const unsigned long long BASE = 10000000000000000000ULL;
        unsigned long long parts[3];

        parts[0] = temp.low % BASE;
        temp /= uint128_t(BASE);
        parts[1] = temp.low % BASE;
        temp /= uint128_t(BASE);
        parts[2] = temp.low;

        bool started = false;
        for (int j = 2; j >= 0; --j) {
            unsigned long long p = parts[j];

            if (!started) { if (p == 0){ continue; } started = true; }

            char chunk[20];
            int k = 0;

            if (p == 0) { chunk[k++] = '0'; }
            else { while (p > 0) { chunk[k++] = '0' + static_cast<char>(p % 10); p /= 10; }}
            if (j < 2) { while (k < 19) { chunk[k++] = '0'; } }

            for (int x = k - 1; x >= 0; --x) { buf[--i] = chunk[x]; }
        }
        return os.write(buf + i, 39 - i);
    }
};

inline std::string toString128(const uint128_t& v) {
    if (v == uint128_t(0)) return "0";
#if defined(__SIZEOF_INT128__)
    __uint128_t x = ((static_cast<__uint128_t>(v.high) << 64) | static_cast<__uint128_t>(v.low));
    std::string s;
    while (x != 0) {
        s.push_back('0' + static_cast<unsigned>(x % 10));
        x /= 10;
    }
#else
    uint128_t temp = v;
    std::string s;
    while (temp > uint128_t(0)) {
        unsigned long long rem = static_cast<unsigned long long>((temp % uint128_t(10)).low);
        s.push_back('0' + static_cast<unsigned>(rem));
        temp /= uint128_t(10);
    }
#endif
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
    return s;
}

struct uint256_t {
    unsigned long long words[4] = {0,0,0,0};

    constexpr uint256_t() = default;
    constexpr explicit uint256_t(unsigned long long v) : words{v,0,0,0} {}
    constexpr uint256_t(unsigned long long w0, unsigned long long w1, unsigned long long w2, unsigned long long w3) : words{w0, w1, w2, w3} {}

    constexpr bool is_zero() const { return words[0]==0 && words[1]==0 && words[2]==0 && words[3]==0; }
    constexpr int bit_width() const {
        for (int i = 3; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint256_t operator+(const uint256_t& rhs) const {
        uint256_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 4; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint256_t& operator+=(const uint256_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint256_t operator-(const uint256_t& rhs) const {
        uint256_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 4; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint256_t& operator-=(const uint256_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint256_t operator*(const uint256_t& rhs) const {
        unsigned long long res_words[4] = {0,0,0,0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[8] = {0,0,0,0,0,0,0,0};
        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 4; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[8] = {0,0,0,0,0,0,0,0};
        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 4; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        return {res_words[0], res_words[1], res_words[2], res_words[3]};
    }

    constexpr uint256_t& operator*=(const uint256_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint256_t operator/(const uint256_t& divisor) const {
        if (divisor.is_zero()) return uint256_t(0);
        uint256_t quotient(0);
        uint256_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint256_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint256_t& operator/=(const uint256_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint256_t operator%(const uint256_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint256_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint256_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint256_t& rhs) const {
        return std::equal(words, words + 4, rhs.words);
    }
    constexpr bool operator!=(const uint256_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint256_t& rhs) const {
        for (int i = 3; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint256_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint256_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint256_t& rhs) const { return !(*this > rhs); }
    constexpr uint256_t operator<<(int n) const { uint256_t res = *this; res <<= n; return res; }
    constexpr uint256_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 256) {
            std::fill(words, words + 4, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[4] = {0,0,0,0};
        for (int i = 3; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 3) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 3 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint256_t operator>>(int n) const { uint256_t res = *this; res >>= n; return res; }
    constexpr uint256_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 256) {
            std::fill(words, words + 4, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[4] = {0};
        for (int i = 0; i < 4; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint256_t operator|(const uint256_t& rhs) const {
        return {words[0] | rhs.words[0], words[1] | rhs.words[1], words[2] | rhs.words[2], words[3] | rhs.words[3]};
    }

    constexpr uint256_t& operator|=(const uint256_t& rhs) { words[0] |= rhs.words[0]; words[1] |= rhs.words[1]; words[2] |= rhs.words[2]; words[3] |= rhs.words[3]; return *this; }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 3; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint256_t& v) {
        if (v.is_zero()) return os << "0";
#if defined(__SIZEOF_INT128__)
        uint256_t temp = v;
        std::string s;
        while (!temp.is_zero()) {
            s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
        }
        for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
        return os << s;
#else
        char buf[4 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
#endif
    }
};

inline std::string toString256(const uint256_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint256_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[4 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint512_t {
    unsigned long long words[8] = {0,0,0,0,0,0,0,0};

    constexpr uint512_t() = default;
    constexpr explicit uint512_t(unsigned long long v) : words{v,0,0,0,0,0,0,0} {}
    constexpr uint512_t(unsigned long long w0, unsigned long long w1, unsigned long long w2, unsigned long long w3, unsigned long long w4, unsigned long long w5, unsigned long long w6, unsigned long long w7) : words{w0,w1,w2,w3,w4,w5,w6,w7} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 8; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 7; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint512_t operator+(const uint512_t& rhs) const {
        uint512_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 8; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint512_t& operator+=(const uint512_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint512_t operator-(const uint512_t& rhs) const {
        uint512_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 8; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint512_t& operator-=(const uint512_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint512_t operator*(const uint512_t& rhs) const {
        unsigned long long res_words[8] = {0,0,0,0,0,0,0,0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[16] = {0};
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 8; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[16] = {0};
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 8; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        return {res_words[0], res_words[1], res_words[2], res_words[3], res_words[4], res_words[5], res_words[6], res_words[7]};
    }

    constexpr uint512_t& operator*=(const uint512_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint512_t operator/(const uint512_t& divisor) const {
        if (divisor.is_zero()) return uint512_t(0);
        uint512_t quotient(0);
        uint512_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint512_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint512_t& operator/=(const uint512_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint512_t operator%(const uint512_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint512_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint512_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint512_t& rhs) const {
        return std::equal(words, words + 8, rhs.words);
    }
    constexpr bool operator!=(const uint512_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint512_t& rhs) const {
        for (int i = 7; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint512_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint512_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint512_t& rhs) const { return !(*this > rhs); }
    constexpr uint512_t operator<<(int n) const { uint512_t res = *this; res <<= n; return res; }
    constexpr uint512_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 512) {
            std::fill(words, words + 8, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[8] = {0,0,0,0,0,0,0,0};
        for (int i = 7; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 7) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 7 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint512_t operator>>(int n) const { uint512_t res = *this; res >>= n; return res; }
    constexpr uint512_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 512) {
            std::fill(words, words + 8, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[8] = {0,0,0,0,0,0,0,0};
        for (int i = 0; i < 8; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint512_t operator|(const uint512_t& rhs) const {
        uint512_t r;
        for (size_t i=0;i<8;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint512_t& operator|=(const uint512_t& rhs) {
        for (size_t i=0;i<8;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 7; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint512_t& v) {
        if (v.is_zero()) return os << "0";
        char buf[8 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                      static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
    }
};

inline std::string toString512(const uint512_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint512_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[8 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                  static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint1024_t {
    unsigned long long words[16] = {0};

    constexpr uint1024_t() = default;
    constexpr explicit uint1024_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 16; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 15; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint1024_t operator+(const uint1024_t& rhs) const {
        uint1024_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 16; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint1024_t& operator+=(const uint1024_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint1024_t operator-(const uint1024_t& rhs) const {
        uint1024_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 16; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint1024_t& operator-=(const uint1024_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint1024_t operator*(const uint1024_t& rhs) const {
        unsigned long long res_words[16] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[32] = {0};
        for (int i = 0; i < 16; ++i) for (int j = 0; j < 16; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 16; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[32] = {0};
        for (int i = 0; i < 16; ++i) for (int j = 0; j < 16; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 16; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint1024_t res;
        for (int i = 0; i < 16; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint1024_t& operator*=(const uint1024_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint1024_t operator/(const uint1024_t& divisor) const {
        if (divisor.is_zero()) return uint1024_t(0);
        uint1024_t quotient(0);
        uint1024_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint1024_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint1024_t& operator/=(const uint1024_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint1024_t operator%(const uint1024_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint1024_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint1024_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint1024_t& rhs) const {
        return std::equal(words, words + 16, rhs.words);
    }
    constexpr bool operator!=(const uint1024_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint1024_t& rhs) const {
        for (int i = 15; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint1024_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint1024_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint1024_t& rhs) const { return !(*this > rhs); }
    constexpr uint1024_t operator<<(int n) const { uint1024_t res = *this; res <<= n; return res; }
    constexpr uint1024_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 1024) {
            std::fill(words, words + 16, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[16] = {0};
        for (int i = 15; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 15) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 15 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint1024_t operator>>(int n) const { uint1024_t res = *this; res >>= n; return res; }
    constexpr uint1024_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 1024) {
            std::fill(words, words + 16, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[16] = {0};
        for (int i = 0; i < 16; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint1024_t operator|(const uint1024_t& rhs) const {
        uint1024_t r;
        for (size_t i=0;i<16;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint1024_t& operator|=(const uint1024_t& rhs) {
        for (size_t i=0;i<16;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 15; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint1024_t& v) {
        if (v.is_zero()) return os << "0";
        char buf[16 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                      static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                      static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                      static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                      static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                      static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
    }
};

inline std::string toString1024(const uint1024_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint1024_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[16 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                  static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                  static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                  static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                  static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                  static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint2048_t {
    unsigned long long words[32] = {0};

    constexpr uint2048_t() = default;
    constexpr explicit uint2048_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 32; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 31; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint2048_t operator+(const uint2048_t& rhs) const {
        uint2048_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 32; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint2048_t& operator+=(const uint2048_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint2048_t operator-(const uint2048_t& rhs) const {
        uint2048_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 32; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint2048_t& operator-=(const uint2048_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint2048_t operator*(const uint2048_t& rhs) const {
        unsigned long long res_words[32] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[64] = {0};
        for (int i = 0; i < 32; ++i) for (int j = 0; j < 32; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 32; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[64] = {0};
        for (int i = 0; i < 32; ++i) for (int j = 0; j < 32; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 32; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint2048_t res;
        for (int i = 0; i < 32; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint2048_t& operator*=(const uint2048_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint2048_t operator/(const uint2048_t& divisor) const {
        if (divisor.is_zero()) return uint2048_t(0);
        uint2048_t quotient(0);
        uint2048_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint2048_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint2048_t& operator/=(const uint2048_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint2048_t operator%(const uint2048_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint2048_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint2048_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint2048_t& rhs) const {
        return std::equal(words, words + 32, rhs.words);
    }
    constexpr bool operator!=(const uint2048_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint2048_t& rhs) const {
        for (int i = 31; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint2048_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint2048_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint2048_t& rhs) const { return !(*this > rhs); }
    constexpr uint2048_t operator<<(int n) const { uint2048_t res = *this; res <<= n; return res; }
    constexpr uint2048_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 2048) {
            std::fill(words, words + 32, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[32] = {0};
        for (int i = 31; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 31) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 31 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint2048_t operator>>(int n) const { uint2048_t res = *this; res >>= n; return res; }
    constexpr uint2048_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 2048) {
            std::fill(words, words + 32, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[32] = {0};
        for (int i = 0; i < 32; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint2048_t operator|(const uint2048_t& rhs) const {
        uint2048_t r;
        for (size_t i=0;i<32;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint2048_t& operator|=(const uint2048_t& rhs) {
        for (size_t i=0;i<32;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 31; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint2048_t& v) {
        if (v.is_zero()) return os << "0";
        char buf[32 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                      static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                      static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                      static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                      static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                      static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                      static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                      static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                      static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                      static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                      static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                      static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                      static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                      static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
    }
};

inline std::string toString2048(const uint2048_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint2048_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[32 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                  static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                  static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                  static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                  static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                  static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                  static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                  static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                  static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                  static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                  static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                  static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                  static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                  static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint4096_t {
    unsigned long long words[64] = {0};

    constexpr uint4096_t() = default;
    constexpr explicit uint4096_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 64; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 63; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint4096_t operator+(const uint4096_t& rhs) const {
        uint4096_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 64; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint4096_t& operator+=(const uint4096_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint4096_t operator-(const uint4096_t& rhs) const {
        uint4096_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 64; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint4096_t& operator-=(const uint4096_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint4096_t operator*(const uint4096_t& rhs) const {
        unsigned long long res_words[64] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[128] = {0};
        for (int i = 0; i < 64; ++i) for (int j = 0; j < 64; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 64; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[128] = {0};
        for (int i = 0; i < 64; ++i) for (int j = 0; j < 64; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 64; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint4096_t res;
        for (int i = 0; i < 64; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint4096_t& operator*=(const uint4096_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint4096_t operator/(const uint4096_t& divisor) const {
        if (divisor.is_zero()) return uint4096_t(0);
        uint4096_t quotient(0);
        uint4096_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint4096_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint4096_t& operator/=(const uint4096_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint4096_t operator%(const uint4096_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint4096_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint4096_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint4096_t& rhs) const {
        return std::equal(words, words + 64, rhs.words);
    }
    constexpr bool operator!=(const uint4096_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint4096_t& rhs) const {
        for (int i = 63; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint4096_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint4096_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint4096_t& rhs) const { return !(*this > rhs); }
    constexpr uint4096_t operator<<(int n) const { uint4096_t res = *this; res <<= n; return res; }
    constexpr uint4096_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 4096) {
            std::fill(words, words + 64, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[64] = {0};
        for (int i = 63; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 63) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 63 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint4096_t operator>>(int n) const { uint4096_t res = *this; res >>= n; return res; }
    constexpr uint4096_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 4096) {
            std::fill(words, words + 64, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[64] = {0};
        for (int i = 0; i < 64; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint4096_t operator|(const uint4096_t& rhs) const {
        uint4096_t r;
        for (size_t i=0;i<64;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint4096_t& operator|=(const uint4096_t& rhs) {
        for (size_t i=0;i<64;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 63; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint4096_t& v) {
        if (v.is_zero()) return os << "0";
        char buf[64 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[63]), static_cast<unsigned long long>(v.words[62]),
                      static_cast<unsigned long long>(v.words[61]), static_cast<unsigned long long>(v.words[60]),
                      static_cast<unsigned long long>(v.words[59]), static_cast<unsigned long long>(v.words[58]),
                      static_cast<unsigned long long>(v.words[57]), static_cast<unsigned long long>(v.words[56]),
                      static_cast<unsigned long long>(v.words[55]), static_cast<unsigned long long>(v.words[54]),
                      static_cast<unsigned long long>(v.words[53]), static_cast<unsigned long long>(v.words[52]),
                      static_cast<unsigned long long>(v.words[51]), static_cast<unsigned long long>(v.words[50]),
                      static_cast<unsigned long long>(v.words[49]), static_cast<unsigned long long>(v.words[48]),
                      static_cast<unsigned long long>(v.words[47]), static_cast<unsigned long long>(v.words[46]),
                      static_cast<unsigned long long>(v.words[45]), static_cast<unsigned long long>(v.words[44]),
                      static_cast<unsigned long long>(v.words[43]), static_cast<unsigned long long>(v.words[42]),
                      static_cast<unsigned long long>(v.words[41]), static_cast<unsigned long long>(v.words[40]),
                      static_cast<unsigned long long>(v.words[39]), static_cast<unsigned long long>(v.words[38]),
                      static_cast<unsigned long long>(v.words[37]), static_cast<unsigned long long>(v.words[36]),
                      static_cast<unsigned long long>(v.words[35]), static_cast<unsigned long long>(v.words[34]),
                      static_cast<unsigned long long>(v.words[33]), static_cast<unsigned long long>(v.words[32]),
                      static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                      static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                      static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                      static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                      static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                      static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                      static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                      static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                      static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                      static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                      static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                      static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                      static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                      static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
    }
};

inline std::string toString4096(const uint4096_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint4096_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[64 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[63]), static_cast<unsigned long long>(v.words[62]),
                  static_cast<unsigned long long>(v.words[61]), static_cast<unsigned long long>(v.words[60]),
                  static_cast<unsigned long long>(v.words[59]), static_cast<unsigned long long>(v.words[58]),
                  static_cast<unsigned long long>(v.words[57]), static_cast<unsigned long long>(v.words[56]),
                  static_cast<unsigned long long>(v.words[55]), static_cast<unsigned long long>(v.words[54]),
                  static_cast<unsigned long long>(v.words[53]), static_cast<unsigned long long>(v.words[52]),
                  static_cast<unsigned long long>(v.words[51]), static_cast<unsigned long long>(v.words[50]),
                  static_cast<unsigned long long>(v.words[49]), static_cast<unsigned long long>(v.words[48]),
                  static_cast<unsigned long long>(v.words[47]), static_cast<unsigned long long>(v.words[46]),
                  static_cast<unsigned long long>(v.words[45]), static_cast<unsigned long long>(v.words[44]),
                  static_cast<unsigned long long>(v.words[43]), static_cast<unsigned long long>(v.words[42]),
                  static_cast<unsigned long long>(v.words[41]), static_cast<unsigned long long>(v.words[40]),
                  static_cast<unsigned long long>(v.words[39]), static_cast<unsigned long long>(v.words[38]),
                  static_cast<unsigned long long>(v.words[37]), static_cast<unsigned long long>(v.words[36]),
                  static_cast<unsigned long long>(v.words[35]), static_cast<unsigned long long>(v.words[34]),
                  static_cast<unsigned long long>(v.words[33]), static_cast<unsigned long long>(v.words[32]),
                  static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                  static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                  static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                  static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                  static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                  static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                  static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                  static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                  static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                  static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                  static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                  static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                  static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                  static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint8192_t {
    unsigned long long words[128] = {0};

    constexpr uint8192_t() = default;
    constexpr explicit uint8192_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 128; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 127; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint8192_t operator+(const uint8192_t& rhs) const {
        uint8192_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 128; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint8192_t& operator+=(const uint8192_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint8192_t operator-(const uint8192_t& rhs) const {
        uint8192_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 128; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint8192_t& operator-=(const uint8192_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint8192_t operator*(const uint8192_t& rhs) const {
        unsigned long long res_words[128] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[256] = {0};
        for (int i = 0; i < 128; ++i) for (int j = 0; j < 128; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 128; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[256] = {0};
        for (int i = 0; i < 128; ++i) for (int j = 0; j < 128; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 128; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint8192_t res;
        for (int i = 0; i < 128; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint8192_t& operator*=(const uint8192_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint8192_t operator/(const uint8192_t& divisor) const {
        if (divisor.is_zero()) return uint8192_t(0);
        uint8192_t quotient(0);
        uint8192_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint8192_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint8192_t& operator/=(const uint8192_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint8192_t operator%(const uint8192_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint8192_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint8192_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint8192_t& rhs) const {
        return std::equal(words, words + 128, rhs.words);
    }
    constexpr bool operator!=(const uint8192_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint8192_t& rhs) const {
        for (int i = 127; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint8192_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint8192_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint8192_t& rhs) const { return !(*this > rhs); }
    constexpr uint8192_t operator<<(int n) const { uint8192_t res = *this; res <<= n; return res; }
    constexpr uint8192_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 8192) {
            std::fill(words, words + 128, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[128] = {0};
        for (int i = 127; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 127) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 127 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint8192_t operator>>(int n) const { uint8192_t res = *this; res >>= n; return res; }
    constexpr uint8192_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 8192) {
            std::fill(words, words + 128, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[128] = {0};
        for (int i = 0; i < 128; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint8192_t operator|(const uint8192_t& rhs) const {
        uint8192_t r;
        for (size_t i=0;i<128;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint8192_t& operator|=(const uint8192_t& rhs) {
        for (size_t i=0;i<128;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 127; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint8192_t& v) {
        if (v.is_zero()) return os << "0";
        char buf[128 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[127]), static_cast<unsigned long long>(v.words[126]),
                      static_cast<unsigned long long>(v.words[125]), static_cast<unsigned long long>(v.words[124]),
                      static_cast<unsigned long long>(v.words[123]), static_cast<unsigned long long>(v.words[122]),
                      static_cast<unsigned long long>(v.words[121]), static_cast<unsigned long long>(v.words[120]),
                      static_cast<unsigned long long>(v.words[119]), static_cast<unsigned long long>(v.words[118]),
                      static_cast<unsigned long long>(v.words[117]), static_cast<unsigned long long>(v.words[116]),
                      static_cast<unsigned long long>(v.words[115]), static_cast<unsigned long long>(v.words[114]),
                      static_cast<unsigned long long>(v.words[113]), static_cast<unsigned long long>(v.words[112]),
                      static_cast<unsigned long long>(v.words[111]), static_cast<unsigned long long>(v.words[110]),
                      static_cast<unsigned long long>(v.words[109]), static_cast<unsigned long long>(v.words[108]),
                      static_cast<unsigned long long>(v.words[107]), static_cast<unsigned long long>(v.words[106]),
                      static_cast<unsigned long long>(v.words[105]), static_cast<unsigned long long>(v.words[104]),
                      static_cast<unsigned long long>(v.words[103]), static_cast<unsigned long long>(v.words[102]),
                      static_cast<unsigned long long>(v.words[101]), static_cast<unsigned long long>(v.words[100]),
                      static_cast<unsigned long long>(v.words[99]), static_cast<unsigned long long>(v.words[98]),
                      static_cast<unsigned long long>(v.words[97]), static_cast<unsigned long long>(v.words[96]),
                      static_cast<unsigned long long>(v.words[95]), static_cast<unsigned long long>(v.words[94]),
                      static_cast<unsigned long long>(v.words[93]), static_cast<unsigned long long>(v.words[92]),
                      static_cast<unsigned long long>(v.words[91]), static_cast<unsigned long long>(v.words[90]),
                      static_cast<unsigned long long>(v.words[89]), static_cast<unsigned long long>(v.words[88]),
                      static_cast<unsigned long long>(v.words[87]), static_cast<unsigned long long>(v.words[86]),
                      static_cast<unsigned long long>(v.words[85]), static_cast<unsigned long long>(v.words[84]),
                      static_cast<unsigned long long>(v.words[83]), static_cast<unsigned long long>(v.words[82]),
                      static_cast<unsigned long long>(v.words[81]), static_cast<unsigned long long>(v.words[80]),
                      static_cast<unsigned long long>(v.words[79]), static_cast<unsigned long long>(v.words[78]),
                      static_cast<unsigned long long>(v.words[77]), static_cast<unsigned long long>(v.words[76]),
                      static_cast<unsigned long long>(v.words[75]), static_cast<unsigned long long>(v.words[74]),
                      static_cast<unsigned long long>(v.words[73]), static_cast<unsigned long long>(v.words[72]),
                      static_cast<unsigned long long>(v.words[71]), static_cast<unsigned long long>(v.words[70]),
                      static_cast<unsigned long long>(v.words[69]), static_cast<unsigned long long>(v.words[68]),
                      static_cast<unsigned long long>(v.words[67]), static_cast<unsigned long long>(v.words[66]),
                      static_cast<unsigned long long>(v.words[65]), static_cast<unsigned long long>(v.words[64]),
                      static_cast<unsigned long long>(v.words[63]), static_cast<unsigned long long>(v.words[62]),
                      static_cast<unsigned long long>(v.words[61]), static_cast<unsigned long long>(v.words[60]),
                      static_cast<unsigned long long>(v.words[59]), static_cast<unsigned long long>(v.words[58]),
                      static_cast<unsigned long long>(v.words[57]), static_cast<unsigned long long>(v.words[56]),
                      static_cast<unsigned long long>(v.words[55]), static_cast<unsigned long long>(v.words[54]),
                      static_cast<unsigned long long>(v.words[53]), static_cast<unsigned long long>(v.words[52]),
                      static_cast<unsigned long long>(v.words[51]), static_cast<unsigned long long>(v.words[50]),
                      static_cast<unsigned long long>(v.words[49]), static_cast<unsigned long long>(v.words[48]),
                      static_cast<unsigned long long>(v.words[47]), static_cast<unsigned long long>(v.words[46]),
                      static_cast<unsigned long long>(v.words[45]), static_cast<unsigned long long>(v.words[44]),
                      static_cast<unsigned long long>(v.words[43]), static_cast<unsigned long long>(v.words[42]),
                      static_cast<unsigned long long>(v.words[41]), static_cast<unsigned long long>(v.words[40]),
                      static_cast<unsigned long long>(v.words[39]), static_cast<unsigned long long>(v.words[38]),
                      static_cast<unsigned long long>(v.words[37]), static_cast<unsigned long long>(v.words[36]),
                      static_cast<unsigned long long>(v.words[35]), static_cast<unsigned long long>(v.words[34]),
                      static_cast<unsigned long long>(v.words[33]), static_cast<unsigned long long>(v.words[32]),
                      static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                      static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                      static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                      static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                      static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                      static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                      static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                      static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                      static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                      static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                      static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                      static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                      static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                      static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
    }
};

inline std::string toString8192(const uint8192_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint8192_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[128 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[127]), static_cast<unsigned long long>(v.words[126]),
                  static_cast<unsigned long long>(v.words[125]), static_cast<unsigned long long>(v.words[124]),
                  static_cast<unsigned long long>(v.words[123]), static_cast<unsigned long long>(v.words[122]),
                  static_cast<unsigned long long>(v.words[121]), static_cast<unsigned long long>(v.words[120]),
                  static_cast<unsigned long long>(v.words[119]), static_cast<unsigned long long>(v.words[118]),
                  static_cast<unsigned long long>(v.words[117]), static_cast<unsigned long long>(v.words[116]),
                  static_cast<unsigned long long>(v.words[115]), static_cast<unsigned long long>(v.words[114]),
                  static_cast<unsigned long long>(v.words[113]), static_cast<unsigned long long>(v.words[112]),
                  static_cast<unsigned long long>(v.words[111]), static_cast<unsigned long long>(v.words[110]),
                  static_cast<unsigned long long>(v.words[109]), static_cast<unsigned long long>(v.words[108]),
                  static_cast<unsigned long long>(v.words[107]), static_cast<unsigned long long>(v.words[106]),
                  static_cast<unsigned long long>(v.words[105]), static_cast<unsigned long long>(v.words[104]),
                  static_cast<unsigned long long>(v.words[103]), static_cast<unsigned long long>(v.words[102]),
                  static_cast<unsigned long long>(v.words[101]), static_cast<unsigned long long>(v.words[100]),
                  static_cast<unsigned long long>(v.words[99]), static_cast<unsigned long long>(v.words[98]),
                  static_cast<unsigned long long>(v.words[97]), static_cast<unsigned long long>(v.words[96]),
                  static_cast<unsigned long long>(v.words[95]), static_cast<unsigned long long>(v.words[94]),
                  static_cast<unsigned long long>(v.words[93]), static_cast<unsigned long long>(v.words[92]),
                  static_cast<unsigned long long>(v.words[91]), static_cast<unsigned long long>(v.words[90]),
                  static_cast<unsigned long long>(v.words[89]), static_cast<unsigned long long>(v.words[88]),
                  static_cast<unsigned long long>(v.words[87]), static_cast<unsigned long long>(v.words[86]),
                  static_cast<unsigned long long>(v.words[85]), static_cast<unsigned long long>(v.words[84]),
                  static_cast<unsigned long long>(v.words[83]), static_cast<unsigned long long>(v.words[82]),
                  static_cast<unsigned long long>(v.words[81]), static_cast<unsigned long long>(v.words[80]),
                  static_cast<unsigned long long>(v.words[79]), static_cast<unsigned long long>(v.words[78]),
                  static_cast<unsigned long long>(v.words[77]), static_cast<unsigned long long>(v.words[76]),
                  static_cast<unsigned long long>(v.words[75]), static_cast<unsigned long long>(v.words[74]),
                  static_cast<unsigned long long>(v.words[73]), static_cast<unsigned long long>(v.words[72]),
                  static_cast<unsigned long long>(v.words[71]), static_cast<unsigned long long>(v.words[70]),
                  static_cast<unsigned long long>(v.words[69]), static_cast<unsigned long long>(v.words[68]),
                  static_cast<unsigned long long>(v.words[67]), static_cast<unsigned long long>(v.words[66]),
                  static_cast<unsigned long long>(v.words[65]), static_cast<unsigned long long>(v.words[64]),
                  static_cast<unsigned long long>(v.words[63]), static_cast<unsigned long long>(v.words[62]),
                  static_cast<unsigned long long>(v.words[61]), static_cast<unsigned long long>(v.words[60]),
                  static_cast<unsigned long long>(v.words[59]), static_cast<unsigned long long>(v.words[58]),
                  static_cast<unsigned long long>(v.words[57]), static_cast<unsigned long long>(v.words[56]),
                  static_cast<unsigned long long>(v.words[55]), static_cast<unsigned long long>(v.words[54]),
                  static_cast<unsigned long long>(v.words[53]), static_cast<unsigned long long>(v.words[52]),
                  static_cast<unsigned long long>(v.words[51]), static_cast<unsigned long long>(v.words[50]),
                  static_cast<unsigned long long>(v.words[49]), static_cast<unsigned long long>(v.words[48]),
                  static_cast<unsigned long long>(v.words[47]), static_cast<unsigned long long>(v.words[46]),
                  static_cast<unsigned long long>(v.words[45]), static_cast<unsigned long long>(v.words[44]),
                  static_cast<unsigned long long>(v.words[43]), static_cast<unsigned long long>(v.words[42]),
                  static_cast<unsigned long long>(v.words[41]), static_cast<unsigned long long>(v.words[40]),
                  static_cast<unsigned long long>(v.words[39]), static_cast<unsigned long long>(v.words[38]),
                  static_cast<unsigned long long>(v.words[37]), static_cast<unsigned long long>(v.words[36]),
                  static_cast<unsigned long long>(v.words[35]), static_cast<unsigned long long>(v.words[34]),
                  static_cast<unsigned long long>(v.words[33]), static_cast<unsigned long long>(v.words[32]),
                  static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                  static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                  static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                  static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                  static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                  static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                  static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                  static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                  static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                  static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                  static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                  static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                  static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                  static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint16384_t {
    unsigned long long words[256] = {0};

    constexpr uint16384_t() = default;
    constexpr explicit uint16384_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 256; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 255; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint16384_t operator+(const uint16384_t& rhs) const {
        uint16384_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 256; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint16384_t& operator+=(const uint16384_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint16384_t operator-(const uint16384_t& rhs) const {
        uint16384_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 256; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint16384_t& operator-=(const uint16384_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint16384_t operator*(const uint16384_t& rhs) const {
        unsigned long long res_words[256] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[512] = {0};
        for (int i = 0; i < 256; ++i) for (int j = 0; j < 256; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 256; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[512] = {0};
        for (int i = 0; i < 256; ++i) for (int j = 0; j < 256; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 256; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint16384_t res;
        for (int i = 0; i < 256; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint16384_t& operator*=(const uint16384_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint16384_t operator/(const uint16384_t& divisor) const {
        if (divisor.is_zero()) return uint16384_t(0);
        uint16384_t quotient(0);
        uint16384_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint16384_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint16384_t& operator/=(const uint16384_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint16384_t operator%(const uint16384_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint16384_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint16384_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint16384_t& rhs) const {
        return std::equal(words, words + 256, rhs.words);
    }
    constexpr bool operator!=(const uint16384_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint16384_t& rhs) const {
        for (int i = 255; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint16384_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint16384_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint16384_t& rhs) const { return !(*this > rhs); }
    constexpr uint16384_t operator<<(int n) const { uint16384_t res = *this; res <<= n; return res; }
    constexpr uint16384_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 16384) {
            std::fill(words, words + 256, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[256] = {0};
        for (int i = 255; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 255) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 255 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint16384_t operator>>(int n) const { uint16384_t res = *this; res >>= n; return res; }
    constexpr uint16384_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 16384) {
            std::fill(words, words + 256, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[256] = {0};
        for (int i = 0; i < 256; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr uint16384_t operator|(const uint16384_t& rhs) const {
        uint16384_t r;
        for (size_t i=0;i<256;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint16384_t& operator|=(const uint16384_t& rhs) {
        for (size_t i=0;i<256;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 255; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint16384_t& v) {
        if (v.is_zero()) return os << "0";
        char buf[256 * 16 + 1];
        std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                      static_cast<unsigned long long>(v.words[255]), static_cast<unsigned long long>(v.words[254]),
                      static_cast<unsigned long long>(v.words[253]), static_cast<unsigned long long>(v.words[252]),
                      static_cast<unsigned long long>(v.words[251]), static_cast<unsigned long long>(v.words[250]),
                      static_cast<unsigned long long>(v.words[249]), static_cast<unsigned long long>(v.words[248]),
                      static_cast<unsigned long long>(v.words[247]), static_cast<unsigned long long>(v.words[246]),
                      static_cast<unsigned long long>(v.words[245]), static_cast<unsigned long long>(v.words[244]),
                      static_cast<unsigned long long>(v.words[243]), static_cast<unsigned long long>(v.words[242]),
                      static_cast<unsigned long long>(v.words[241]), static_cast<unsigned long long>(v.words[240]),
                      static_cast<unsigned long long>(v.words[239]), static_cast<unsigned long long>(v.words[238]),
                      static_cast<unsigned long long>(v.words[237]), static_cast<unsigned long long>(v.words[236]),
                      static_cast<unsigned long long>(v.words[235]), static_cast<unsigned long long>(v.words[234]),
                      static_cast<unsigned long long>(v.words[233]), static_cast<unsigned long long>(v.words[232]),
                      static_cast<unsigned long long>(v.words[231]), static_cast<unsigned long long>(v.words[230]),
                      static_cast<unsigned long long>(v.words[229]), static_cast<unsigned long long>(v.words[228]),
                      static_cast<unsigned long long>(v.words[227]), static_cast<unsigned long long>(v.words[226]),
                      static_cast<unsigned long long>(v.words[225]), static_cast<unsigned long long>(v.words[224]),
                      static_cast<unsigned long long>(v.words[223]), static_cast<unsigned long long>(v.words[222]),
                      static_cast<unsigned long long>(v.words[221]), static_cast<unsigned long long>(v.words[220]),
                      static_cast<unsigned long long>(v.words[219]), static_cast<unsigned long long>(v.words[218]),
                      static_cast<unsigned long long>(v.words[217]), static_cast<unsigned long long>(v.words[216]),
                      static_cast<unsigned long long>(v.words[215]), static_cast<unsigned long long>(v.words[214]),
                      static_cast<unsigned long long>(v.words[213]), static_cast<unsigned long long>(v.words[212]),
                      static_cast<unsigned long long>(v.words[211]), static_cast<unsigned long long>(v.words[210]),
                      static_cast<unsigned long long>(v.words[209]), static_cast<unsigned long long>(v.words[208]),
                      static_cast<unsigned long long>(v.words[207]), static_cast<unsigned long long>(v.words[206]),
                      static_cast<unsigned long long>(v.words[205]), static_cast<unsigned long long>(v.words[204]),
                      static_cast<unsigned long long>(v.words[203]), static_cast<unsigned long long>(v.words[202]),
                      static_cast<unsigned long long>(v.words[201]), static_cast<unsigned long long>(v.words[200]),
                      static_cast<unsigned long long>(v.words[199]), static_cast<unsigned long long>(v.words[198]),
                      static_cast<unsigned long long>(v.words[197]), static_cast<unsigned long long>(v.words[196]),
                      static_cast<unsigned long long>(v.words[195]), static_cast<unsigned long long>(v.words[194]),
                      static_cast<unsigned long long>(v.words[193]), static_cast<unsigned long long>(v.words[192]),
                      static_cast<unsigned long long>(v.words[191]), static_cast<unsigned long long>(v.words[190]),
                      static_cast<unsigned long long>(v.words[189]), static_cast<unsigned long long>(v.words[188]),
                      static_cast<unsigned long long>(v.words[187]), static_cast<unsigned long long>(v.words[186]),
                      static_cast<unsigned long long>(v.words[185]), static_cast<unsigned long long>(v.words[184]),
                      static_cast<unsigned long long>(v.words[183]), static_cast<unsigned long long>(v.words[182]),
                      static_cast<unsigned long long>(v.words[181]), static_cast<unsigned long long>(v.words[180]),
                      static_cast<unsigned long long>(v.words[179]), static_cast<unsigned long long>(v.words[178]),
                      static_cast<unsigned long long>(v.words[177]), static_cast<unsigned long long>(v.words[176]),
                      static_cast<unsigned long long>(v.words[175]), static_cast<unsigned long long>(v.words[174]),
                      static_cast<unsigned long long>(v.words[173]), static_cast<unsigned long long>(v.words[172]),
                      static_cast<unsigned long long>(v.words[171]), static_cast<unsigned long long>(v.words[170]),
                      static_cast<unsigned long long>(v.words[169]), static_cast<unsigned long long>(v.words[168]),
                      static_cast<unsigned long long>(v.words[167]), static_cast<unsigned long long>(v.words[166]),
                      static_cast<unsigned long long>(v.words[165]), static_cast<unsigned long long>(v.words[164]),
                      static_cast<unsigned long long>(v.words[163]), static_cast<unsigned long long>(v.words[162]),
                      static_cast<unsigned long long>(v.words[161]), static_cast<unsigned long long>(v.words[160]),
                      static_cast<unsigned long long>(v.words[159]), static_cast<unsigned long long>(v.words[158]),
                      static_cast<unsigned long long>(v.words[157]), static_cast<unsigned long long>(v.words[156]),
                      static_cast<unsigned long long>(v.words[155]), static_cast<unsigned long long>(v.words[154]),
                      static_cast<unsigned long long>(v.words[153]), static_cast<unsigned long long>(v.words[152]),
                      static_cast<unsigned long long>(v.words[151]), static_cast<unsigned long long>(v.words[150]),
                      static_cast<unsigned long long>(v.words[149]), static_cast<unsigned long long>(v.words[148]),
                      static_cast<unsigned long long>(v.words[147]), static_cast<unsigned long long>(v.words[146]),
                      static_cast<unsigned long long>(v.words[145]), static_cast<unsigned long long>(v.words[144]),
                      static_cast<unsigned long long>(v.words[143]), static_cast<unsigned long long>(v.words[142]),
                      static_cast<unsigned long long>(v.words[141]), static_cast<unsigned long long>(v.words[140]),
                      static_cast<unsigned long long>(v.words[139]), static_cast<unsigned long long>(v.words[138]),
                      static_cast<unsigned long long>(v.words[137]), static_cast<unsigned long long>(v.words[136]),
                      static_cast<unsigned long long>(v.words[135]), static_cast<unsigned long long>(v.words[134]),
                      static_cast<unsigned long long>(v.words[133]), static_cast<unsigned long long>(v.words[132]),
                      static_cast<unsigned long long>(v.words[131]), static_cast<unsigned long long>(v.words[130]),
                      static_cast<unsigned long long>(v.words[129]), static_cast<unsigned long long>(v.words[128]),
                      static_cast<unsigned long long>(v.words[127]), static_cast<unsigned long long>(v.words[126]),
                      static_cast<unsigned long long>(v.words[125]), static_cast<unsigned long long>(v.words[124]),
                      static_cast<unsigned long long>(v.words[123]), static_cast<unsigned long long>(v.words[122]),
                      static_cast<unsigned long long>(v.words[121]), static_cast<unsigned long long>(v.words[120]),
                      static_cast<unsigned long long>(v.words[119]), static_cast<unsigned long long>(v.words[118]),
                      static_cast<unsigned long long>(v.words[117]), static_cast<unsigned long long>(v.words[116]),
                      static_cast<unsigned long long>(v.words[115]), static_cast<unsigned long long>(v.words[114]),
                      static_cast<unsigned long long>(v.words[113]), static_cast<unsigned long long>(v.words[112]),
                      static_cast<unsigned long long>(v.words[111]), static_cast<unsigned long long>(v.words[110]),
                      static_cast<unsigned long long>(v.words[109]), static_cast<unsigned long long>(v.words[108]),
                      static_cast<unsigned long long>(v.words[107]), static_cast<unsigned long long>(v.words[106]),
                      static_cast<unsigned long long>(v.words[105]), static_cast<unsigned long long>(v.words[104]),
                      static_cast<unsigned long long>(v.words[103]), static_cast<unsigned long long>(v.words[102]),
                      static_cast<unsigned long long>(v.words[101]), static_cast<unsigned long long>(v.words[100]),
                      static_cast<unsigned long long>(v.words[99]), static_cast<unsigned long long>(v.words[98]),
                      static_cast<unsigned long long>(v.words[97]), static_cast<unsigned long long>(v.words[96]),
                      static_cast<unsigned long long>(v.words[95]), static_cast<unsigned long long>(v.words[94]),
                      static_cast<unsigned long long>(v.words[93]), static_cast<unsigned long long>(v.words[92]),
                      static_cast<unsigned long long>(v.words[91]), static_cast<unsigned long long>(v.words[90]),
                      static_cast<unsigned long long>(v.words[89]), static_cast<unsigned long long>(v.words[88]),
                      static_cast<unsigned long long>(v.words[87]), static_cast<unsigned long long>(v.words[86]),
                      static_cast<unsigned long long>(v.words[85]), static_cast<unsigned long long>(v.words[84]),
                      static_cast<unsigned long long>(v.words[83]), static_cast<unsigned long long>(v.words[82]),
                      static_cast<unsigned long long>(v.words[81]), static_cast<unsigned long long>(v.words[80]),
                      static_cast<unsigned long long>(v.words[79]), static_cast<unsigned long long>(v.words[78]),
                      static_cast<unsigned long long>(v.words[77]), static_cast<unsigned long long>(v.words[76]),
                      static_cast<unsigned long long>(v.words[75]), static_cast<unsigned long long>(v.words[74]),
                      static_cast<unsigned long long>(v.words[73]), static_cast<unsigned long long>(v.words[72]),
                      static_cast<unsigned long long>(v.words[71]), static_cast<unsigned long long>(v.words[70]),
                      static_cast<unsigned long long>(v.words[69]), static_cast<unsigned long long>(v.words[68]),
                      static_cast<unsigned long long>(v.words[67]), static_cast<unsigned long long>(v.words[66]),
                      static_cast<unsigned long long>(v.words[65]), static_cast<unsigned long long>(v.words[64]),
                      static_cast<unsigned long long>(v.words[63]), static_cast<unsigned long long>(v.words[62]),
                      static_cast<unsigned long long>(v.words[61]), static_cast<unsigned long long>(v.words[60]),
                      static_cast<unsigned long long>(v.words[59]), static_cast<unsigned long long>(v.words[58]),
                      static_cast<unsigned long long>(v.words[57]), static_cast<unsigned long long>(v.words[56]),
                      static_cast<unsigned long long>(v.words[55]), static_cast<unsigned long long>(v.words[54]),
                      static_cast<unsigned long long>(v.words[53]), static_cast<unsigned long long>(v.words[52]),
                      static_cast<unsigned long long>(v.words[51]), static_cast<unsigned long long>(v.words[50]),
                      static_cast<unsigned long long>(v.words[49]), static_cast<unsigned long long>(v.words[48]),
                      static_cast<unsigned long long>(v.words[47]), static_cast<unsigned long long>(v.words[46]),
                      static_cast<unsigned long long>(v.words[45]), static_cast<unsigned long long>(v.words[44]),
                      static_cast<unsigned long long>(v.words[43]), static_cast<unsigned long long>(v.words[42]),
                      static_cast<unsigned long long>(v.words[41]), static_cast<unsigned long long>(v.words[40]),
                      static_cast<unsigned long long>(v.words[39]), static_cast<unsigned long long>(v.words[38]),
                      static_cast<unsigned long long>(v.words[37]), static_cast<unsigned long long>(v.words[36]),
                      static_cast<unsigned long long>(v.words[35]), static_cast<unsigned long long>(v.words[34]),
                      static_cast<unsigned long long>(v.words[33]), static_cast<unsigned long long>(v.words[32]),
                      static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                      static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                      static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                      static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                      static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                      static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                      static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                      static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                      static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                      static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                      static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                      static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                      static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                      static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                      static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                      static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
        std::string out(buf);
        size_t pos = out.find_first_not_of('0');
        if (pos == std::string::npos) return os << "0";
        return os << out.substr(pos);
    }
};

inline std::string toString16384(const uint16384_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint16384_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    char buf[256 * 16 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx%016llx",
                  static_cast<unsigned long long>(v.words[255]), static_cast<unsigned long long>(v.words[254]),
                  static_cast<unsigned long long>(v.words[253]), static_cast<unsigned long long>(v.words[252]),
                  static_cast<unsigned long long>(v.words[251]), static_cast<unsigned long long>(v.words[250]),
                  static_cast<unsigned long long>(v.words[249]), static_cast<unsigned long long>(v.words[248]),
                  static_cast<unsigned long long>(v.words[247]), static_cast<unsigned long long>(v.words[246]),
                  static_cast<unsigned long long>(v.words[245]), static_cast<unsigned long long>(v.words[244]),
                  static_cast<unsigned long long>(v.words[243]), static_cast<unsigned long long>(v.words[242]),
                  static_cast<unsigned long long>(v.words[241]), static_cast<unsigned long long>(v.words[240]),
                  static_cast<unsigned long long>(v.words[239]), static_cast<unsigned long long>(v.words[238]),
                  static_cast<unsigned long long>(v.words[237]), static_cast<unsigned long long>(v.words[236]),
                  static_cast<unsigned long long>(v.words[235]), static_cast<unsigned long long>(v.words[234]),
                  static_cast<unsigned long long>(v.words[233]), static_cast<unsigned long long>(v.words[232]),
                  static_cast<unsigned long long>(v.words[231]), static_cast<unsigned long long>(v.words[230]),
                  static_cast<unsigned long long>(v.words[229]), static_cast<unsigned long long>(v.words[228]),
                  static_cast<unsigned long long>(v.words[227]), static_cast<unsigned long long>(v.words[226]),
                  static_cast<unsigned long long>(v.words[225]), static_cast<unsigned long long>(v.words[224]),
                  static_cast<unsigned long long>(v.words[223]), static_cast<unsigned long long>(v.words[222]),
                  static_cast<unsigned long long>(v.words[221]), static_cast<unsigned long long>(v.words[220]),
                  static_cast<unsigned long long>(v.words[219]), static_cast<unsigned long long>(v.words[218]),
                  static_cast<unsigned long long>(v.words[217]), static_cast<unsigned long long>(v.words[216]),
                  static_cast<unsigned long long>(v.words[215]), static_cast<unsigned long long>(v.words[214]),
                  static_cast<unsigned long long>(v.words[213]), static_cast<unsigned long long>(v.words[212]),
                  static_cast<unsigned long long>(v.words[211]), static_cast<unsigned long long>(v.words[210]),
                  static_cast<unsigned long long>(v.words[209]), static_cast<unsigned long long>(v.words[208]),
                  static_cast<unsigned long long>(v.words[207]), static_cast<unsigned long long>(v.words[206]),
                  static_cast<unsigned long long>(v.words[205]), static_cast<unsigned long long>(v.words[204]),
                  static_cast<unsigned long long>(v.words[203]), static_cast<unsigned long long>(v.words[202]),
                  static_cast<unsigned long long>(v.words[201]), static_cast<unsigned long long>(v.words[200]),
                  static_cast<unsigned long long>(v.words[199]), static_cast<unsigned long long>(v.words[198]),
                  static_cast<unsigned long long>(v.words[197]), static_cast<unsigned long long>(v.words[196]),
                  static_cast<unsigned long long>(v.words[195]), static_cast<unsigned long long>(v.words[194]),
                  static_cast<unsigned long long>(v.words[193]), static_cast<unsigned long long>(v.words[192]),
                  static_cast<unsigned long long>(v.words[191]), static_cast<unsigned long long>(v.words[190]),
                  static_cast<unsigned long long>(v.words[189]), static_cast<unsigned long long>(v.words[188]),
                  static_cast<unsigned long long>(v.words[187]), static_cast<unsigned long long>(v.words[186]),
                  static_cast<unsigned long long>(v.words[185]), static_cast<unsigned long long>(v.words[184]),
                  static_cast<unsigned long long>(v.words[183]), static_cast<unsigned long long>(v.words[182]),
                  static_cast<unsigned long long>(v.words[181]), static_cast<unsigned long long>(v.words[180]),
                  static_cast<unsigned long long>(v.words[179]), static_cast<unsigned long long>(v.words[178]),
                  static_cast<unsigned long long>(v.words[177]), static_cast<unsigned long long>(v.words[176]),
                  static_cast<unsigned long long>(v.words[175]), static_cast<unsigned long long>(v.words[174]),
                  static_cast<unsigned long long>(v.words[173]), static_cast<unsigned long long>(v.words[172]),
                  static_cast<unsigned long long>(v.words[171]), static_cast<unsigned long long>(v.words[170]),
                  static_cast<unsigned long long>(v.words[169]), static_cast<unsigned long long>(v.words[168]),
                  static_cast<unsigned long long>(v.words[167]), static_cast<unsigned long long>(v.words[166]),
                  static_cast<unsigned long long>(v.words[165]), static_cast<unsigned long long>(v.words[164]),
                  static_cast<unsigned long long>(v.words[163]), static_cast<unsigned long long>(v.words[162]),
                  static_cast<unsigned long long>(v.words[161]), static_cast<unsigned long long>(v.words[160]),
                  static_cast<unsigned long long>(v.words[159]), static_cast<unsigned long long>(v.words[158]),
                  static_cast<unsigned long long>(v.words[157]), static_cast<unsigned long long>(v.words[156]),
                  static_cast<unsigned long long>(v.words[155]), static_cast<unsigned long long>(v.words[154]),
                  static_cast<unsigned long long>(v.words[153]), static_cast<unsigned long long>(v.words[152]),
                  static_cast<unsigned long long>(v.words[151]), static_cast<unsigned long long>(v.words[150]),
                  static_cast<unsigned long long>(v.words[149]), static_cast<unsigned long long>(v.words[148]),
                  static_cast<unsigned long long>(v.words[147]), static_cast<unsigned long long>(v.words[146]),
                  static_cast<unsigned long long>(v.words[145]), static_cast<unsigned long long>(v.words[144]),
                  static_cast<unsigned long long>(v.words[143]), static_cast<unsigned long long>(v.words[142]),
                  static_cast<unsigned long long>(v.words[141]), static_cast<unsigned long long>(v.words[140]),
                  static_cast<unsigned long long>(v.words[139]), static_cast<unsigned long long>(v.words[138]),
                  static_cast<unsigned long long>(v.words[137]), static_cast<unsigned long long>(v.words[136]),
                  static_cast<unsigned long long>(v.words[135]), static_cast<unsigned long long>(v.words[134]),
                  static_cast<unsigned long long>(v.words[133]), static_cast<unsigned long long>(v.words[132]),
                  static_cast<unsigned long long>(v.words[131]), static_cast<unsigned long long>(v.words[130]),
                  static_cast<unsigned long long>(v.words[129]), static_cast<unsigned long long>(v.words[128]),
                  static_cast<unsigned long long>(v.words[127]), static_cast<unsigned long long>(v.words[126]),
                  static_cast<unsigned long long>(v.words[125]), static_cast<unsigned long long>(v.words[124]),
                  static_cast<unsigned long long>(v.words[123]), static_cast<unsigned long long>(v.words[122]),
                  static_cast<unsigned long long>(v.words[121]), static_cast<unsigned long long>(v.words[120]),
                  static_cast<unsigned long long>(v.words[119]), static_cast<unsigned long long>(v.words[118]),
                  static_cast<unsigned long long>(v.words[117]), static_cast<unsigned long long>(v.words[116]),
                  static_cast<unsigned long long>(v.words[115]), static_cast<unsigned long long>(v.words[114]),
                  static_cast<unsigned long long>(v.words[113]), static_cast<unsigned long long>(v.words[112]),
                  static_cast<unsigned long long>(v.words[111]), static_cast<unsigned long long>(v.words[110]),
                  static_cast<unsigned long long>(v.words[109]), static_cast<unsigned long long>(v.words[108]),
                  static_cast<unsigned long long>(v.words[107]), static_cast<unsigned long long>(v.words[106]),
                  static_cast<unsigned long long>(v.words[105]), static_cast<unsigned long long>(v.words[104]),
                  static_cast<unsigned long long>(v.words[103]), static_cast<unsigned long long>(v.words[102]),
                  static_cast<unsigned long long>(v.words[101]), static_cast<unsigned long long>(v.words[100]),
                  static_cast<unsigned long long>(v.words[99]), static_cast<unsigned long long>(v.words[98]),
                  static_cast<unsigned long long>(v.words[97]), static_cast<unsigned long long>(v.words[96]),
                  static_cast<unsigned long long>(v.words[95]), static_cast<unsigned long long>(v.words[94]),
                  static_cast<unsigned long long>(v.words[93]), static_cast<unsigned long long>(v.words[92]),
                  static_cast<unsigned long long>(v.words[91]), static_cast<unsigned long long>(v.words[90]),
                  static_cast<unsigned long long>(v.words[89]), static_cast<unsigned long long>(v.words[88]),
                  static_cast<unsigned long long>(v.words[87]), static_cast<unsigned long long>(v.words[86]),
                  static_cast<unsigned long long>(v.words[85]), static_cast<unsigned long long>(v.words[84]),
                  static_cast<unsigned long long>(v.words[83]), static_cast<unsigned long long>(v.words[82]),
                  static_cast<unsigned long long>(v.words[81]), static_cast<unsigned long long>(v.words[80]),
                  static_cast<unsigned long long>(v.words[79]), static_cast<unsigned long long>(v.words[78]),
                  static_cast<unsigned long long>(v.words[77]), static_cast<unsigned long long>(v.words[76]),
                  static_cast<unsigned long long>(v.words[75]), static_cast<unsigned long long>(v.words[74]),
                  static_cast<unsigned long long>(v.words[73]), static_cast<unsigned long long>(v.words[72]),
                  static_cast<unsigned long long>(v.words[71]), static_cast<unsigned long long>(v.words[70]),
                  static_cast<unsigned long long>(v.words[69]), static_cast<unsigned long long>(v.words[68]),
                  static_cast<unsigned long long>(v.words[67]), static_cast<unsigned long long>(v.words[66]),
                  static_cast<unsigned long long>(v.words[65]), static_cast<unsigned long long>(v.words[64]),
                  static_cast<unsigned long long>(v.words[63]), static_cast<unsigned long long>(v.words[62]),
                  static_cast<unsigned long long>(v.words[61]), static_cast<unsigned long long>(v.words[60]),
                  static_cast<unsigned long long>(v.words[59]), static_cast<unsigned long long>(v.words[58]),
                  static_cast<unsigned long long>(v.words[57]), static_cast<unsigned long long>(v.words[56]),
                  static_cast<unsigned long long>(v.words[55]), static_cast<unsigned long long>(v.words[54]),
                  static_cast<unsigned long long>(v.words[53]), static_cast<unsigned long long>(v.words[52]),
                  static_cast<unsigned long long>(v.words[51]), static_cast<unsigned long long>(v.words[50]),
                  static_cast<unsigned long long>(v.words[49]), static_cast<unsigned long long>(v.words[48]),
                  static_cast<unsigned long long>(v.words[47]), static_cast<unsigned long long>(v.words[46]),
                  static_cast<unsigned long long>(v.words[45]), static_cast<unsigned long long>(v.words[44]),
                  static_cast<unsigned long long>(v.words[43]), static_cast<unsigned long long>(v.words[42]),
                  static_cast<unsigned long long>(v.words[41]), static_cast<unsigned long long>(v.words[40]),
                  static_cast<unsigned long long>(v.words[39]), static_cast<unsigned long long>(v.words[38]),
                  static_cast<unsigned long long>(v.words[37]), static_cast<unsigned long long>(v.words[36]),
                  static_cast<unsigned long long>(v.words[35]), static_cast<unsigned long long>(v.words[34]),
                  static_cast<unsigned long long>(v.words[33]), static_cast<unsigned long long>(v.words[32]),
                  static_cast<unsigned long long>(v.words[31]), static_cast<unsigned long long>(v.words[30]),
                  static_cast<unsigned long long>(v.words[29]), static_cast<unsigned long long>(v.words[28]),
                  static_cast<unsigned long long>(v.words[27]), static_cast<unsigned long long>(v.words[26]),
                  static_cast<unsigned long long>(v.words[25]), static_cast<unsigned long long>(v.words[24]),
                  static_cast<unsigned long long>(v.words[23]), static_cast<unsigned long long>(v.words[22]),
                  static_cast<unsigned long long>(v.words[21]), static_cast<unsigned long long>(v.words[20]),
                  static_cast<unsigned long long>(v.words[19]), static_cast<unsigned long long>(v.words[18]),
                  static_cast<unsigned long long>(v.words[17]), static_cast<unsigned long long>(v.words[16]),
                  static_cast<unsigned long long>(v.words[15]), static_cast<unsigned long long>(v.words[14]),
                  static_cast<unsigned long long>(v.words[13]), static_cast<unsigned long long>(v.words[12]),
                  static_cast<unsigned long long>(v.words[11]), static_cast<unsigned long long>(v.words[10]),
                  static_cast<unsigned long long>(v.words[9]), static_cast<unsigned long long>(v.words[8]),
                  static_cast<unsigned long long>(v.words[7]), static_cast<unsigned long long>(v.words[6]),
                  static_cast<unsigned long long>(v.words[5]), static_cast<unsigned long long>(v.words[4]),
                  static_cast<unsigned long long>(v.words[3]), static_cast<unsigned long long>(v.words[2]),
                  static_cast<unsigned long long>(v.words[1]), static_cast<unsigned long long>(v.words[0]));
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}

struct uint32768_t {
    unsigned long long words[512] = {0};

    constexpr uint32768_t() = default;
    constexpr explicit uint32768_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 512; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 511; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint32768_t operator+(const uint32768_t& rhs) const {
        uint32768_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 512; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint32768_t& operator+=(const uint32768_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint32768_t operator-(const uint32768_t& rhs) const {
        uint32768_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 512; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint32768_t& operator-=(const uint32768_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint32768_t operator*(const uint32768_t& rhs) const {
        unsigned long long res_words[512] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[1024] = {0};
        for (int i = 0; i < 512; ++i) for (int j = 0; j < 512; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 512; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[1024] = {0};
        for (int i = 0; i < 512; ++i) for (int j = 0; j < 512; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 512; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint32768_t res;
        for (int i = 0; i < 512; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint32768_t& operator*=(const uint32768_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint32768_t operator/(const uint32768_t& divisor) const {
        if (divisor.is_zero()) return uint32768_t(0);
        uint32768_t quotient(0);
        uint32768_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint32768_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint32768_t& operator/=(const uint32768_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint32768_t operator%(const uint32768_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint32768_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint32768_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint32768_t& rhs) const {
        for (size_t i = 0; i < 512; ++i) if (words[i] != rhs.words[i]) return false;
        return true;
    }
    constexpr bool operator!=(const uint32768_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint32768_t& rhs) const {
        for (int i = 511; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint32768_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint32768_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint32768_t& rhs) const { return !(*this > rhs); }
    constexpr uint32768_t operator<<(int n) const { uint32768_t res = *this; res <<= n; return res; }
    constexpr uint32768_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 32768) { for (size_t i=0; i<512; ++i) words[i]=0; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[512] = {0};
        for (int i = 511; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 511) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 511 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        for (size_t i = 0; i < 512; ++i) words[i] = nw[i];
        return *this;
    }

    constexpr uint32768_t operator>>(int n) const { uint32768_t res = *this; res >>= n; return res; }
    constexpr uint32768_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 32768) { for (size_t i=0; i<512; ++i) words[i]=0; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[512] = {0};
        for (int i = 0; i < 512; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        for (size_t i = 0; i < 512; ++i) words[i] = nw[i];
        return *this;
    }

    constexpr uint32768_t operator|(const uint32768_t& rhs) const {
        uint32768_t r;
        for (size_t i=0;i<512;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint32768_t& operator|=(const uint32768_t& rhs) {
        for (size_t i=0;i<512;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 511; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint32768_t& v) {
        if (v.is_zero()) return os << "0";
#if defined(__SIZEOF_INT128__)
        uint32768_t temp = v;
        std::string s;
        while (!temp.is_zero()) {
            s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
        }
        for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
        return os << s;
#else
        std::string out;
        bool leading = true;
        for (int i = 511; i >= 0; --i) {
            unsigned long long val = static_cast<unsigned long long>(v.words[i]);
            char buf[17];
            if (leading) {
                if (val == 0) continue;
                std::snprintf(buf, sizeof(buf), "%llx", val);
                out += buf;
                leading = false;
            } else {
                std::snprintf(buf, sizeof(buf), "%016llx", val);
                out += buf;
            }
        }
        return os << (out.empty() ? "0" : out);
#endif
    }
};

inline std::string toString32768(const uint32768_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint32768_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
    return s;
#else
    std::string out;
    out.reserve(512 * 16 + 1);
    bool leading = true;
    for (int i = 511; i >= 0; --i) {
        unsigned long long val = v.words[i];
        if (leading) {
            if (val == 0) continue;
            char buf[17];
            std::snprintf(buf, sizeof(buf), "%llx", static_cast<unsigned long long>(val));
            out += buf;
            leading = false;
        } else {
            char buf[17];
            std::snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(val));
            out += buf;
        }
    }
    return out.empty() ? "0" : out;
#endif
}

struct uint65536_t {
    unsigned long long words[1024] = {0};

    constexpr uint65536_t() = default;
    constexpr explicit uint65536_t(unsigned long long v) : words{{v}} {}

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 1024; ++i) if (words[i] != 0) return false;
        return true;
    }
    constexpr int bit_width() const {
        for (int i = 1023; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint65536_t operator+(const uint65536_t& rhs) const {
        uint65536_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 1024; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<unsigned long long>(carry);
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + static_cast<unsigned long long>(carry);
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr uint65536_t& operator+=(const uint65536_t& rhs) { *this = *this + rhs; return *this; }
    constexpr uint65536_t operator-(const uint65536_t& rhs) const {
        uint65536_t res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < 1024; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - static_cast<unsigned long long>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint65536_t& operator-=(const uint65536_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint65536_t operator*(const uint65536_t& rhs) const {
        unsigned long long res_words[1024] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[2048] = {0};
        for (int i = 0; i < 1024; ++i) for (int j = 0; j < 1024; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 1024; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[2048] = {0};
        for (int i = 0; i < 1024; ++i) for (int j = 0; j < 1024; ++j) {
            unsigned long long lo = words[i] * rhs.words[j];
            unsigned long long hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 1024; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint65536_t res;
        for (int i = 0; i < 1024; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint65536_t& operator*=(const uint65536_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint65536_t operator/(const uint65536_t& divisor) const {
        if (divisor.is_zero()) return uint65536_t(0);
        uint65536_t quotient(0);
        uint65536_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint65536_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= divisor) {
                remainder -= divisor;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                quotient.words[word_idx] |= 1ULL << bit_idx;
            }
        }
        return quotient;
    }

    constexpr uint65536_t& operator/=(const uint65536_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint65536_t operator%(const uint65536_t& rhs) const {
        if (rhs.is_zero()) return *this;
        uint65536_t remainder(0);
        int bw = bit_width();
        if (bw == 0) return uint65536_t(0);
        for (int i = bw - 1; i >= 0; --i) {
            remainder <<= 1;
            unsigned long long bit = (words[i / 64] >> (i % 64)) & 1;
            remainder.words[0] |= bit;
            if (remainder >= rhs) {
                remainder -= rhs;
            }
        }
        return remainder;
    }

    constexpr bool operator==(const uint65536_t& rhs) const {
        for (size_t i = 0; i < 1024; ++i) if (words[i] != rhs.words[i]) return false;
        return true;
    }
    constexpr bool operator!=(const uint65536_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint65536_t& rhs) const {
        for (int i = 1023; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint65536_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint65536_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint65536_t& rhs) const { return !(*this > rhs); }
    constexpr uint65536_t operator<<(int n) const { uint65536_t res = *this; res <<= n; return res; }
    constexpr uint65536_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 65536) { for (size_t i=0; i<1024; ++i) words[i]=0; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[1024] = {0};
        for (int i = 1023; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 1023) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 1023 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        for (size_t i = 0; i < 1024; ++i) words[i] = nw[i];
        return *this;
    }

    constexpr uint65536_t operator>>(int n) const { uint65536_t res = *this; res >>= n; return res; }
    constexpr uint65536_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 65536) { for (size_t i=0; i<1024; ++i) words[i]=0; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[1024] = {0};
        for (int i = 0; i < 1024; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        for (size_t i = 0; i < 1024; ++i) words[i] = nw[i];
        return *this;
    }

    constexpr uint65536_t operator|(const uint65536_t& rhs) const {
        uint65536_t r;
        for (size_t i=0;i<1024;++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint65536_t& operator|=(const uint65536_t& rhs) {
        for (size_t i=0;i<1024;++i) words[i] |= rhs.words[i];
        return *this;
    }

#if defined(__SIZEOF_INT128__)
    unsigned long long div_mod_uint64(unsigned long long m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 1023; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<unsigned long long>(cur / m);
            rem = cur % m;
        }
        return static_cast<unsigned long long>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint65536_t& v) {
        if (v.is_zero()) return os << "0";
#if defined(__SIZEOF_INT128__)
        uint65536_t temp = v;
        std::string s;
        while (!temp.is_zero()) {
            s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
        }
        for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
        return os << s;
#else
        std::string out;
        bool leading = true;
        for (int i = 1023; i >= 0; --i) {
            unsigned long long val = static_cast<unsigned long long>(v.words[i]);
            char buf[17];
            if (leading) {
                if (val == 0) continue;
                std::snprintf(buf, sizeof(buf), "%llx", val);
                out += buf;
                leading = false;
            } else {
                std::snprintf(buf, sizeof(buf), "%016llx", val);
                out += buf;
            }
        }
        return os << (out.empty() ? "0" : out);
#endif
    }
};

inline std::string toString65536(const uint65536_t& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    uint65536_t temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_uint64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
    return s;
#else
    std::string out;
    out.reserve(1024 * 16 + 1);
    bool leading = true;
    for (int i = 1023; i >= 0; --i) {
        unsigned long long val = v.words[i];
        if (leading) {
            if (val == 0) continue;
            char buf[17];
            std::snprintf(buf, sizeof(buf), "%llx", static_cast<unsigned long long>(val));
            out += buf;
            leading = false;
        } else {
            char buf[17];
            std::snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(val));
            out += buf;
        }
    }
    return out.empty() ? "0" : out;
#endif
}