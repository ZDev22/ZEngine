#ifndef MATH_HPP
#define MATH_HPP

#define PI 3.1415926535

#include <cmath>
#include <array>
#include <vector>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4244)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnarrowing"
#endif

inline namespace {

// Global functions
float clamp(float value, float minValue, float maxValue) {
    if (value < minValue) { return minValue; }
    if (value > maxValue) { return maxValue; }
    return value;
}

// Interpolation
float lerp(float a, float b, float t) { return a + (b - a) * t; }
float smoothstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3 - 2 * t);
}

float smootherstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) { return edge0; }
    float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float easeInSine(float t) { return 1 - cos((t * PI) / 2); }
float easeOutSine(float t) { return sin((t * PI) / 2); }
float easeInOutSine(float t) { return -(cos(PI * t) - 1) / 2; }
float easeInExpo(float t) { return (t == 0) ? 0 : pow(2, 10 * (t - 1)); }
float easeOutExpo(float t) { return (t == 1) ? 1 : 1 - pow(2, -10 * t); }
float easeInOutExpo(float t) {
    if (t == 0 || t == 1) { return t; }
    if (t < .5f) { return pow(2, 20 * t - 10) / 2; }
    return (2 - pow(2, -20 * t + 10)) / 2;
}

float easeInCirc(float t) { return 1 - sqrt(1 - t * t); }
float easeOutCirc(float t) { return sqrt(1 - (t - 1) * (t - 1)); }
float easeInOutCirc(float t) {
    if (t < .5f) { return (1 - sqrt(1 - 4 * t * t)) / 2; }
    return (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}

float easeOutBounce(float t) {
    if (t < 4.f / 11.f) { return (121.f * t * t) / 16.f; } 
    else if (t < 8.f / 11.f) { return (363.f / 40.f * t * t) - (99.f / 10.f * t) + 17.f / 5.f; }
    else if (t < 9.f / 10.f) { return (4356.f / 361.f * t * t) - (35442 / 1805.f * t) + 16061.f / 1805.f; }
    return (54.f / 5.f * t * t) - (513.f / 25.f * t) + 268.f / 25.f;
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
short averageChar(std::vector<unsigned char>& chars) {
    short average = 0;
    for (const short a : chars) { average += a; }
    return average / chars.size();
}

short averageShort(std::vector<short>& shorts) {
    int average = 0;
    for (const short a : shorts) { average += a; }
    return average / shorts.size();
}

int averageInt(std::vector<int>& ints) {
    long long average = 0;
    for (const int a : ints) { average += a; }
    return average / ints.size();
}

long long averageLong(std::vector<long long>& longs) {
    long long average = 0;
    for (const long long a : longs) { average += a; }
    return average / longs.size();
}

float averageFloat(std::vector<float>& floats) {
    float average = 0;
    for (const float a : floats) { average += a; }
    return average / floats.size();
}

bool averageBool(const std::vector<bool>& bools) {
    int averageTrue = 0, averageFalse = 0;
    for (bool a : bools) {
        if (a) { averageTrue++; }
        else { averageFalse++; }
    }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return randomBool();
}

short averageMinMaxChar(const std::vector<unsigned char>& chars) {
    unsigned char minVal = chars[0], maxVal = chars[0];
    short sum = 0;

    for (const short a : chars) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : chars) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / chars.size();
}

short averageMinMaxShort(const std::vector<short>& shorts) {
    short minVal = shorts[0], maxVal = shorts[0];
    int sum = 0;

    for (const short a : shorts) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const short a : shorts) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / shorts.size();
}

int averageMinMaxInt(const std::vector<int>& ints) {
    int minVal = ints[0], maxVal = ints[0];
    long long sum = 0;

    for (const int a : ints) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const int a : ints) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / ints.size();
}

long long averageMinMaxLong(const std::vector<long long>& longs) {
    long long minVal = longs[0], maxVal = longs[0], sum = 0;

    for (const long long a : longs) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const long long a : longs) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / longs.size();
}

float averageMinMaxFloat(const std::vector<float>& floats) {
    float minVal = floats[0], maxVal = floats[0], sum = 0;

    for (const float a : floats) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const float a : floats) { if (a != minVal && a != maxVal) { sum += a; } }
    return sum / floats.size();
}

// Values
void setValuesInRangeChar(std::vector<unsigned char>& vec, const unsigned char value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeShort(std::vector<short>& vec, const short value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeInt(std::vector<int>& vec, const int value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeLong(std::vector<long long>& vec, const long long value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setValuesInRangeFloat(std::vector<float>& vec, const float value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
void setZeroChar(std::vector<unsigned char>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count); }
void setZeroShort(std::vector<short>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 2); }
void setZeroInt(std::vector<int>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }
void setZeroLong(std::vector<long long>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 8); }
void setZeroFloat(std::vector<float>& vec, size_t startIndex, size_t count) { memset(vec.data() + startIndex, 0, count * 4); }

void setTrue(std::vector<bool>& vec, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, true); }
void setFalse(std::vector<bool>& vec, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, false); }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

#endif