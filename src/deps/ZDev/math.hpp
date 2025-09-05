#pragma once

#define PI 3.1415926535
#define E 2.7182818284
#define GOLDENRATIO 1.6180339887
#define SQRT2 1.4142135624
#define SQRT3 1.7320508076
#define LN2 .6931471806
#define LN10 2.3025850929
#define APERY 1.2020569032
#define FEIGENBAUM1 4.6692016091
#define FEIGENBAUM2 2.5029078750
#define SQRT5 2.2360679775
#define PHI 1.6180339887
#define EULERMASCHERONI .5772156649
#define CATALAN .9159655941
#define GLAISHER 1.2824271291
#define Khinchin 2.6854520010
#define LAPLACE 1.3862943611
#define BERNOUILLI .0787456776

#include <cmath>
#include <vector>
#include <cstring>
#include <cstdint>

#include "../../deps/ZDev/bigInts.hpp"

#include <glm/glm.hpp>

// Global functions
inline constexpr float clamp(float value, float minValue, float maxValue) {
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
static unsigned long long state = 381195919421132;

inline unsigned long long xorshift32() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

inline unsigned char randomChar(const char min, const char max) { return min + (xorshift32() % (max - min + 1)); }
inline short randomShort(const short min, const short max) { return min + (xorshift32() % (max - min + 1)); }
inline int randomInt(const int min, const int max) { return min + (xorshift32() % (max - min + 1)); }
inline long long randomLong(const long long min, const int max) { return min + (xorshift32() % (max - min + 1)); }
inline float randomFloat(const float min, const float max) { return min + (max - min) * (xorshift32() / 4294967295.0f); }
inline unsigned char randomUnsignedChar(const unsigned char min, const unsigned char max) { return min + (xorshift32() % (max - min + 1)); }
inline short randomUnsignedShort(const unsigned short min, const unsigned short max) { return min + (xorshift32() % (max - min + 1)); }
inline int randomUnsignedInt(const unsigned int min, const unsigned int max) { return min + (xorshift32() % (max - min + 1)); }
inline long long randomUnsignedLong(const unsigned long long min, const unsigned int max) { return min + (xorshift32() % (max - min + 1)); }
inline float randomUnsignedFloat(const unsigned float min, const unsigned float max) { return min + (max - min) * (xorshift32() / 4294967295.0f); }
inline bool randomBool() { return (xorshift32() & 1) == 0; }

// Weight table
class WeightTable {
public:
    WeightTable() {}

    void addItem(unsigned int weight, int value) {
        WeigthTableValues weightValues;
        weightValues.weight = weight;
        weightValues.value = value;
        weightValues.ID = weights.size();
        weightValues.offset = 0;
        weights.push_back(weightValues);
    }

    void removeItem(unsigned int ID) { weights.erase(ID); }

    int getRandomValue() {
        unsigned long long offset = 0;
        for (size_t i = 0; i < weights.size(); i++) {
            weights[i].offset = offset;
            offset += weights[i].weight;
        }

        unsigned long long number = randomUnsignedLong(0, 18446744073709551615);
        for (size_t i = 0; i < weights.size(); i++) { if (weights[i].offset >= number) { return weights[i].value; }}
    }

    std::vector<WeightTableValues> getTable() { return weights; }

private:
    struct WeightTableValues {
        unsigned int weight;
        int value;
        unsigned int ID;
        unsigned long long offset;
    };
    std::vector<WeightTableValues> weights = {};
};

// Averages
inline short averageChar(const std::vector<unsigned char>& chars) {
    short average = 0;
    for (const short a : chars) { average += a; }
    return average / chars.size();
}

inline short averageShort(const std::vector<short>& shorts) {
    int average = 0;
    for (const short a : shorts) { average += a; }
    return average / shorts.size();
}

inline int averageInt(const std::vector<int>& ints) {
    long long average = 0;
    for (const int a : ints) { average += a; }
    return average / ints.size();
}

inline long long averageLong(const std::vector<long long>& longs) {
    long long average = 0;
    for (const long long a : longs) { average += a; }
    return average / longs.size();
}

inline float averageFloat(const std::vector<float>& floats) {
    float average = 0;
    for (const float a : floats) { average += a; }
    return average / floats.size();
}

inline bool averageBool(const std::vector<bool>& bools) {
    int averageTrue = 0, averageFalse = 0;
    for (bool a : bools) {
        if (a) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return randomBool();
}

inline short averageMinMaxChar(const std::vector<unsigned char>& chars) {
    unsigned char minVal = chars[0], maxVal = chars[0];
    short sum = 0;

    for (const short a : chars) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : chars) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / chars.size();
}

inline short averageMinMaxShort(const std::vector<short>& shorts) {
    short minVal = shorts[0], maxVal = shorts[0];
    int sum = 0;

    for (const short a : shorts) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : shorts) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / shorts.size();
}

inline int averageMinMaxInt(const std::vector<int>& ints) {
    int minVal = ints[0], maxVal = ints[0];
    long long sum = 0;

    for (const int a : ints) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const int a : ints) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / ints.size();
}

inline long long averageMinMaxLong(const std::vector<long long>& longs) {
    long long minVal = longs[0], maxVal = longs[0], sum = 0;

    for (const long long a : longs) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const long long a : longs) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / longs.size();
}

inline float averageMinMaxFloat(const std::vector<float>& floats) {
    float minVal = floats[0], maxVal = floats[0], sum = 0;

    for (const float a : floats) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const float a : floats) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / floats.size();
}

// Values
inline void setValuesInRangeChar(std::vector<unsigned char>& vec, const unsigned char value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeShort(std::vector<short>& vec, const short value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeInt(std::vector<int>& vec, const int value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeLong(std::vector<long long>& vec, const long long value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeFloat(std::vector<float>& vec, const float value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setZeroChar(std::vector<unsigned char>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count); }
inline void setZeroShort(std::vector<short>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 2); }
inline void setZeroInt(std::vector<int>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }
inline void setZeroLong(std::vector<long long>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 8); }
inline void setZeroFloat(std::vector<float>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }

inline void setTrue(std::vector<bool>& vec, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, true); }
inline void setFalse(std::vector<bool>& vec, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, false); }

// Cmath debloated functions
inline constexpr short absoluteShort(const short i) { return (i < 0) ? -i : i; }
inline constexpr int absoluteInt(const int i) { return (i < 0) ? -i : i; }
inline constexpr long long absoluteLong(const long long i) { return (i < 0) ? -i : i; }
inline constexpr float absoluteFloat(const float i) { return (i < 0.f) ? -i : i; }

// Factorials
inline constexpr uint64_t factorial64(int n) {
    uint64_t result = uint64_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint64_t(i) * uint64_t(i + 1);
        result *= uint64_t(i + 2) * uint64_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint64_t(i); }
    return result;
}
inline constexpr uint128_t factorial128(int n) {
    uint128_t result = uint128_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint128_t(i) * uint128_t(i + 1);
        result *= uint128_t(i + 2) * uint128_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint128_t(i); }
    return result;
}
inline constexpr uint256_t factorial256(int n) {
    uint256_t result = uint256_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint256_t(i) * uint256_t(i + 1);
        result *= uint256_t(i + 2) * uint256_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint256_t(i); }
    return result;
}
inline constexpr uint512_t factorial512(int n) {
    uint512_t result = uint512_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint512_t(i) * uint512_t(i + 1);
        result *= uint512_t(i + 2) * uint512_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint512_t(i); }
    return result;
}
inline constexpr uint1024_t factorial1024(int n) {
    uint1024_t result = uint1024_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint1024_t(i) * uint1024_t(i + 1);
        result *= uint1024_t(i + 2) * uint1024_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint1024_t(i); }
    return result;
}
inline constexpr uint2048_t factorial2048(int n) {
    uint2048_t result = uint2048_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint2048_t(i) * uint2048_t(i + 1);
        result *= uint2048_t(i + 2) * uint2048_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint2048_t(i); }
    return result;
}
inline constexpr uint4096_t factorial4096(int n) {
    uint4096_t result = uint4096_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint4096_t(i) * uint4096_t(i + 1);
        result *= uint4096_t(i + 2) * uint4096_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint4096_t(i); }
    return result;
}
inline constexpr uint8192_t factorial8192(int n) {
    uint8192_t result = uint8192_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint8192_t(i) * uint8192_t(i + 1);
        result *= uint8192_t(i + 2) * uint8192_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint8192_t(i); }
    return result;
}
inline constexpr uint16384_t factorial16384(int n) {
    uint16384_t result = uint16384_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint16384_t(i) * uint16384_t(i + 1);
        result *= uint16384_t(i + 2) * uint16384_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint16384_t(i); }
    return result;
}
inline constexpr uint32768_t factorial32768(int n) {
    uint32768_t result = uint32768_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint32768_t(i) * uint32768_t(i + 1);
        result *= uint32768_t(i + 2) * uint32768_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint32768_t(i); }
    return result;
}
inline constexpr uint65536_t factorial65536(int n) {
    uint65536_t result = uint65536_t(1);
    int i = 2;
    for (; i + 3 <= n; i += 4) {
        result *= uint65536_t(i) * uint65536_t(i + 1);
        result *= uint65536_t(i + 2) * uint65536_t(i + 3);
    }
    for (; i <= n; ++i) { result *= uint65536_t(i); }
    return result;
}

// Points
inline constexpr float slopeOf(glm::vec2& point1, glm::vec2& point2) { return ((point2.y - point1.y) / (point2.x - point1.x)); }
inline glm::vec2 averagePoint(std::vector<glm::vec2> points) {
    glm::vec2 averagePoint = glm::vec2(0.f);
    for (int i = 0; i < points.size(); i++) { averagePoint += points[i]; }
    return averagePoint / glm::vec2(points.size());
}