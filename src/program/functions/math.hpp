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
float clamp(float value, float minValue, float maxValue) { return max(minValue, min(maxValue, value)); }

// Interpolation
float lerp(float a, float b, float t) { return a + (b - a) * t; }
float smoothstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) return edge0;
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

float smootherstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) return edge0;
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float easeInSine(float t) { return 1 - cos((t * 3.14159265358979323846) / 2); }
float easeOutSine(float t) { return sin((t * 3.14159265358979323846) / 2); }
float easeInOutSine(float t) { return -(cos(3.14159265358979323846 * t) - 1) / 2; }
float easeInExpo(float t) { return (t == 0) ? 0 : pow(2, 10 * (t - 1)); }
float easeOutExpo(float t) { return (t == 1) ? 1 : 1 - pow(2, -10 * t); }
float easeInOutExpo(float t) {
    if (t == 0) return 0;
    if (t == 1) return 1;
    if (t < 0.5f) return pow(2, 20 * t - 10) / 2;
    return (2 - pow(2, -20 * t + 10)) / 2;
}

float easeInCirc(float t) { return 1 - sqrt(1 - t * t); }
float easeOutCirc(float t) { return sqrt(1 - (t - 1) * (t - 1)); }
float easeInOutCirc(float t) {
    if (t < 0.5f) return (1 - sqrt(1 - 4 * t * t)) / 2;
    return (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}

float easeOutBounce(float t) {
    if (t < 4 / 11.0f) { return (121 * t * t) / 16.0f; } 
    else if (t < 8 / 11.0f) { return (363 / 40.0f * t * t) - (99 / 10.0f * t) + 17 / 5.0f; }
    else if (t < 9 / 10.0f) { return (4356 / 361.0f * t * t) - (35442 / 1805.0f * t) + 16061 / 1805.0f; }
    return (54 / 5.0f * t * t) - (513 / 25.0f * t) + 268 / 25.0f;
}

// Random
static uint32_t state = 730182364;

uint32_t xorshift32() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

unsigned char randomChar(const char min, const char max) { return min + (xorshift32() % (max - min + 1)); }
short randomShort(const short min, const short max) { return min + (xorshift32() % (max - min + 1)); }
int randomInt(const int min, const int max) { return min + (xorshift32() % (max - min + 1)); }
long randomLong(const long long min, const int max) { return min + (xorshift32() % (max - min + 1)); }
float randomFloat(float min, float max) { return min + (max - min) * (xorshift32() / 4294967295.0f); }
bool randomBool() { return (xorshift32() & 1) == 0; }

// Averages
short averageChar(vector<unsigned char>& chars) {
    short average;
    for (const short a : chars) { average += a; }
    return { average / chars.size() };
}

short averageShort(vector<short>& shorts) {
    int average;
    for (const short a : shorts) { average += a; }
    return { average / shorts.size() };
}

int averageInt(vector<int>& ints) {
    long long average;
    for (const int a : ints) { average += a; }
    return { average / ints.size() };
}

long long averageLong(vector<long long>& longs) {
    long long average;
    for (const long long a : longs) { average += a; }
    return { average / longs.size() };
}

float averageFloat(vector<float>& floats) {
    float average;
    for (const float a : floats) { average += a; }
    return { average / floats.size() };
}

bool averageBool(const vector<bool>& bools) {
    int averageTrue = 0, averageFalse = 0;
    for (bool a : bools) {
        if (a) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return randomBool();
}

short averageMinMaxChar(const vector<unsigned char>& chars) {
    unsigned char minVal = 0, maxVal = 0;
    short sum;

    for (const short a : chars) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : chars) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / chars.size();
}

short averageMinMaxShort(const vector<short>& shorts) {
    short minVal = 0, maxVal = 0;
    int sum;

    for (const short a : shorts) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : shorts) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / shorts.size();
}

int averageMinMaxInt(const vector<int>& ints) {
    int minVal = 0, maxVal = 0;
    long long sum;

    for (const int a : ints) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const int a : ints) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / ints.size();
}

long long averageMinMaxLong(const vector<long long>& longs) {
    long long minVal = 0, maxVal = 0, sum;

    for (const long long a : longs) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const long long a : longs) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / longs.size();
}

float averageMinMaxFloat(const vector<float>& floats) {
    float minVal = 0.f, maxVal = 0.f, sum;

    for (const float a : floats) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const float a : floats) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / floats.size();
}

// Values
void setValuesInRangeChar(vector<unsigned char>& vec, const unsigned char value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeShort(vector<short>& vec, const short value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeInt(vector<int>& vec, const int value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeLong(vector<long long>& vec, const long long value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeFloat(vector<float>& vec, const float value, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setZeroChar(vector<unsigned char>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count); }
void setZeroShort(vector<short>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 2); }
void setZeroInt(vector<int>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }
void setZeroLong(vector<long long>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 8); }
void setZeroFloat(vector<float>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }

void setTrue(vector<bool>& vec, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, true); }
void setFalse(vector<bool>& vec, int minIndex, int maxIndex) { fill(vec.begin() + minIndex, vec.begin() + maxIndex, false); }

#endif