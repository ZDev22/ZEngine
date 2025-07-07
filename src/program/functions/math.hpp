#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include <array>
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <algorithm>

using namespace std;

// Global functions
inline float clamp(float value, float minValue, float maxValue) { return max(minValue, min(maxValue, value)); }

// Interpolation
inline float lerp(float a, float b, float t) { return a + (b - a) * t; }
inline float smoothstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) return edge0;
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

inline float smootherstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) return edge0;
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

inline float easeInSine(float t) { return 1 - cos((t * 3.14159265358979323846) / 2); }
inline float easeOutSine(float t) { return sin((t * 3.14159265358979323846) / 2); }
inline float easeInOutSine(float t) { return -(cos(3.14159265358979323846 * t) - 1) / 2; }
inline float easeInExpo(float t) { return (t == 0) ? 0 : pow(2, 10 * (t - 1)); }
inline float easeOutExpo(float t) { return (t == 1) ? 1 : 1 - pow(2, -10 * t); }
inline float easeInOutExpo(float t) {
    if (t == 0) return 0;
    if (t == 1) return 1;
    if (t < 0.5f) return pow(2, 20 * t - 10) / 2;
    return (2 - pow(2, -20 * t + 10)) / 2;
}

inline float easeInCirc(float t) { return 1 - sqrt(1 - t * t); }
inline float easeOutCirc(float t) { return sqrt(1 - (t - 1) * (t - 1)); }
inline float easeInOutCirc(float t) {
    if (t < 0.5f) return (1 - sqrt(1 - 4 * t * t)) / 2;
    return (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}

inline float easeOutBounce(float t) {
    if (t < 4 / 11.0f) { return (121 * t * t) / 16.0f; } 
    else if (t < 8 / 11.0f) { return (363 / 40.0f * t * t) - (99 / 10.0f * t) + 17 / 5.0f; }
    else if (t < 9 / 10.0f) { return (4356 / 361.0f * t * t) - (35442 / 1805.0f * t) + 16061 / 1805.0f; }
    return (54 / 5.0f * t * t) - (513 / 25.0f * t) + 268 / 25.0f;
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
inline long randomLong(const long long min, const int max) { return min + (xorshift32() % (max - min + 1)); }
inline float randomFloat(float min, float max) { return min + (max - min) * (xorshift32() / 4294967295.0f); }
inline bool randomBool() { return (xorshift32() & 1) == 0; }

// Averages
inline short averageChar(vector<unsigned char>& chars) {
    short average;
    for (const short a : chars) { average += a; }
    return { average / chars.size() };
}

inline short averageShort(vector<short>& shorts) {
    int average;
    for (const short a : shorts) { average += a; }
    return { average / shorts.size() };
}

inline int averageInt(vector<int>& ints) {
    long long average;
    for (const int a : ints) { average += a; }
    return { average / ints.size() };
}

inline long long averageLong(vector<long long>& longs) {
    long long average;
    for (const long long a : longs) { average += a; }
    return { average / longs.size() };
}

inline float averageFloat(vector<float>& floats) {
    float average;
    for (const float a : floats) { average += a; }
    return { average / floats.size() };
}

inline bool averageBool(const vector<bool>& bools) {
    int averageTrue = 0, averageFalse = 0;
    for (bool a : bools) {
        if (a) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return randomBool();
}

inline short averageMinMaxChar(const vector<unsigned char>& chars) {
    unsigned char minVal = 0, maxVal = 0;
    short sum;

    for (const short a : chars) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : chars) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / chars.size();
}

inline short averageMinMaxShort(const vector<short>& shorts) {
    short minVal = 0, maxVal = 0;
    int sum;

    for (const short a : shorts) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : shorts) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / shorts.size();
}

inline int averageMinMaxInt(const vector<int>& ints) {
    int minVal = 0, maxVal = 0;
    long long sum;

    for (const int a : ints) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const int a : ints) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / ints.size();
}

inline long long averageMinMaxLong(const vector<long long>& longs) {
    long long minVal = 0, maxVal = 0, sum;

    for (const long long a : longs) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const long long a : longs) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / longs.size();
}

inline float averageMinMaxFloat(const vector<float>& floats) {
    float minVal = 0.f, maxVal = 0.f, sum;

    for (const float a : floats) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const float a : floats) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / floats.size();
}

// Values
inline void setValuesInRangeChar(vector<unsigned char>& vec, const unsigned char value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeShort(vector<short>& vec, const short value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeInt(vector<int>& vec, const int value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeLong(vector<long long>& vec, const long long value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setValuesInRangeFloat(vector<float>& vec, const float value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
inline void setZeroChar(vector<unsigned char>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count); }
inline void setZeroShort(vector<short>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 2); }
inline void setZeroInt(vector<int>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }
inline void setZeroLong(vector<long long>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 8); }
inline void setZeroFloat(vector<float>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }

inline void setTrue(vector<bool>& vec, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, true); }
inline void setFalse(vector<bool>& vec, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, false); }

#endif