/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/ for current license

v2.6.9

zmath.hpp is a lightweight cross-platform single-header cpp library for high-preformance math functions!
Made for the purpose of gaming, like RNG and weight-table functions, as well as doing my homework.
Works with bigInts.hpp and bitset.hpp if you want, but not required!

#define ZMATH_BIG_INT - Use bigInt.hpp along with zmath, enables bigInt functions
#define ZMATH_BITSET - Use bitset.hpp along with zmath, enable bitset functions
*/

#pragma once

#define ZMATH_PI              3.1415926535f
#define ZMATH_PIR              .0174532925f
#define ZMATH_PID            57.2957795131f
#define ZMATH_2PI             6.2831853071f
#define ZMATH_SQRTPI          1.7724538509f
#define ZMATH_HALFPI          1.5707963267f
#define ZMATH_2THIRDSPI       4.7123889803f
#define ZMATH_1FOURTHPI        .7853981633f
#define ZMATH_1OVERPI          .3183098861f
#define ZMATH_E               2.7182818284f
#define ZMATH_EULER            .5772156649f
#define ZMATH_GOLDENRATIO     1.6180339887f
#define ZMATH_SQRT2           1.4142135624f
#define ZMATH_SQRT3           1.7320508076f
#define ZMATH_SQRT5           2.2360679775f
#define ZMATH_LN2              .6931471806f
#define ZMATH_LN10            2.3025850929f
#define ZMATH_APERY           1.2020569032f
#define ZMATH_FEIGENBAUM      4.6692016091f
#define ZMATH_PHI             1.6180339887f
#define ZMATH_EULERMASCHERONI  .5772156649f
#define ZMATH_CATALAN          .9159655941f
#define ZMATH_GLAISHER        1.2824271291f
#define ZMATH_KHINCHIN        2.6854520010f
#define ZMATH_LAPLACE         1.3862943611f
#define ZMATH_BERNOUILLI       .0787456776f
#define ZMATH_1THIRD           .3333333333f
#define ZMATH_2THIRDS          .6666666666f

#ifdef ZMATH_BIG_INT
    #include "bigInts.hpp"
#endif
#ifdef ZMATH_BITSET
    #include "bitset.hpp"
#endif

#include <math.h>
#include <string.h>
#include <stdio.h>

#include <vector>

// Interpolation
template<typename T> inline T clamp(T num, T min, T max) {
    if (num < min) { return min; }
    if (num > max) { return max; }
    return num;
}

inline float interpolate(float x, float y, float speed) { return x + speed * (y - x); }
inline float remap(float num, float inputx, float inputy, float outputx, float outputy) {  return (num - inputx) / (inputy - inputx) * (outputy - outputx) + outputx; }
inline float normalize(float num, float x, float y) {  return (num - x) / (y - num); }
inline float wrap(float num, float min, float max) { return num - (max - min) * floorf((num - min) / (max - min)); }

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

namespace zmath {
    struct Vec2 { float x; float y; };
    struct Vec3 { float x; float y; float z; };
    struct Vec4 { float x; float y; float z; float w; };

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
            for (auto weight : weights) {
                weight.offset = offset;
                offset += weight.weight;
            }

            unsigned long long number = Random(0ULL, offset);
            for (auto weight : weights) { if (number >= weight.offset && number < weight.offset + weight.weight) { return weight.value; }}
            return -1;
        }

        std::vector<WeightTableValues> getTable() const { return weights; }

    private:
        std::vector<WeightTableValues> weights;
    };
}

// Averages
template<typename T> inline T average(const std::vector<T>& items) {
    T sum = 0;
    for (auto item : items) { sum += item; }
    return sum / items.size();
}
inline bool average(const std::vector<bool>& bools) {
    if (bools.size() == 0) return Random();
    int averageTrue = 0, averageFalse = 0;
    for (auto boolean : bools) {
        if (boolean) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return Random();
}
template<typename T> inline T averageMinMax(const std::vector<T>& items) {
    T minVal = items[0], maxVal = items[0];
    unsigned long long sum = 0;

    for (auto item : items) {
        if (item < minVal) { minVal = item; }
        else if (item > maxVal) { maxVal = item; }
        else { sum += item; }
    }
    return (short)(sum / items);
}
inline float averageMinMax(const std::vector<float>& floats) {
    float minVal = floats[0], maxVal = floats[0], sum = 0.f;

    for (auto floater : floats) {
        float a = floater;
        if (floater < minVal) { minVal = floater; }
        else if (a > maxVal) { maxVal = floater; }
        else { sum += a; }
    }
    return sum / floats.size();
}

// Values
template<typename T> inline void setValuesInRange(std::vector<T>& vec, const T value, unsigned int minIndex, unsigned int maxIndex) { for (unsigned int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
template<typename T> inline void setFalse(std::vector<T>& vec, unsigned int startIndex, unsigned int count) { memset(vec.data() + startIndex, 0, count); }
template<typename T> inline void setTrue(std::vector<T>& vec, unsigned int startIndex, unsigned int count) { memset(vec.data() + startIndex, 1, count); }

// Cmath debloated functions
template<typename T> inline T absolute(const T i) { return i < 0 ? -i : i; }
inline bool absolute(bool i) { return true; }

// Advanced
template<typename T> bool prime(T num) {
    unsigned char divisors = 0;
    for (T i = 1; i <= num; i++) {
        if (num % i == 0) {
            if (++divisors == 3) { return false; }
        }
    }
    return true;
}

inline const char* findFactor(signed char add, signed char multiply) {
    signed char x = 0;
    signed char y = 0;
    if (add > 0) {
        for (signed char i = add; i >= -add * 3; i--) {
            x = i;
            y = add - i;
            goto factor;
        }
        return "none";
    }
    else {
        for (signed char i = add; i <= -add * 3; i++) {
            x = i;
            y = add - i;
            goto factor;
        }
        return "none";
    }
    factor:
    char* result = (char*)malloc(20);
    snprintf(result, 20, "x = %d | x = %d", -x, -y);
    return (const char*)result;
}

// Conversions
inline constexpr float radians(const float degrees) { return degrees * ZMATH_PIR; }
inline constexpr float degrees(const float radians) { return radians * ZMATH_PID; }
inline constexpr float celsius(const float fahrenheit) { return (fahrenheit - 32.f) * .5555555555f; }
inline constexpr float fahrenheit(const float celsius) { return (celsius / .5555555555f) + 32.f; }

// BigInts
#ifdef ZMATH_BIG_INT
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
    for (auto leggs : legs) { result *= leggs; }
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

// Bitset
#ifdef ZMATH_BITSET
template<BITSET_VARIABLE_TYPE T> inline Bitset createRandomBitset() {
    Bitset bits(T);
    for (unsigned long long i = 0; i < T * sizeof(T); i++) { if (Random()) { bits.set(i); } }
    return bits;
}
#endif