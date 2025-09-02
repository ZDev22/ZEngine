#pragma once

#include <string>
#include <ostream>
#include <cstdio>
#include <array>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace bigInts {

constexpr int bit_width64(uint64_t x) {
    if (x == 0) return 0;
#if defined(_MSC_VER)
    unsigned long index;
    _BitScanReverse64(&index, x);
    return static_cast<int>(index) + 1;
#else
    return 64 - __builtin_clzll(x);
#endif
}

constexpr uint64_t mul_hi(uint64_t a, uint64_t b) {
    if (std::is_constant_evaluated()) {
        return (((((a & 0xFFFFFFFFu) * (b & 0xFFFFFFFFu)) >> 32) + (((a >> 32) * (b & 0xFFFFFFFFu)) & 0xFFFFFFFFu) + (((a & 0xFFFFFFFFu) * (b >> 32)) & 0xFFFFFFFFu)) >> 32) + (((a >> 32) * (b & 0xFFFFFFFFu)) >> 32) + (((a & 0xFFFFFFFFu) * (b >> 32)) >> 32) + ((a >> 32) * (b >> 32));
    } else {
#if defined(_MSC_VER)
        return __umulh(a, b);
#elif defined(__GNUC__) && defined(__x86_64__)
        return static_cast<uint64_t>((static_cast<__uint128_t>(a) * b) >> 64);
#else
        return (((((a & 0xFFFFFFFFu) * (b & 0xFFFFFFFFu)) >> 32) + (((a >> 32) * (b & 0xFFFFFFFFu)) & 0xFFFFFFFFu) + (((a & 0xFFFFFFFFu) * (b >> 32)) & 0xFFFFFFFFu)) >> 32) + (((a >> 32) * (b & 0xFFFFFFFFu)) >> 32) + (((a & 0xFFFFFFFFu) * (b >> 32)) >> 32) + ((a >> 32) * (b >> 32));
#endif
    }
}

struct uint128_t {
    uint64_t low = 0;
    uint64_t high = 0;

    constexpr uint128_t() = default;
    constexpr uint128_t(uint64_t l, uint64_t h) : low(l), high(h) {}
    constexpr explicit uint128_t(uint64_t v) : low(v), high(0) {}

    constexpr uint128_t operator+(const uint128_t& rhs) const {
        uint64_t new_low;
        uint64_t new_high;
        if (std::is_constant_evaluated()) {
            new_low = low + rhs.low;
            new_high = high + rhs.high + (new_low < low ? 1 : 0);
        } else {
#if defined(_MSC_VER)
            unsigned char carry = _addcarry_u64(0, low, rhs.low, &new_low);
            _addcarry_u64(carry, high, rhs.high, &new_high);
#elif defined(__GNUC__) && defined(__x86_64__)
            __uint128_t self = (static_cast<__uint128_t>(high) << 64) | low;
            __uint128_t other = (static_cast<__uint128_t>(rhs.high) << 64) | rhs.low;
            __uint128_t sum = self + other;
            new_low = static_cast<uint64_t>(sum);
            new_high = static_cast<uint64_t>(sum >> 64);
#else
            new_low = low + rhs.low;
            new_high = high + rhs.high + (new_low < low ? 1 : 0);
#endif
        }
        return {new_low, new_high};
    }

    constexpr uint128_t operator-(const uint128_t& rhs) const {
        uint64_t new_low;
        uint64_t new_high;
        if (std::is_constant_evaluated()) {
            new_low = low - rhs.low;
            new_high = high - rhs.high - (low < rhs.low ? 1 : 0);
        } else {
#if defined(_MSC_VER)
            unsigned char borrow = _subborrow_u64(0, low, rhs.low, &new_low);
            _subborrow_u64(borrow, high, rhs.high, &new_high);
#elif defined(__GNUC__) && defined(__x86_64__)
            __uint128_t self = (static_cast<__uint128_t>(high) << 64) | low;
            __uint128_t other = (static_cast<__uint128_t>(rhs.high) << 64) | rhs.low;
            __uint128_t diff = self - other;
            new_low = static_cast<uint64_t>(diff);
            new_high = static_cast<uint64_t>(diff >> 64);
#else
            new_low = low - rhs.low;
            new_high = high - rhs.high - (low < rhs.low ? 1 : 0);
#endif
        }
        return {new_low, new_high};
    }

    constexpr uint128_t& operator-=(const uint128_t& rhs) { *this = *this - rhs; return *this; }

    constexpr uint128_t operator*(const uint128_t& rhs) const {
        uint64_t new_low;
        uint64_t new_high;
        if (std::is_constant_evaluated()) {
            new_low = low * rhs.low;
            uint64_t lh = low * rhs.high;
            uint64_t hl = high * rhs.low;
            uint64_t carry_ll = mul_hi(low, rhs.low);
            uint64_t mid = lh + hl + carry_ll;
            new_high = high * rhs.high + (mid < lh ? 1 : 0) + mul_hi(low, rhs.high) + mul_hi(high, rhs.low);
        } else {
#if defined(_MSC_VER)
            uint64_t ll_high;
            new_low = _umul128(low, rhs.low, &ll_high);
            uint64_t lh_high;
            uint64_t lh_low = _umul128(low, rhs.high, &lh_high);
            uint64_t hl_high;
            uint64_t hl_low = _umul128(high, rhs.low, &hl_high);
            uint64_t hh_low = _umul128(high, rhs.high, nullptr);
            uint64_t mid = lh_low + hl_low + ll_high;
            new_high = hh_low + lh_high + hl_high + (mid < lh_low ? 1 : 0);
#elif defined(__GNUC__) && defined(__x86_64__)
            __uint128_t self = (static_cast<__uint128_t>(high) << 64) | low;
            __uint128_t other = (static_cast<__uint128_t>(rhs.high) << 64) | rhs.low;
            __uint128_t prod = self * other;
            new_low = static_cast<uint64_t>(prod);
            new_high = static_cast<uint64_t>(prod >> 64);
#else
            new_low = low * rhs.low;
            uint64_t lh = low * rhs.high;
            uint64_t hl = high * rhs.low;
            uint64_t carry_ll = mul_hi(low, rhs.low);
            uint64_t mid = lh + hl + carry_ll;
            new_high = high * rhs.high + (mid < lh ? 1 : 0) + mul_hi(low, rhs.high) + mul_hi(high, rhs.low);
#endif
        }
        return {new_low, new_high};
    }

    constexpr uint128_t& operator*=(const uint128_t& rhs) { *this = *this * rhs; return *this; }
    constexpr uint128_t operator<<(int n) const { uint128_t res = *this; res <<= n; return res; }
    constexpr uint128_t& operator<<=(int n) {
        if (n <= 0 || n >= 128) { low = 0; high = 0; return *this; }
        if (n < 64) { high = (high << n) | (low >> (64 - n)); low = low << n; } else { high = low << (n - 64); low = 0; }
        return *this;
    }

    constexpr uint128_t operator>>(int n) const { uint128_t res = *this; res >>= n; return res; }
    constexpr uint128_t& operator>>=(int n) {
        if (n <= 0 || n >= 128) { low = 0; high = 0; return *this; }
        if (n < 64) { low = (low >> n) | (high << (64 - n)); high = high >> n; } else { low = high >> (n - 64); high = 0; }
        return *this;
    }

    constexpr uint128_t operator|(const uint128_t& rhs) const { return {low | rhs.low, high | rhs.high}; }
    constexpr uint128_t& operator|=(const uint128_t& rhs) { low |= rhs.low; high |= rhs.high; return *this; }
    constexpr bool operator>(const uint128_t& rhs) const { return (high > rhs.high) || (high == rhs.high && low > rhs.low); }
    constexpr bool operator>=(const uint128_t& rhs) const { return (high > rhs.high) || (high == rhs.high && low >= rhs.low); }
    constexpr uint128_t operator/(uint128_t rhs) const {
        if (rhs == uint128_t(0)) return *this;
        uint128_t quot(0);
        uint128_t rem = *this;
        int shift = (bit_width64(high) - bit_width64(rhs.high)) + 64;
        if (bit_width64(rhs.high) == 0) shift = bit_width64(rem.high) + 64 - bit_width64(rhs.low);
        if (shift < 0) shift = 0;
        rhs <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (rem >= rhs) { rem -= rhs; quot |= uint128_t(1) << i; }
            rhs >>= 1;
        }
        return quot;
    }

    constexpr uint128_t& operator/=(const uint128_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint128_t operator%(const uint128_t& rhs) const { if (rhs == uint128_t(0)) return *this; return *this - (*this / rhs) * rhs; }
    constexpr bool operator==(const uint128_t& rhs) const { return low == rhs.low && high == rhs.high; }
    friend std::ostream& operator<<(std::ostream& os, const uint128_t& v) {
        if (v == uint128_t(0)) return os << "0";
        uint128_t temp = v;
        std::string s;
        while (temp > uint128_t(0)) {
            s.push_back('0' + static_cast<char>((temp % uint128_t(10)).low));
            temp /= uint128_t(10);
        }
        for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
        return os << s;
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
        uint64_t rem = static_cast<uint64_t>((temp % uint128_t(10)).low);
        s.push_back('0' + static_cast<unsigned>(rem));
        temp /= uint128_t(10);
    }
#endif
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
}

struct uint256_t {
    std::array<uint64_t, 4> words{{0,0,0,0}};

    constexpr uint256_t() = default;
    constexpr explicit uint256_t(uint64_t v) : words{{v,0,0,0}} {}
    constexpr uint256_t(uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3) : words{{w0,w1,w2,w3}} {}
    constexpr uint256_t(const uint128_t& low128, const uint128_t& high128) { words[0] = low128.low; words[1] = low128.high; words[2] = high128.low; words[3] = high128.high; }
    constexpr bool is_zero() const { return words[0]==0 && words[1]==0 && words[2]==0 && words[3]==0; }
    constexpr bool operator==(const uint256_t& rhs) const { return words == rhs.words; }
    constexpr bool operator!=(const uint256_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint256_t& rhs) const {
        for (int i = 3; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint256_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint256_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint256_t& rhs) const { return !(*this > rhs); }
    constexpr uint256_t operator+(const uint256_t& rhs) const {
        uint256_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 4; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<uint64_t>(carry);
            res.words[i] = static_cast<uint64_t>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            uint64_t tmp = words[i] + rhs.words[i];
            uint64_t c = (tmp < words[i]) ? 1 : 0;
            uint64_t tmp2 = tmp + static_cast<uint64_t>(carry);
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
            uint64_t tmp = words[i] - rhs.words[i] - static_cast<uint64_t>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint256_t& operator-=(const uint256_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint256_t operator<<(int n) const { uint256_t res = *this; res <<= n; return res; }
    constexpr uint256_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 256) { words = {0,0,0,0}; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t,4> nw{{0,0,0,0}};
        for (int i = 3; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 3) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 3 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint256_t operator>>(int n) const { uint256_t res = *this; res >>= n; return res; }
    constexpr uint256_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 256) { words = {0,0,0,0}; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t,4> nw{{0,0,0,0}};
        for (int i = 0; i < 4; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint256_t operator|(const uint256_t& rhs) const {
        return {words[0] | rhs.words[0], words[1] | rhs.words[1], words[2] | rhs.words[2], words[3] | rhs.words[3]};
    }

    constexpr uint256_t& operator|=(const uint256_t& rhs) { words[0] |= rhs.words[0]; words[1] |= rhs.words[1]; words[2] |= rhs.words[2]; words[3] |= rhs.words[3]; return *this; }
    constexpr uint256_t operator*(const uint256_t& rhs) const {
        uint64_t res_words[4] = {0,0,0,0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[8] = {0,0,0,0,0,0,0,0};
        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 4; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<uint64_t>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[8] = {0,0,0,0,0,0,0,0};
        for (int i = 0; i < 4; ++i) for (int j = 0; j < 4; ++j) {
            uint64_t lo = words[i] * rhs.words[j];
            uint64_t hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 4; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<uint64_t>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        return {res_words[0], res_words[1], res_words[2], res_words[3]};
    }

    constexpr uint256_t& operator*=(const uint256_t& rhs) { *this = *this * rhs; return *this; }

    int bit_width() const {
        for (int i = 3; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint256_t operator/(uint256_t divisor) const {
        if (divisor.is_zero()) return uint256_t(0);
        uint256_t dividend = *this;
        uint256_t quotient(0);
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint256_t(0);
        if (m > n) return uint256_t(0);
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (dividend >= divisor) {
                dividend -= divisor;
                uint256_t one;
                if (i < 64) one.words[0] = (uint64_t)1 << i;
                else if (i < 128) one.words[1] = (uint64_t)1 << (i - 64);
                else if (i < 192) one.words[2] = (uint64_t)1 << (i - 128);
                else one.words[3] = (uint64_t)1 << (i - 192);
                quotient |= one;
            }
            divisor >>= 1;
        }
        return quotient;
    }

    constexpr uint256_t& operator/=(const uint256_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint256_t operator%(const uint256_t& rhs) const {
        if (rhs.is_zero()) return uint256_t(0);
        uint256_t dividend = *this;
        uint256_t divisor = rhs;
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint256_t(0);
        if (m > n) return dividend;
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) if (dividend >= divisor) dividend -= divisor, divisor >>= 1; else divisor >>= 1;
        return dividend;
    }

#if defined(__SIZEOF_INT128__)
    uint64_t div_mod_uint64(uint64_t m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 3; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<uint64_t>(cur / m);
            rem = cur % m;
        }
        return static_cast<uint64_t>(rem);
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

    constexpr uint128_t low128() const { return {words[0], words[1]}; }
    constexpr uint128_t high128() const { return {words[2], words[3]}; }
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
    std::array<uint64_t, 8> words{{0,0,0,0,0,0,0,0}};

    constexpr uint512_t() = default;
    constexpr explicit uint512_t(uint64_t v) : words{{v,0,0,0,0,0,0,0}} {}
    constexpr uint512_t(uint64_t w0, uint64_t w1, uint64_t w2, uint64_t w3, uint64_t w4, uint64_t w5, uint64_t w6, uint64_t w7) : words{{w0,w1,w2,w3,w4,w5,w6,w7}} {}
    constexpr uint512_t(const uint256_t& low256, const uint256_t& high256) {
        for (size_t i = 0; i < 4; ++i) { words[i] = low256.words[i]; words[i+4] = high256.words[i]; }
    }

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 8; ++i) if (words[i] != 0) return false;
        return true;
    }

    constexpr bool operator==(const uint512_t& rhs) const { return words == rhs.words; }
    constexpr bool operator!=(const uint512_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint512_t& rhs) const {
        for (int i = 7; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint512_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint512_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint512_t& rhs) const { return !(*this > rhs); }
    constexpr uint512_t operator+(const uint512_t& rhs) const {
        uint512_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 8; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<uint64_t>(carry);
            res.words[i] = static_cast<uint64_t>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            uint64_t tmp = words[i] + rhs.words[i];
            uint64_t c = (tmp < words[i]) ? 1 : 0;
            uint64_t tmp2 = tmp + static_cast<uint64_t>(carry);
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
            uint64_t tmp = words[i] - rhs.words[i] - static_cast<uint64_t>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint512_t& operator-=(const uint512_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint512_t operator<<(int n) const { uint512_t res = *this; res <<= n; return res; }
    constexpr uint512_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 512) { words = {0,0,0,0,0,0,0,0}; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t,8> nw{{0,0,0,0,0,0,0,0}};
        for (int i = 7; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 7) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 7 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint512_t operator>>(int n) const { uint512_t res = *this; res >>= n; return res; }
    constexpr uint512_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 512) { words = {0,0,0,0,0,0,0,0}; return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t,8> nw{{0,0,0,0,0,0,0,0}};
        for (int i = 0; i < 8; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        words = nw;
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

    constexpr uint512_t operator*(const uint512_t& rhs) const {
        uint64_t res_words[8] = {0,0,0,0,0,0,0,0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[16] = {0};
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 8; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<uint64_t>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[16] = {0};
        for (int i = 0; i < 8; ++i) for (int j = 0; j < 8; ++j) {
            uint64_t lo = words[i] * rhs.words[j];
            uint64_t hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 8; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<uint64_t>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        return {res_words[0], res_words[1], res_words[2], res_words[3], res_words[4], res_words[5], res_words[6], res_words[7]};
    }

    constexpr uint512_t& operator*=(const uint512_t& rhs) { *this = *this * rhs; return *this; }

    int bit_width() const {
        for (int i = 7; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint512_t operator/(uint512_t divisor) const {
        if (divisor.is_zero()) return uint512_t(0);
        uint512_t dividend = *this;
        uint512_t quotient(0);
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint512_t(0);
        if (m > n) return uint512_t(0);
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (dividend >= divisor) {
                dividend -= divisor;
                uint512_t one;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                one.words[word_idx] = (uint64_t)1 << bit_idx;
                quotient |= one;
            }
            divisor >>= 1;
        }
        return quotient;
    }

    constexpr uint512_t& operator/=(const uint512_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint512_t operator%(const uint512_t& rhs) const {
        if (rhs.is_zero()) return uint512_t(0);
        uint512_t dividend = *this;
        uint512_t divisor = rhs;
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint512_t(0);
        if (m > n) return dividend;
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) if (dividend >= divisor) dividend -= divisor, divisor >>= 1; else divisor >>= 1;
        return dividend;
    }

#if defined(__SIZEOF_INT128__)
    uint64_t div_mod_uint64(uint64_t m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 7; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<uint64_t>(cur / m);
            rem = cur % m;
        }
        return static_cast<uint64_t>(rem);
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
    std::array<uint64_t, 16> words{{0}};

    constexpr uint1024_t() = default;
    constexpr explicit uint1024_t(uint64_t v) : words{{v}} {}
    constexpr uint1024_t(const uint512_t& low512, const uint512_t& high512) {
        for (size_t i = 0; i < 8; ++i) { words[i] = low512.words[i]; words[i+8] = high512.words[i]; }
    }

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 16; ++i) if (words[i] != 0) return false;
        return true;
    }

    constexpr bool operator==(const uint1024_t& rhs) const { return words == rhs.words; }
    constexpr bool operator!=(const uint1024_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint1024_t& rhs) const {
        for (int i = 15; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint1024_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint1024_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint1024_t& rhs) const { return !(*this > rhs); }
    constexpr uint1024_t operator+(const uint1024_t& rhs) const {
        uint1024_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 16; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<uint64_t>(carry);
            res.words[i] = static_cast<uint64_t>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            uint64_t tmp = words[i] + rhs.words[i];
            uint64_t c = (tmp < words[i]) ? 1 : 0;
            uint64_t tmp2 = tmp + static_cast<uint64_t>(carry);
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
            uint64_t tmp = words[i] - rhs.words[i] - static_cast<uint64_t>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint1024_t& operator-=(const uint1024_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint1024_t operator<<(int n) const { uint1024_t res = *this; res <<= n; return res; }
    constexpr uint1024_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 1024) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 16> nw{{0}};
        for (int i = 15; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 15) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 15 && bit_shift != 0) nw[ni+1] |= part >> (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint1024_t operator>>(int n) const { uint1024_t res = *this; res >>= n; return res; }
    constexpr uint1024_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 1024) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 16> nw{{0}};
        for (int i = 0; i < 16; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni-1] |= part << (64 - bit_shift);
        }
        words = nw;
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

    constexpr uint1024_t operator*(const uint1024_t& rhs) const {
        uint64_t res_words[16] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[32] = {0};
        for (int i = 0; i < 16; ++i) for (int j = 0; j < 16; ++j) tmp[i+j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 16; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<uint64_t>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[32] = {0};
        for (int i = 0; i < 16; ++i) for (int j = 0; j < 16; ++j) {
            uint64_t lo = words[i] * rhs.words[j];
            uint64_t hi = mul_hi(words[i], rhs.words[j]);
            acc[i+j] += lo;
            acc[i+j+1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 16; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<uint64_t>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint1024_t res;
        for (int i = 0; i < 16; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint1024_t& operator*=(const uint1024_t& rhs) { *this = *this * rhs; return *this; }

    int bit_width() const {
        for (int i = 15; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint1024_t operator/(uint1024_t divisor) const {
        if (divisor.is_zero()) return uint1024_t(0);
        uint1024_t dividend = *this;
        uint1024_t quotient(0);
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint1024_t(0);
        if (m > n) return uint1024_t(0);
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (dividend >= divisor) {
                dividend -= divisor;
                uint1024_t one;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                one.words[word_idx] = (uint64_t)1 << bit_idx;
                quotient |= one;
            }
            divisor >>= 1;
        }
        return quotient;
    }

    constexpr uint1024_t& operator/=(const uint1024_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint1024_t operator%(const uint1024_t& rhs) const {
        if (rhs.is_zero()) return uint1024_t(0);
        uint1024_t dividend = *this;
        uint1024_t divisor = rhs;
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint1024_t(0);
        if (m > n) return dividend;
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) if (dividend >= divisor) dividend -= divisor, divisor >>= 1; else divisor >>= 1;
        return dividend;
    }

#if defined(__SIZEOF_INT128__)
    uint64_t div_mod_uint64(uint64_t m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 15; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<uint64_t>(cur / m);
            rem = cur % m;
        }
        return static_cast<uint64_t>(rem);
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
    std::array<uint64_t, 32> words{{0}};

    constexpr uint2048_t() = default;
    constexpr explicit uint2048_t(uint64_t v) : words{{v}} {}
    constexpr uint2048_t(const uint512_t& low512, const uint512_t& high512) {
        for (size_t i = 0; i < 8; ++i) { words[i] = low512.words[i]; words[i + 8] = high512.words[i]; }
    }

    constexpr bool is_zero() const {
        for (size_t i = 0; i < 32; ++i) if (words[i] != 0) return false;
        return true;
    }

    constexpr bool operator==(const uint2048_t& rhs) const { return words == rhs.words; }
    constexpr bool operator!=(const uint2048_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint2048_t& rhs) const {
        for (int i = 31; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint2048_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint2048_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint2048_t& rhs) const { return !(*this > rhs); }
    constexpr uint2048_t operator+(const uint2048_t& rhs) const {
        uint2048_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 32; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<uint64_t>(carry);
            res.words[i] = static_cast<uint64_t>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            uint64_t tmp = words[i] + rhs.words[i];
            uint64_t c = (tmp < words[i]) ? 1 : 0;
            uint64_t tmp2 = tmp + static_cast<uint64_t>(carry);
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
            uint64_t tmp = words[i] - rhs.words[i] - static_cast<uint64_t>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint2048_t& operator-=(const uint2048_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint2048_t operator<<(int n) const { uint2048_t res = *this; res <<= n; return res; }
    constexpr uint2048_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 2048) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 32> nw{{0}};
        for (int i = 31; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 31) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 31 && bit_shift != 0) nw[ni + 1] |= part >> (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint2048_t operator>>(int n) const { uint2048_t res = *this; res >>= n; return res; }
    constexpr uint2048_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 2048) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 32> nw{{0}};
        for (int i = 0; i < 32; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni - 1] |= part << (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint2048_t operator|(const uint2048_t& rhs) const {
        uint2048_t r;
        for (size_t i = 0; i < 32; ++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint2048_t& operator|=(const uint2048_t& rhs) {
        for (size_t i = 0; i < 32; ++i) words[i] |= rhs.words[i];
        return *this;
    }

    constexpr uint2048_t operator*(const uint2048_t& rhs) const {
        uint64_t res_words[32] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[64] = {0};
        for (int i = 0; i < 32; ++i) for (int j = 0; j < 32; ++j) tmp[i + j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 32; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<uint64_t>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[64] = {0};
        for (int i = 0; i < 32; ++i) for (int j = 0; j < 32; ++j) {
            uint64_t lo = words[i] * rhs.words[j];
            uint64_t hi = mul_hi(words[i], rhs.words[j]);
            acc[i + j] += lo;
            acc[i + j + 1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 32; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<uint64_t>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint2048_t res;
        for (int i = 0; i < 32; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint2048_t& operator*=(const uint2048_t& rhs) { *this = *this * rhs; return *this; }

    int bit_width() const {
        for (int i = 31; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint2048_t operator/(uint2048_t divisor) const {
        if (divisor.is_zero()) return uint2048_t(0);
        uint2048_t dividend = *this;
        uint2048_t quotient(0);
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint2048_t(0);
        if (m > n) return uint2048_t(0);
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (dividend >= divisor) {
                dividend -= divisor;
                uint2048_t one;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                one.words[word_idx] = (uint64_t)1 << bit_idx;
                quotient |= one;
            }
            divisor >>= 1;
        }
        return quotient;
    }

    constexpr uint2048_t& operator/=(const uint2048_t& rhs) { *this = *this / rhs; return *this; }

    constexpr uint2048_t operator%(const uint2048_t& rhs) const {
        if (rhs.is_zero()) return uint2048_t(0);
        uint2048_t dividend = *this;
        uint2048_t divisor = rhs;
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint2048_t(0);
        if (m > n) return dividend;
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) if (dividend >= divisor) dividend -= divisor, divisor >>= 1; else divisor >>= 1;
        return dividend;
    }

#if defined(__SIZEOF_INT128__)
    uint64_t div_mod_uint64(uint64_t m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 31; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<uint64_t>(cur / m);
            rem = cur % m;
        }
        return static_cast<uint64_t>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint2048_t& v) {
        if (v.is_zero()) return os << "0";
        std::string out;
        out.reserve(32 * 16);
        bool leading = true;
        for (int i = 31; i >= 0; --i) {
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
    std::string out;
    out.reserve(32 * 16);
    bool leading = true;
    for (int i = 31; i >= 0; --i) {
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
    return out.empty() ? "0" : out;
#endif
}

struct uint4096_t {
    std::array<uint64_t, 64> words{{0}};

    constexpr uint4096_t() = default;
    constexpr explicit uint4096_t(uint64_t v) : words{{v}} {}
    constexpr bool is_zero() const {
        for (size_t i = 0; i < 64; ++i) if (words[i] != 0) return false;
        return true;
    }

    constexpr bool operator==(const uint4096_t& rhs) const { return words == rhs.words; }
    constexpr bool operator!=(const uint4096_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint4096_t& rhs) const {
        for (int i = 63; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint4096_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint4096_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint4096_t& rhs) const { return !(*this > rhs); }
    constexpr uint4096_t operator+(const uint4096_t& rhs) const {
        uint4096_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 64; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<uint64_t>(carry);
            res.words[i] = static_cast<uint64_t>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            uint64_t tmp = words[i] + rhs.words[i];
            uint64_t c = (tmp < words[i]) ? 1 : 0;
            uint64_t tmp2 = tmp + static_cast<uint64_t>(carry);
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
            uint64_t tmp = words[i] - rhs.words[i] - static_cast<uint64_t>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint4096_t& operator-=(const uint4096_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint4096_t operator<<(int n) const { uint4096_t res = *this; res <<= n; return res; }
    constexpr uint4096_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 4096) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 64> nw{{0}};
        for (int i = 63; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 63) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 63 && bit_shift != 0) nw[ni + 1] |= part >> (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint4096_t operator>>(int n) const { uint4096_t res = *this; res >>= n; return res; }
    constexpr uint4096_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 4096) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 64> nw{{0}};
        for (int i = 0; i < 64; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni - 1] |= part << (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint4096_t operator|(const uint4096_t& rhs) const {
        uint4096_t r;
        for (size_t i = 0; i < 64; ++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint4096_t& operator|=(const uint4096_t& rhs) {
        for (size_t i = 0; i < 64; ++i) words[i] |= rhs.words[i];
        return *this;
    }

    constexpr uint4096_t operator*(const uint4096_t& rhs) const {
        uint64_t res_words[64] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[128] = {0};
        for (int i = 0; i < 64; ++i) for (int j = 0; j < 64; ++j) tmp[i + j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 64; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<uint64_t>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[128] = {0};
        for (int i = 0; i < 64; ++i) for (int j = 0; j < 64; ++j) {
            uint64_t lo = words[i] * rhs.words[j];
            uint64_t hi = mul_hi(words[i], rhs.words[j]);
            acc[i + j] += lo;
            acc[i + j + 1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 64; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<uint64_t>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint4096_t res;
        for (int i = 0; i < 64; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint4096_t& operator*=(const uint4096_t& rhs) { *this = *this * rhs; return *this; }

    int bit_width() const {
        for (int i = 63; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint4096_t operator/(uint4096_t divisor) const {
        if (divisor.is_zero()) return uint4096_t(0);
        uint4096_t dividend = *this;
        uint4096_t quotient(0);
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint4096_t(0);
        if (m > n) return uint4096_t(0);
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (dividend >= divisor) {
                dividend -= divisor;
                uint4096_t one;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                one.words[word_idx] = (uint64_t)1 << bit_idx;
                quotient |= one;
            }
            divisor >>= 1;
        }
        return quotient;
    }

    constexpr uint4096_t& operator/=(const uint4096_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint4096_t operator%(const uint4096_t& rhs) const {
        if (rhs.is_zero()) return uint4096_t(0);
        uint4096_t dividend = *this;
        uint4096_t divisor = rhs;
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint4096_t(0);
        if (m > n) return dividend;
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) if (dividend >= divisor) dividend -= divisor, divisor >>= 1; else divisor >>= 1;
        return dividend;
    }

#if defined(__SIZEOF_INT128__)
    uint64_t div_mod_uint64(uint64_t m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 63; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<uint64_t>(cur / m);
            rem = cur % m;
        }
        return static_cast<uint64_t>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint4096_t& v) {
        if (v.is_zero()) return os << "0";
        std::string out;
        out.reserve(64 * 16);
        bool leading = true;
        for (int i = 63; i >= 0; --i) {
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
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; i++, j--) { char tmp = s[i]; s[i] = s[j]; s[j] = tmp; }
    return s;
#else
    std::string out;
    out.reserve(64 * 16);
    bool leading = true;
    for (int i = 63; i >= 0; --i) {
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
    return out.empty() ? "0" : out;
#endif
}

struct uint8192_t {
    std::array<uint64_t, 128> words{{0}};

    constexpr uint8192_t() = default;
    constexpr explicit uint8192_t(uint64_t v) : words{{v}} {}
    constexpr bool is_zero() const {
        for (size_t i = 0; i < 128; ++i) if (words[i] != 0) return false;
        return true;
    }

    constexpr bool operator==(const uint8192_t& rhs) const { return words == rhs.words; }
    constexpr bool operator!=(const uint8192_t& rhs) const { return !(*this == rhs); }
    constexpr bool operator>(const uint8192_t& rhs) const {
        for (int i = 127; i >= 0; --i) { if (words[i] > rhs.words[i]) return true; if (words[i] < rhs.words[i]) return false; }
        return false;
    }

    constexpr bool operator>=(const uint8192_t& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const uint8192_t& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const uint8192_t& rhs) const { return !(*this > rhs); }
    constexpr uint8192_t operator+(const uint8192_t& rhs) const {
        uint8192_t res;
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < 128; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + static_cast<uint64_t>(carry);
            res.words[i] = static_cast<uint64_t>(sum);
            carry = static_cast<unsigned __int128>(sum >> 64);
#else
            uint64_t tmp = words[i] + rhs.words[i];
            uint64_t c = (tmp < words[i]) ? 1 : 0;
            uint64_t tmp2 = tmp + static_cast<uint64_t>(carry);
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
            uint64_t tmp = words[i] - rhs.words[i] - static_cast<uint64_t>(borrow);
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr uint8192_t& operator-=(const uint8192_t& rhs) { *this = *this - rhs; return *this; }
    constexpr uint8192_t operator<<(int n) const { uint8192_t res = *this; res <<= n; return res; }
    constexpr uint8192_t& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= 8192) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 128> nw{{0}};
        for (int i = 127; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni > 127) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 <= 127 && bit_shift != 0) nw[ni + 1] |= part >> (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint8192_t operator>>(int n) const { uint8192_t res = *this; res >>= n; return res; }
    constexpr uint8192_t& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= 8192) { words.fill(0); return *this; }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        std::array<uint64_t, 128> nw{{0}};
        for (int i = 0; i < 128; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            uint64_t part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni - 1] |= part << (64 - bit_shift);
        }
        words = nw;
        return *this;
    }

    constexpr uint8192_t operator|(const uint8192_t& rhs) const {
        uint8192_t r;
        for (size_t i = 0; i < 128; ++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr uint8192_t& operator|=(const uint8192_t& rhs) {
        for (size_t i = 0; i < 128; ++i) words[i] |= rhs.words[i];
        return *this;
    }

    constexpr uint8192_t operator*(const uint8192_t& rhs) const {
        uint64_t res_words[128] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[256] = {0};
        for (int i = 0; i < 128; ++i) for (int j = 0; j < 128; ++j) tmp[i + j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
        unsigned __int128 carry = 0;
        for (int i = 0; i < 128; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<uint64_t>(tmp[i]);
            carry = static_cast<unsigned __int128>(tmp[i] >> 64);
        }
#else
        unsigned __int128 acc[256] = {0};
        for (int i = 0; i < 128; ++i) for (int j = 0; j < 128; ++j) {
            uint64_t lo = words[i] * rhs.words[j];
            uint64_t hi = mul_hi(words[i], rhs.words[j]);
            acc[i + j] += lo;
            acc[i + j + 1] += hi;
        }
        unsigned __int128 carry = 0;
        for (int i = 0; i < 128; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<uint64_t>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        uint8192_t res;
        for (int i = 0; i < 128; ++i) res.words[i] = res_words[i];
        return res;
    }

    constexpr uint8192_t& operator*=(const uint8192_t& rhs) { *this = *this * rhs; return *this; }

    int bit_width() const {
        for (int i = 127; i >= 0; --i) if (words[i] != 0) return i*64 + bit_width64(words[i]);
        return 0;
    }

    constexpr uint8192_t operator/(uint8192_t divisor) const {
        if (divisor.is_zero()) return uint8192_t(0);
        uint8192_t dividend = *this;
        uint8192_t quotient(0);
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint8192_t(0);
        if (m > n) return uint8192_t(0);
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (dividend >= divisor) {
                dividend -= divisor;
                uint8192_t one;
                int word_idx = i / 64;
                int bit_idx = i % 64;
                one.words[word_idx] = (uint64_t)1 << bit_idx;
                quotient |= one;
            }
            divisor >>= 1;
        }
        return quotient;
    }

    constexpr uint8192_t& operator/=(const uint8192_t& rhs) { *this = *this / rhs; return *this; }
    constexpr uint8192_t operator%(const uint8192_t& rhs) const {
        if (rhs.is_zero()) return uint8192_t(0);
        uint8192_t dividend = *this;
        uint8192_t divisor = rhs;
        int n = dividend.bit_width();
        int m = divisor.bit_width();
        if (n == 0) return uint8192_t(0);
        if (m > n) return dividend;
        int shift = n - m;
        divisor <<= shift;
        for (int i = shift; i >= 0; --i) if (dividend >= divisor) dividend -= divisor, divisor >>= 1; else divisor >>= 1;
        return dividend;
    }

#if defined(__SIZEOF_INT128__)
    uint64_t div_mod_uint64(uint64_t m) {
        if (m == 0) return 0;
        __uint128_t rem = 0;
        for (int i = 127; i >= 0; --i) {
            __uint128_t cur = (rem << 64) | words[i];
            words[i] = static_cast<uint64_t>(cur / m);
            rem = cur % m;
        }
        return static_cast<uint64_t>(rem);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const uint8192_t& v) {
        if (v.is_zero()) return os << "0";
        std::string out;
        out.reserve(128 * 16);
        bool leading = true;
        for (int i = 127; i >= 0; --i) {
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
    std::string out;
    out.reserve(128 * 16);
    bool leading = true;
    for (int i = 127; i >= 0; --i) {
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
    return out.empty() ? "0" : out;
#endif
}
}