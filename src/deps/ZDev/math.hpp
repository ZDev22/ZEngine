#ifndef MATH_HPP
#define MATH_HPP

#define PI 3.1415926535

#include <cmath>
#include <array>
#include <vector>
#include <cstring>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4244)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnarrowing"
#endif

// Global functions
inline float clamp(float value, float minValue, float maxValue) {
    if (value < minValue) { return minValue; }
    if (value > maxValue) { return maxValue; }
    return value;
}

// Interpolation
inline float smoothstep(const float edge0, const float edge1, const float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

inline float smootherstep(const float edge0, const float edge1, const float x) {
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
static uint32_t state = 730182364;

inline uint32_t xorshift32() {
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
inline bool randomBool() { return (xorshift32() & 1) == 0; }

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

// cmath debloated functions
inline constexpr short absoluteShort(const short i) { return (i < 0) ? -i : i; }
inline constexpr int absoluteInt(const int i) { return (i < 0) ? -i : i; }
inline constexpr long long absoluteLong(const long long i) { return (i < 0) ? -i : i; }
inline constexpr float absoluteFloat(const float i) { return (i < 0.f) ? -i : i; }

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#endif