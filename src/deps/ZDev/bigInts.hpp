#pragma once

#include <string>
#include <ostream>
#include <cstring>
#include <algorithm>

template<unsigned long long bitCount>
    requires (bitCount % 64 == 0)
struct bigInt {
    static constexpr size_t bytes = bitCount / 64;
    unsigned long long words[bytes] = {0};

    constexpr bigInt() = default;
    constexpr explicit bigInt(unsigned long long v) {
        words[0] = v;
        for (size_t i = 1; i < bytes; ++i) words[i] = 0;
    }

    constexpr bigInt(const unsigned long long* initWords, size_t count) {
        size_t limit = std::min(count, bytes);
        for (size_t i = 0; i < limit; ++i) words[i] = initWords[i];
        for (size_t i = limit; i < bytes; ++i) words[i] = 0;
    }

    constexpr bool is_zero() const {
        for (size_t i = 0; i < bytes; ++i) if (words[i] != 0) return false;
        return true;
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
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < bytes; ++i) {
#if defined(__GNUC__) && defined(__x86_64__)
            __uint128_t sum = static_cast<__uint128_t>(words[i]) + rhs.words[i] + carry;
            res.words[i] = static_cast<unsigned long long>(sum);
            carry = sum >> 64;
#else
            unsigned long long tmp = words[i] + rhs.words[i];
            unsigned long long c = (tmp < words[i]) ? 1 : 0;
            unsigned long long tmp2 = tmp + carry;
            res.words[i] = tmp2;
            carry = c + (tmp2 < tmp ? 1 : 0);
#endif
        }
        return res;
    }

    constexpr bigInt& operator+=(const bigInt& rhs) { *this = *this + rhs; return *this; }

    constexpr bigInt operator-(const bigInt& rhs) const {
        bigInt res;
        unsigned __int128 borrow = 0;
        for (size_t i = 0; i < bytes; ++i) {
            unsigned long long tmp = words[i] - rhs.words[i] - borrow;
            borrow = (static_cast<unsigned __int128>(words[i]) < static_cast<unsigned __int128>(rhs.words[i]) + borrow) ? 1 : 0;
            res.words[i] = tmp;
        }
        return res;
    }

    constexpr bigInt& operator-=(const bigInt& rhs) { *this = *this - rhs; return *this; }

    constexpr bigInt operator*(const bigInt& rhs) const {
        unsigned long long res_words[bytes] = {0};
#if defined(__SIZEOF_INT128__)
        __uint128_t tmp[bytes * 2] = {0};
        for (size_t i = 0; i < bytes; ++i) {
            for (size_t j = 0; j < bytes; ++j) {
                if (i + j < bytes * 2) {
                    tmp[i + j] += static_cast<__uint128_t>(words[i]) * rhs.words[j];
                }
            }
        }
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < bytes; ++i) {
            tmp[i] += carry;
            res_words[i] = static_cast<unsigned long long>(tmp[i]);
            carry = tmp[i] >> 64;
        }
#else
        unsigned __int128 acc[bytes * 2] = {0};
        for (size_t i = 0; i < bytes; ++i) {
            for (size_t j = 0; j < bytes; ++j) {
                if (i + j < bytes * 2) {
                    unsigned long long lo = words[i] * rhs.words[j];
                    unsigned long long hi = mul_hi(words[i], rhs.words[j]);
                    acc[i + j] += lo;
                    if (i + j + 1 < bytes * 2) acc[i + j + 1] += hi;
                }
            }
        }
        unsigned __int128 carry = 0;
        for (size_t i = 0; i < bytes; ++i) {
            acc[i] += carry;
            res_words[i] = static_cast<unsigned long long>(acc[i]);
            carry = acc[i] >> 64;
        }
#endif
        return bigInt(res_words, bytes);
    }

    constexpr bigInt& operator*=(const bigInt& rhs) { *this = *this * rhs; return *this; }

    constexpr bigInt operator/(const bigInt& divisor) const {
        if (divisor.is_zero()) return bigInt(0);
        bigInt quotient(0);
        bigInt remainder(0);
        int bw = bit_width();
        if (bw == 0) return bigInt(0);
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

    constexpr bigInt& operator/=(const bigInt& rhs) { *this = *this / rhs; return *this; }

    constexpr bigInt operator%(const bigInt& rhs) const {
        if (rhs.is_zero()) return *this;
        bigInt remainder(0);
        int bw = bit_width();
        if (bw == 0) return bigInt(0);
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

    constexpr bool operator==(const bigInt& rhs) const {
        return std::equal(words, words + bytes, rhs.words);
    }

    constexpr bool operator==(unsigned char rhs) const { return words[0] == rhs && is_zero_from(1); }
    constexpr bool operator==(unsigned short rhs) const { return words[0] == rhs && is_zero_from(1); }
    constexpr bool operator==(unsigned int rhs) const { return words[0] == rhs && is_zero_from(1); }
    constexpr bool operator==(unsigned long long rhs) const { return words[0] == rhs && is_zero_from(1); }

    constexpr bool operator!=(const bigInt& rhs) const { return !(*this == rhs); }

    constexpr bool operator>(const bigInt& rhs) const {
        for (int i = bytes - 1; i >= 0; --i) {
            if (words[i] > rhs.words[i]) return true;
            if (words[i] < rhs.words[i]) return false;
        }
        return false;
    }

    constexpr bool operator>=(const bigInt& rhs) const { return (*this > rhs) || (*this == rhs); }
    constexpr bool operator<(const bigInt& rhs) const { return rhs > *this; }
    constexpr bool operator<=(const bigInt& rhs) const { return !(*this > rhs); }

    constexpr bigInt operator<<(int n) const { bigInt res = *this; res <<= n; return res; }

    constexpr bigInt& operator<<=(int n) {
        if (n <= 0) return *this;
        if (n >= bitCount) {
            std::fill(words, words + bytes, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (int i = bytes - 1; i >= 0; --i) {
            int ni = i + word_shift;
            if (ni >= static_cast<int>(bytes)) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part << bit_shift;
            if (ni + 1 < static_cast<int>(bytes) && bit_shift != 0) nw[ni + 1] |= part >> (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr bigInt operator>>(int n) const { bigInt res = *this; res >>= n; return res; }

    constexpr bigInt& operator>>=(int n) {
        if (n <= 0) return *this;
        if (n >= bitCount) {
            std::fill(words, words + bytes, 0);
            return *this;
        }
        int word_shift = n / 64;
        int bit_shift = n % 64;
        unsigned long long nw[bytes] = {0};
        for (size_t i = 0; i < bytes; ++i) {
            int ni = i - word_shift;
            if (ni < 0) continue;
            unsigned long long part = words[i];
            nw[ni] |= bit_shift == 0 ? part : part >> bit_shift;
            if (ni - 1 >= 0 && bit_shift != 0) nw[ni - 1] |= part << (64 - bit_shift);
        }
        std::copy(std::begin(nw), std::end(nw), std::begin(words));
        return *this;
    }

    constexpr bigInt operator|(const bigInt& rhs) const {
        bigInt r;
        for (size_t i = 0; i < bytes; ++i) r.words[i] = words[i] | rhs.words[i];
        return r;
    }

    constexpr bigInt& operator|=(const bigInt& rhs) {
        for (size_t i = 0; i < bytes; ++i) words[i] |= rhs.words[i];
        return *this;
    }

private:
    constexpr bool is_zero_from(size_t start) const {
        for (size_t i = start; i < bytes; ++i) if (words[i] != 0) return false;
        return true;
    }

#if defined(__SIZEOF_INT128__)
public:
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
#endif
};

template<unsigned long long bitCount>
requires (bitCount % 64 == 0)
std::ostream& operator<<(std::ostream& os, const bigInt<bitCount>& v) {
    if (v.is_zero()) return os << "0";
    char buf[bitCount / 4 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx" + (16 - (bitCount / 4) % 16), v.words[bitCount / 64 - 1]);
    for (int i = bitCount / 64 - 2; i >= 0; --i) {
        char temp[17];
        std::snprintf(temp, sizeof(temp), "%016llx", v.words[i]);
        std::strcat(buf, temp);
    }
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return os << "0";
    return os << out.substr(pos);
}

template<unsigned long long bitCount>
requires (bitCount % 64 == 0)
inline std::string toString(const bigInt<bitCount>& v) {
    if (v.is_zero()) return "0";
#if defined(__SIZEOF_INT128__)
    bigInt<bitCount> temp = v;
    std::string s;
    while (!temp.is_zero()) {
        s.push_back('0' + static_cast<unsigned>(temp.div_mod_bigInt64(10)));
    }
    for (std::string::size_type i = 0, j = s.size() - 1; i < j; ++i, --j) {
        char tmp = s[i];
        s[i] = s[j];
        s[j] = tmp;
    }
    return s;
#else
    char buf[bitCount / 4 + 1];
    std::snprintf(buf, sizeof(buf), "%016llx" + (16 - (bitCount / 4) % 16), v.words[bitCount / 64 - 1]);
    for (int i = bitCount / 64 - 2; i >= 0; --i) {
        char temp[17];
        std::snprintf(temp, sizeof(temp), "%016llx", v.words[i]);
        std::strcat(buf, temp);
    }
    std::string out(buf);
    size_t pos = out.find_first_not_of('0');
    if (pos == std::string::npos) return "0";
    return out.substr(pos);
#endif
}