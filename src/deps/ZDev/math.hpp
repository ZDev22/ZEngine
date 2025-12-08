// licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/ for current license

#pragma once

#define PI 3.1415926535f
#define PIR .0174532925f
#define PID 57.2957795131f
#define E 2.7182818284f
#define GOLDENRATIO 1.6180339887f
#define SQRT2 1.4142135624f
#define SQRT3 1.7320508076f
#define SQRT5 2.2360679775f
#define LN2 .6931471806f
#define LN10 2.3025850929f
#define APERY 1.2020569032f
#define FEIGENBAUM 4.6692016091f
#define PHI 1.6180339887f
#define EULERMASCHERONI .5772156649f
#define CATALAN .9159655941f
#define GLAISHER 1.2824271291f
#define Khinchin 2.6854520010f
#define LAPLACE 1.3862943611f
#define BERNOUILLI .0787456776f

#include <math.h>
#include <string.h>
#include <vector>

#ifdef BIG_INT_MATHPP
    #include "bigInts.hpp"
#endif
#ifdef BITSET_MATHPP
    #include "bitset.hpp"
#endif

// Global
inline float clamp(const float value, const float minValue, const float maxValue) {
    if (value < minValue) { return minValue; }
    if (value > maxValue) { return maxValue; }
    return value;
}

// Interpolation
inline float smoothStep(const float edge0, const float edge1, const float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}
inline float smootherStep(const float edge0, const float edge1, const float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6 - 15) + 10);
}
inline float linearInterpolate(const float a, const float b, const float t) { return a + (b - a) * t; }
inline float easeInSine(const float t) { return 1 - cos((t * PI) / 2); }
inline float easeOutSine(const float t) { return sin((t * PI) / 2); }
inline float easeInOutSine(const float t) { return -(cos(PI * t) - 1) / 2; }
inline float easeInExpo(const float t) { return (t == 0) ? 0 : pow(2, 10 * (t - 1)); }
inline float easeOutExpo(const float t) { return (t == 1) ? 1 : 1 - pow(2, -10 * t); }
inline float easeInOutExpo(const float t) {
    if (t == 0 || t == 1) { return t; }
    if (t < .5f) { return pow(2, 20 * t - 10) / 2; }
    return (2 - pow(2, -20 * t + 10)) / 2;
}
inline float easeInCirc(const float t) { return 1 - sqrt(1 - t * t); }
inline float easeOutCirc(const float t) { return sqrt(1 - (t - 1) * (t - 1)); }
inline float easeInOutCirc(const float t) {
    if (t < .5f) { return (1 - sqrt(1 - 4 * t * t)) / 2; }
    return (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}
inline float easeOutBounce(const float t) {
    if (t < 4.f / 11.f) { return (121.f * t * t) / 16.f; }
    else if (t < 8.f / 11.f) { return (363.f / 40.f * t * t) - (99.f / 10.f * t) + 17.f / 5.f; }
    else if (t < 9.f / 10.f) { return (4356.f / 361.f * t * t) - (35442 / 1805.f * t) + 16061.f / 1805.f; }
    return (54.f / 5.f * t * t) - (513.f / 25.f * t) + 268.f / 25.f;
}

// Random
static unsigned long long state = 381195919421132ULL;
inline unsigned long long xorshift32() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}
template<typename T> inline T Random(const T min, const T max) { return static_cast<T>(min + (xorshift32() % (max - min + 1))); }
inline float Random(const float min, const float max) { return min + (max - min) * (static_cast<float>(xorshift32()) / 4294967295.0f); }
inline bool Random() { return (xorshift32() & 1ULL) == 0ULL; }

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
    return static_cast<short>(sum / items.size());
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
template<typename T> void setZero(std::vector<T>& vec, unsigned int startIndex, unsigned int count) { memset(vec.data() + startIndex, 0, count); }
inline void setTrue(std::vector<bool>& vec, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = true; }
inline void setFalse(std::vector<bool>& vec, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = false; }

// Cmath debloated functions
template<typename T> inline T absolute(const T i) { return i < 0 ? -i : i; }
inline bool absolute(bool i) { return true; }

// Exponents
#ifdef BIG_INT_MATHPP
template<unsigned long long bitCount> inline bigInt<bitCount> exponent(bigInt<bitCount> base, unsigned int exp) {
    bigInt<bitCount> result = 1;
    while (exp > 0) {
        if (exp & 1) { result *= base; }
        base *= base;
        exp >>= 1;
    }
    return result;
}

template<unsigned long long bitCount> inline bigInt<bitCount> factorial(unsigned int n) {
    bigInt<bitCount> result = 1;
    for (unsigned int i = 1; i <= n; i++) { result *= i; }
    return result;
}

template<unsigned long long bitCount> inline bigInt<bitCount> superFactorial(unsigned int n) {
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

template<unsigned long long bitCount> inline bigInt<bitCount> exponentFactorial(unsigned int n) {
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

// Conversions
inline constexpr float radians(const float degrees) { return degrees * PIR; }
inline constexpr float degrees(const float radians) { return radians * PID; }
inline constexpr float celsius(const float fahrenheit) { return (fahrenheit - 32.f) * .5555555555f; }
inline constexpr float fahrenheit(const float celsius) { return (celsius / .5555555555f) + 32.f; }

// Bitset
#ifdef BITSET_MATHPP
template<unsigned long long range>
inline bitset<range> createRandomBitset() {
    bitset<range> bits;
    for (unsigned long long i = 0; i < range; i++) { if (Random()) bits.set(i); }
    return bits;
}
#endif