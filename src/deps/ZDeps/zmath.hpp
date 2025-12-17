/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/ for current license

v1.6.9

math.hpp is a lightweight cross-platform single-header cpp library for high-preformance math functions!
Made for the purpose of gaming, like RNG and weight-table functions, as well as doing my homework.
Works with bigInts.hpp and bitset.hpp if you want, but not required!
*/

#pragma once

#define MATHPP_PI              3.1415926535f
#define MATHPP_PIR              .0174532925f
#define MATHPP_PID            57.2957795131f
#define MATHPP_E               2.7182818284f
#define MATHPP_GOLDENRATIO     1.6180339887f
#define MATHPP_SQRT2           1.4142135624f
#define MATHPP_SQRT3           1.7320508076f
#define MATHPP_SQRT5           2.2360679775f
#define MATHPP_LN2              .6931471806f
#define MATHPP_LN10            2.3025850929f
#define MATHPP_APERY           1.2020569032f
#define MATHPP_FEIGENBAUM      4.6692016091f
#define MATHPP_PHI             1.6180339887f
#define MATHPP_EULERMASCHERONI  .5772156649f
#define MATHPP_CATALAN          .9159655941f
#define MATHPP_GLAISHER        1.2824271291f
#define MATHPP_Khinchin        2.6854520010f
#define MATHPP_LAPLACE         1.3862943611f
#define MATHPP_BERNOUILLI       .0787456776f

#ifdef BIG_INT_MATHPP
    #include "bigInts.hpp"
#endif
#ifdef BITSET_MATHPP
    #include "bitset.hpp"
#endif

#include <math.h>
#include <string.h>
#include <stdio.h>

#include <vector>

// Random
static unsigned long long state = 381195919421132ULL;
inline unsigned long long xorshift() {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
}
template<typename T> inline T Random(const T min, const T max) { return (T)(min + (xorshift() % (max - min + 1))); }
inline float Random(const float min, const float max) { return min + (max - min) * ((float)(xorshift()) / 4294967295.0f); }
inline bool Random() { return (xorshift() & 1ULL) == 0ULL; }

// Weight table
struct WeightTable {
public:
    struct WeightTableValues {
        unsigned int weight;
        int value;
        unsigned int ID;
        unsigned int offset;
    };

    void addItem(unsigned int weight, int value) {
        WeightTableValues weightValues;
        weightValues.weight = weight;
        weightValues.value = value;
        weightValues.ID = weights.size();
        weightValues.offset = 0;
        weights.push_back(weightValues);
    }

    void removeItem(unsigned int ID) {
        weights.erase(weights.begin() + ID);
        for (unsigned int i = ID - 2; i < weights.size(); i++) { weights[i].ID = i; }
    }

    unsigned int getRandomValue() {
        unsigned long long offset = 0;
        for (unsigned int i = 0; i < weights.size(); i++) {
            weights[i].offset = offset;
            offset += weights[i].weight;
        }

        unsigned long long number = Random(0ULL, offset);
        for (unsigned int i = 0; i < weights.size(); i++) { if (number >= weights[i].offset && number < weights[i].offset + weights[i].weight) { return weights[i].value; }}
        return -1;
    }

    std::vector<WeightTableValues> getTable() const { return weights; }

private:
    std::vector<WeightTableValues> weights;
};

// Averages
template<typename T> inline T average(const std::vector<T>& items) {
    T sum = 0;
    for (unsigned int i = 0; i < items.size(); ++i) sum += items[i];
    return sum / items.size();
}
inline bool average(const std::vector<bool>& bools) {
    if (bools.size() == 0) return Random();
    int averageTrue = 0, averageFalse = 0;
    for (unsigned int i = 0; i < bools.size(); ++i) {
        if (bools[i]) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return Random();
}
template<typename T> inline T averageMinMax(const std::vector<T>& items) {
    T minVal = items[0], maxVal = items[0];
    unsigned long long sum = 0;

    for (unsigned int i = 0; i < items.size(); ++i) {
        unsigned long long a = items[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (unsigned int i = 0; i < items.size(); ++i) {
        unsigned long long a = items[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return (short)(sum / items.size());
}
inline float averageMinMax(const std::vector<float>& floats) {
    float minVal = floats[0], maxVal = floats[0], sum = 0.f;

    for (unsigned int i = 0; i < floats.size(); ++i) {
        float a = floats[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (unsigned int i = 0; i < floats.size(); ++i) {
        float a = floats[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return sum / floats.size();
}

// Values
template<typename T> void setValuesInRange(std::vector<T>& vec, const T value, unsigned int minIndex, unsigned int maxIndex) { for (unsigned int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
template<typename T> void setFalse(std::vector<T>& vec, unsigned int startIndex, unsigned int count) { memset(vec.data() + startIndex, 0, count); }
template<typename T> void setTrue(std::vector<T>& vec, unsigned int startIndex, unsigned int count) { memset(vec.data() + startIndex, 1, count); }

// Cmath debloated functions
template<typename T> inline T absolute(const T i) { return i < 0 ? -i : i; }
inline bool absolute(bool i) { return true; }

// Exponents
#ifdef BIG_INT_MATHPP
template<unsigned int bitCount> inline bigInt<bitCount> exponent(bigInt<bitCount> base, unsigned int exp) {
    bigInt<bitCount> result = 1;
    while (exp > 0) {
        if (exp & 1) { result *= base; }
        base *= base;
        exp >>= 1;
    }
    return result;
}

template<unsigned int bitCount> inline bigInt<bitCount> factorial(unsigned int n) {
    bigInt<bitCount> result = 1;
    for (unsigned int i = 1; i <= n; i++) { result *= i; }
    return result;
}

template<unsigned int bitCount> inline bigInt<bitCount> superFactorial(unsigned int n) {
    std::vector<bigInt<bitCount>> legs;
    bigInt<bitCount> result = 1;

    for (unsigned int q = n; q > 0; q--) {
        for (unsigned int i = 1; i <= q; i++) { result *= i; }
        legs.push_back(result);
        result = 1;
    }

    result = 1;
    for (unsigned int i = 0; i < legs.size(); i++) { result *= legs[i]; }
    return result;
}

template<unsigned int bitCount> inline bigInt<bitCount> exponentFactorial(unsigned int n) {
    bigInt<bitCount> result = 1;
    for (unsigned int i = 1; i <= n; i++) { result *= i; }
    bigInt<bitCount> base = result;
    for (unsigned int i = 1; i < n; i++) { result *= base; }
    return result;
}
#endif

// Advanced
template<typename T> inline bool prime(T num) {
    unsigned char divisors = 0;
    for (T i = 1; i <= num; i++) {
        if (num % i == 0) {
            if (++divisors == 3) { return false; }
        }
    }
    return true;
}

inline const char* findFactor(signed char add, signed char multiply) {
    std::vector<signed char> factors;
    if (add > 0) {
        for (signed char i = add; i >= -add * 3; i--) {
            factors.push_back(i);
            factors.push_back(add - i);
        }
    }
    else {
        for (signed char i = add; i <= -add * 3; i++) {
            factors.push_back(i);
            factors.push_back(add - i);
        }
    }
    for (unsigned char i = 0; i < factors.size(); i += 2) {
        if (factors[i] * factors[i + 1] == multiply) {
            char* result = (char*)malloc(20);
            snprintf(result, 20, "x = %d | x = %d", -factors[i], -factors[i + 1]);
            return (const char*)result;
        }
    }
    return "none";
}

// Conversions
inline constexpr float radians(const float degrees) { return degrees * MATHPP_PIR; }
inline constexpr float degrees(const float radians) { return radians * MATHPP_PID; }
inline constexpr float celsius(const float fahrenheit) { return (fahrenheit - 32.f) * .5555555555f; }
inline constexpr float fahrenheit(const float celsius) { return (celsius / .5555555555f) + 32.f; }

// Bitset
#ifdef BITSET_MATHPP
template<BITSET_VARIABLE_TYPE T>
inline Bitset createRandomBitset() {
    Bitset bits(T);
    for (unsigned long long i = 0; i < T * sizeof(T); i++) { if (Random()) bits.set(i); }
    return bits;
}
#endif