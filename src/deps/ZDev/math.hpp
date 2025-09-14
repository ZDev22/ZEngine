#pragma once

#define PI 3.1415926535
#define PIR 0.0174532925
#define PID 57.2957795131
#define E 2.7182818284
#define GOLDENRATIO 1.6180339887
#define SQRT2 1.4142135624
#define SQRT3 1.7320508076
#define SQRT5 2.2360679775
#define LN2 .6931471806
#define LN10 2.3025850929
#define APERY 1.2020569032
#define FEIGENBAUM 4.6692016091
#define PHI 1.6180339887
#define EULERMASCHERONI .5772156649
#define CATALAN .9159655941
#define GLAISHER 1.2824271291
#define Khinchin 2.6854520010
#define LAPLACE 1.3862943611
#define BERNOUILLI .0787456776

#include <cmath>
#include <cstring>

#include "vector.hpp"
#include "bigInts.hpp"

#include <glm/glm.hpp>

// Global
inline constexpr float clamp(const float value, const float minValue, const float maxValue) {
    if (value < minValue) { return minValue; }
    if (value > maxValue) { return maxValue; }
    return value;
}

// Interpolation
inline constexpr float smoothStep(const float edge0, const float edge1, const float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

inline constexpr float smootherStep(const float edge0, const float edge1, const float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

inline constexpr float linearInterpolate(const float a, const float b, const float t) { return a + (b - a) * t; }
inline constexpr float easeInSine(const float t) { return 1 - cos((t * PI) / 2); }
inline constexpr float easeOutSine(const float t) { return sin((t * PI) / 2); }
inline constexpr float easeInOutSine(const float t) { return -(cos(PI * t) - 1) / 2; }
inline constexpr float easeInExpo(const float t) { return (t == 0) ? 0 : pow(2, 10 * (t - 1)); }
inline constexpr float easeOutExpo(const float t) { return (t == 1) ? 1 : 1 - pow(2, -10 * t); }
inline constexpr float easeInOutExpo(const float t) {
    if (t == 0 || t == 1) { return t; }
    if (t < .5f) { return pow(2, 20 * t - 10) / 2; }
    return (2 - pow(2, -20 * t + 10)) / 2;
}

inline constexpr float easeInCirc(const float t) { return 1 - sqrt(1 - t * t); }
inline constexpr float easeOutCirc(const float t) { return sqrt(1 - (t - 1) * (t - 1)); }
inline constexpr float easeInOutCirc(const float t) {
    if (t < .5f) { return (1 - sqrt(1 - 4 * t * t)) / 2; }
    return (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}

inline constexpr float easeOutBounce(const float t) {
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
inline unsigned char randomChar(const unsigned char min, unsigned const char max) { return static_cast<unsigned char>(min + (xorshift32() % (max - min + 1))); }
inline short randomShort(const short min, const short max) { return static_cast<short>(min + (xorshift32() % (max - min + 1))); }
inline int randomInt(const int min, const int max) { return static_cast<int>(min + (xorshift32() % (max - min + 1))); }
inline long long randomLong(const long long min, const long long max) { return static_cast<long long>(min + (xorshift32() % (max - min + 1))); }
inline unsigned short randomUnsignedShort(const unsigned short min, const unsigned short max) { return static_cast<unsigned short>(min + (xorshift32() % (max - min + 1))); }
inline unsigned int randomUnsignedInt(const unsigned int min, const unsigned int max) { return static_cast<unsigned int>(min + (xorshift32() % (max - min + 1))); }
inline unsigned long long randomUnsignedLong(const unsigned long long min, const unsigned long long max) { return static_cast<unsigned long long>(min + (xorshift32() % (max - min + 1))); }
inline float randomFloat(const float min, const float max) { return min + (max - min) * (static_cast<float>(xorshift32()) / 4294967295.0f); }
inline bool randomBool() { return (xorshift32() & 1ULL) == 0ULL; }

// Weight table
class WeightTable {
public:
    struct WeightTableValues {
        unsigned int weight;
        int value;
        unsigned int ID;
        unsigned long long offset;
    };

    WeightTable() {}

    void addItem(unsigned int weight, int value) {
        WeightTableValues weightValues;
        weightValues.weight = weight;
        weightValues.value = value;
        weightValues.ID = static_cast<unsigned int>(weights.size());
        weightValues.offset = 0;
        weights.push_back(weightValues);
    }

    void removeItem(unsigned int ID) { weights.erase(ID); }

    unsigned int getRandomValue() {
        unsigned long long offset = 0ULL;
        for (size_t i = 0; i < weights.size(); i++) {
            weights[i].offset = offset;
            offset += weights[i].weight;
        }

        unsigned long long number = randomUnsignedLong(0ULL, 18446744073709551615ULL);
        for (size_t i = 0; i < weights.size(); i++) { if (number >= weights[i].offset && number < weights[i].offset + weights[i].weight) { return weights[i].value; }}
        return -1;
    }

    vector<WeightTableValues> getTable() const { return weights; }

private:
    vector<WeightTableValues> weights;
};    

// Averages
inline short averageChar(const vector<unsigned char>& chars) {
    if (chars.size() == 0) return 0;
    int sum = 0;
    for (size_t i = 0; i < chars.size(); ++i) sum += chars[i];
    return static_cast<short>(sum / static_cast<int>(chars.size()));
}
inline short averageShort(const vector<short>& shorts) {
    if (shorts.size() == 0) return 0;
    int sum = 0;
    for (size_t i = 0; i < shorts.size(); ++i) sum += shorts[i];
    return static_cast<short>(sum / static_cast<int>(shorts.size()));
}
inline int averageInt(const vector<int>& ints) {
    if (ints.size() == 0) return 0;
    long long sum = 0;
    for (size_t i = 0; i < ints.size(); ++i) sum += ints[i];
    return static_cast<int>(sum / static_cast<long long>(ints.size()));
}
inline long long averageLong(const vector<long long>& longs) {
    if (longs.size() == 0) return 0;
    long long sum = 0;
    for (size_t i = 0; i < longs.size(); ++i) sum += longs[i];
    return sum / static_cast<long long>(longs.size());
}
inline float averageFloat(const vector<float>& floats) {
    if (floats.size() == 0) return 0.0f;
    float sum = 0.0f;
    for (size_t i = 0; i < floats.size(); ++i) sum += floats[i];
    return sum / static_cast<float>(floats.size());
}
inline bool averageBool(const vector<bool>& bools) {
    if (bools.size() == 0) return randomBool();
    int averageTrue = 0, averageFalse = 0;
    for (size_t i = 0; i < bools.size(); ++i) {
        if (bools[i]) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return randomBool();
}
inline short averageMinMaxChar(const vector<unsigned char>& chars) {
    if (chars.size() == 0) return 0;
    unsigned char minVal = chars[0], maxVal = chars[0];
    int sum = 0;

    for (size_t i = 0; i < chars.size(); ++i) {
        unsigned char a = chars[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (size_t i = 0; i < chars.size(); ++i) {
        unsigned char a = chars[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return static_cast<short>(sum / static_cast<int>(chars.size()));
}
inline short averageMinMaxShort(const vector<short>& shorts) {
    if (shorts.size() == 0) return 0;
    short minVal = shorts[0], maxVal = shorts[0];
    int sum = 0;

    for (size_t i = 0; i < shorts.size(); ++i) {
        short a = shorts[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (size_t i = 0; i < shorts.size(); ++i) {
        short a = shorts[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return static_cast<short>(sum / static_cast<int>(shorts.size()));
}
inline int averageMinMaxInt(const vector<int>& ints) {
    if (ints.size() == 0) return 0;
    int minVal = ints[0], maxVal = ints[0];
    long long sum = 0;

    for (size_t i = 0; i < ints.size(); ++i) {
        int a = ints[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (size_t i = 0; i < ints.size(); ++i) {
        int a = ints[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return static_cast<int>(sum / static_cast<long long>(ints.size()));
}
inline long long averageMinMaxLong(const vector<long long>& longs) {
    if (longs.size() == 0) return 0;
    long long minVal = longs[0], maxVal = longs[0], sum = 0;

    for (size_t i = 0; i < longs.size(); ++i) {
        long long a = longs[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (size_t i = 0; i < longs.size(); ++i) {
        long long a = longs[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return sum / static_cast<long long>(longs.size());
}
inline float averageMinMaxFloat(const vector<float>& floats) {
    if (floats.size() == 0) return 0.0f;
    float minVal = floats[0], maxVal = floats[0], sum = 0.0f;

    for (size_t i = 0; i < floats.size(); ++i) {
        float a = floats[i];
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (size_t i = 0; i < floats.size(); ++i) {
        float a = floats[i];
        if (a != minVal && a != maxVal) sum += a;
    }
    return sum / static_cast<float>(floats.size());
}

// Values
inline void setValuesInRangeChar(vector<unsigned char>& vec, const unsigned char value, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
inline void setValuesInRangeShort(vector<short>& vec, const short value, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
inline void setValuesInRangeInt(vector<int>& vec, const int value, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
inline void setValuesInRangeLong(vector<long long>& vec, const long long value, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
inline void setValuesInRangeFloat(vector<float>& vec, const float value, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = value; }
inline void setZeroChar(vector<unsigned char>& vec, size_t startIndex, size_t count) { std::memset(vec.data() + startIndex, 0, count); }
inline void setZeroShort(vector<short>& vec, size_t startIndex, size_t count) { std::memset(vec.data() + startIndex, 0, count * 2); }
inline void setZeroInt(vector<int>& vec, size_t startIndex, size_t count) { std::memset(vec.data() + startIndex, 0, count * 4); }
inline void setZeroLong(vector<long long>& vec, size_t startIndex, size_t count) { std::memset(vec.data() + startIndex, 0, count * 8); }
inline void setZeroFloat(vector<float>& vec, size_t startIndex, size_t count) { std::memset(vec.data() + startIndex, 0, count * 4); }
inline void setTrue(vector<bool>& vec, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = true; }
inline void setFalse(vector<bool>& vec, int minIndex, int maxIndex) { for (int i = minIndex; i < maxIndex; ++i) vec[i] = false; }

// Cmath debloated functions
inline constexpr short absoluteShort(const short i) { return (i < 0) ? -i : i; }
inline constexpr int absoluteInt(const int i) { return (i < 0) ? -i : i; }
inline constexpr long long absoluteLong(const long long i) { return (i < 0) ? -i : i; }
inline constexpr float absoluteFloat(const float i) { return (i < 0.f) ? -i : i; }

// Exponents
template<unsigned long long bitCount>
    requires (bitCount % 64 == 0)
inline constexpr bigInt<bitCount> exponent(bigInt<bitCount> value, unsigned int exponent) {
    bigInt<bitCount> base = value;
    for (unsigned int i = 1; i < exponent; i++) { value *= base; }
    return value;
}

template<unsigned long long bitCount>
    requires (bitCount % 64 == 0)
inline constexpr bigInt<bitCount> factorial(unsigned int exponent) {
    bigInt<bitCount> result = bigInt<bitCount>(1);
    int i = 2;
    for (; i + 3 <= exponent; i += 4) {
        result *= bigInt<bitCount>(i) * bigInt<bitCount>(i + 1);
        result *= bigInt<bitCount>(i + 2) * bigInt<bitCount>(i + 3);
    }
    for (; i <= exponent; ++i) { result *= bigInt<bitCount>(i); }
    return result;
}

// Points
inline constexpr float slopeOf(const glm::vec2& point1, const glm::vec2& point2) { return ((point2.y - point1.y) / (point2.x - point1.x)); }
inline glm::vec2 averagePoint(const vector<glm::vec2>& points) {
    glm::vec2 avg = glm::vec2(0.f);
    if (points.size() == 0) { return avg; }
    for (size_t i = 0; i < points.size(); ++i) { avg += points[i]; }
    return avg / static_cast<float>(points.size());
}

// Conversions
inline constexpr float radians(const float degrees) { return degrees * PIR; }
inline constexpr float degrees(const float radians) { return radians * PID; }
inline constexpr float celsius(const float fahrenheit) { return (fahrenheit - 32) * .5555555555; }
inline constexpr float fahrenheit(const float celsius) { return (celsius * 2) + 12; }