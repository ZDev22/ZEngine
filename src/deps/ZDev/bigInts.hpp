#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <bit>
#include <concepts>
#include <ostream>
#include <string>
#include <algorithm>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace bigInts {

constexpr uint64_t mul_hi(uint64_t a, uint64_t b) {
    if (std::is_constant_evaluated()) {
        uint64_t aL = a & 0xFFFFFFFF;
        uint64_t aH = a >> 32;
        uint64_t bL = b & 0xFFFFFFFF;
        uint64_t bH = b >> 32;
        uint64_t ll = aL * bL;
        uint64_t hl = aH * bL;
        uint64_t lh = aL * bH;
        uint64_t hh = aH * bH;
        uint64_t mid = (ll >> 32) + (hl & 0xFFFFFFFF) + (lh & 0xFFFFFFFF);
        return (mid >> 32) + (hl >> 32) + (lh >> 32) + hh;
    } else {
#if defined(_MSC_VER)
        return __umulh(a, b);
#elif defined(__GNUC__) && defined(__x86_64__)
        return static_cast<uint64_t>((static_cast<__uint128_t>(a) * b) >> 64);
#else
        uint64_t aL = a & 0xFFFFFFFF;
        uint64_t aH = a >> 32;
        uint64_t bL = b & 0xFFFFFFFF;
        uint64_t bH = b >> 32;
        uint64_t ll = aL * bL;
        uint64_t hl = aH * bL;
        uint64_t lh = aL * bH;
        uint64_t hh = aH * bH;
        uint64_t mid = (ll >> 32) + (hl & 0xFFFFFFFF) + (lh & 0xFFFFFFFF);
        return (mid >> 32) + (hl >> 32) + (lh >> 32) + hh;
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
            uint64_t carry = (new_low < low) ? 1 : 0;
            new_high = high + rhs.high + carry;
        } else {
#if defined(_MSC_VER)
            unsigned char carry = _addcarry_u64(0, low, rhs.low, &new_low);
            _addcarry_u64(carry, high, rhs.high, &new_high);
#elif defined(__GNUC__) && defined(__x86_64__)
            __uint128_t self = static_cast<__uint128_t>(high) << 64 | low;
            __uint128_t other = static_cast<__uint128_t>(rhs.high) << 64 | rhs.low;
            __uint128_t sum = self + other;
            new_low = static_cast<uint64_t>(sum);
            new_high = static_cast<uint64_t>(sum >> 64);
#else
            new_low = low + rhs.low;
            uint64_t carry = (new_low < low) ? 1 : 0;
            new_high = high + rhs.high + carry;
#endif
        }
        return {new_low, new_high};
    }

    constexpr uint128_t operator-(const uint128_t& rhs) const {
        uint64_t new_low;
        uint64_t new_high;
        if (std::is_constant_evaluated()) {
            new_low = low - rhs.low;
            uint64_t borrow = (low < rhs.low) ? 1 : 0;
            new_high = high - rhs.high - borrow;
        } else {
#if defined(_MSC_VER)
            unsigned char borrow = _subborrow_u64(0, low, rhs.low, &new_low);
            _subborrow_u64(borrow, high, rhs.high, &new_high);
#elif defined(__GNUC__) && defined(__x86_64__)
            __uint128_t self = static_cast<__uint128_t>(high) << 64 | low;
            __uint128_t other = static_cast<__uint128_t>(rhs.high) << 64 | rhs.low;
            __uint128_t diff = self - other;
            new_low = static_cast<uint64_t>(diff);
            new_high = static_cast<uint64_t>(diff >> 64);
#else
            new_low = low - rhs.low;
            uint64_t borrow = (low < rhs.low) ? 1 : 0;
            new_high = high - rhs.high - borrow;
#endif
        }
        return {new_low, new_high};
    }

    constexpr uint128_t& operator-=(const uint128_t& rhs) {
        *this = *this - rhs;
        return *this;
    }

    constexpr uint128_t operator*(const uint128_t& rhs) const {
        uint64_t new_low;
        uint64_t new_high;
        if (std::is_constant_evaluated()) {
            uint64_t ll = low * rhs.low;
            uint64_t lh = low * rhs.high;
            uint64_t hl = high * rhs.low;
            uint64_t hh = high * rhs.high;
            uint64_t carry_ll = mul_hi(low, rhs.low);
            uint64_t mid = lh + hl + carry_ll;
            uint64_t carry_mid = (mid < lh) ? 1 : 0;
            new_high = hh + carry_mid + mul_hi(low, rhs.high) + mul_hi(high, rhs.low);
            new_low = ll;
        } else {
#if defined(_MSC_VER)
            uint64_t ll_high;
            new_low = _umul128(low, rhs.low, &ll_high);
            uint64_t lh_high;
            uint64_t lh_low = _umul128(low, rhs.high, &lh_high);
            uint64_t hl_high;
            uint64_t hl_low = _umul128(high, rhs.low, &hl_high);
            uint64_t hh_high;
            uint64_t hh_low = _umul128(high, rhs.high, &hh_high);
            uint64_t mid = lh_low + hl_low + ll_high;
            uint64_t carry_mid = (mid < lh_low) ? 1 : 0;
            new_high = hh_low + lh_high + hl_high + carry_mid;
#elif defined(__GNUC__) && defined(__x86_64__)
            __uint128_t self = static_cast<__uint128_t>(high) << 64 | low;
            __uint128_t other = static_cast<__uint128_t>(rhs.high) << 64 | rhs.low;
            __uint128_t prod = self * other;
            new_low = static_cast<uint64_t>(prod);
            new_high = static_cast<uint64_t>(prod >> 64);
#else
            uint64_t ll = low * rhs.low;
            uint64_t lh = low * rhs.high;
            uint64_t hl = high * rhs.low;
            uint64_t hh = high * rhs.high;
            uint64_t carry_ll = mul_hi(low, rhs.low);
            uint64_t mid = lh + hl + carry_ll;
            uint64_t carry_mid = (mid < lh) ? 1 : 0;
            new_high = hh + carry_mid + mul_hi(low, rhs.high) + mul_hi(high, rhs.low);
            new_low = ll;
#endif
        }
        return {new_low, new_high};
    }

    constexpr uint128_t& operator*=(const uint128_t& rhs) {
        *this = *this * rhs;
        return *this;
    }

    constexpr uint128_t operator<<(int n) const {
        uint128_t res = *this;
        res <<= n;
        return res;
    }

    constexpr uint128_t& operator<<=(int n) {
        if (n <= 0 || n >= 128) {
            low = 0;
            high = 0;
            return *this;
        }
        if (n < 64) {
            high = (high << n) | (low >> (64 - n));
            low = low << n;
        } else {
            high = low << (n - 64);
            low = 0;
        }
        return *this;
    }

    constexpr uint128_t operator>>(int n) const {
        uint128_t res = *this;
        res >>= n;
        return res;
    }

    constexpr uint128_t& operator>>=(int n) {
        if (n <= 0 || n >= 128) {
            low = 0;
            high = 0;
            return *this;
        }
        if (n < 64) {
            low = (low >> n) | (high << (64 - n));
            high = high >> n;
        } else {
            low = high >> (n - 64);
            high = 0;
        }
        return *this;
    }

    constexpr uint128_t operator|(const uint128_t& rhs) const {
        return {low | rhs.low, high | rhs.high};
    }

    constexpr uint128_t& operator|=(const uint128_t& rhs) {
        low |= rhs.low;
        high |= rhs.high;
        return *this;
    }

    constexpr bool operator>(const uint128_t& rhs) const {
        return (high > rhs.high) || (high == rhs.high && low > rhs.low);
    }

    constexpr bool operator>=(const uint128_t& rhs) const {
        return (high > rhs.high) || (high == rhs.high && low >= rhs.low);
    }

    constexpr uint128_t operator/(uint128_t rhs) const {
        if (rhs == uint128_t(0)) return *this;
        uint128_t quot(0);
        uint128_t rem = *this;
        int shift = std::bit_width(high) - std::bit_width(rhs.high) + 64;
        rhs <<= shift;
        for (int i = shift; i >= 0; --i) {
            if (rem >= rhs) {
                rem -= rhs;
                quot |= uint128_t(1) << i;
            }
            rhs >>= 1;
        }
        return quot;
    }

    constexpr uint128_t& operator/=(const uint128_t& rhs) {
        *this = *this / rhs;
        return *this;
    }

    constexpr uint128_t operator%(const uint128_t& rhs) const {
        if (rhs == uint128_t(0)) return *this;
        return *this - (*this / rhs) * rhs;
    }

    constexpr bool operator==(const uint128_t& rhs) const {
        return low == rhs.low && high == rhs.high;
    }

    friend std::ostream& operator<<(std::ostream& os, const uint128_t& v) {
        if (v == uint128_t(0)) return os << "0";
        uint128_t temp = v;
        std::string s;
        while (temp > uint128_t(0)) {
            s.push_back('0' + static_cast<char>((temp % uint128_t(10)).low));
            temp /= uint128_t(10);
        }
        std::reverse(s.begin(), s.end());
        return os << s;
    }
};

inline std::string toString(const uint128_t& v) {
    if (v == bigInts::uint128_t(0)) return "0";
    
    uint128_t temp = v;
    std::string s;

    while (temp > uint128_t(0)) {
        s.push_back('0' + static_cast<char>((temp % uint128_t(10)).low));
        temp /= uint128_t(10);
    }

    std::reverse(s.begin(), s.end());
    return s;
}
}