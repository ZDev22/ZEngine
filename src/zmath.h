/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v3.6.12

#define ZMATH_IMPLEMENTATION - define in one c ONLY

zmath.h is a lightweight cross-platform single-header c library for high-preformance math functions!
Made for the purpose of gaming, like RNG and weight-table functions, as well as doing my homework.
*/

#ifndef ZMATH_H
#define ZMATH_H

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

// Interpolation
float interpolate(const float x, const float y, const float speed);
float remap(const float num, const float inputx, const float inputy, const float outputx, const float outputy);
float normalize(const float num, const float x, const float y);
float wrap(const float num, const float min, const float max);

// Random
void sRandom(void);
long long xorshift();
long long randomNum(const long long min, const long long max);
float randomF(const float min, const float max);
_Bool randomBool();

// Structs
typedef struct Vec2 { float x; float y; } Vec2;
typedef struct Vec3 { float x; float y; float z; } Vec3;
typedef struct Vec4 { float x; float y; float z; float w; } Vec4;

typedef struct WeightTableValue {
    unsigned int weight;
    int value;
    unsigned int ID;
    unsigned int offset;
} WeightTableValue;

typedef struct WeightTable {
    WeightTableValue* weights;
    unsigned int size;
} WeightTable;

// Weight table
void addWeightTableItem(WeightTable* table, unsigned int weight, int value);
void removeItem(WeightTable* table, unsigned int ID);
unsigned int getRandomValue(WeightTable* table);

// Averages
unsigned int average(const unsigned int* items, const unsigned int itemSize);
_Bool averageBool(const _Bool* bools, const unsigned int boolSize);
unsigned int averageMinMax(const unsigned int* items, const unsigned int itemSize);
float averageMinMaxF(const float* floats, const unsigned int floatSize);

// Advanced
_Bool prime(unsigned long long num);
const char* findFactor(signed char add, signed char multiply);

// Conversions
float radians(const float degrees);
float degrees(const float radians);
float celsius(const float fahrenheit);
float fahrenheit(const float celsius);

#ifdef ZMATH_IMPLEMENTATION

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Interpolation
float interpolate(const float x, const float y, const float speed) { return x + speed * (y - x); }
float remap(const float num, const float inputx, const float inputy, const float outputx, const float outputy) {  return (num - inputx) / (inputy - inputx) * (outputy - outputx) + outputx; }
float normalize(const float num, const float x, const float y) {  return (num - x) / (y - num); }
float wrap(const float num, const float min, const float max) { return num - (max - min) * floorf((num - min) / (max - min)); }

// Random
static long long zmath_state;
void sRandom(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    zmath_state = (unsigned long long)ts.tv_sec * 1000000000ULL + (unsigned long long)ts.tv_nsec;
}
long long xorshift() {
    zmath_state ^= zmath_state << 13;
    zmath_state ^= zmath_state >> 17;
    zmath_state ^= zmath_state << 5;
    return zmath_state;
}
long long randomNum(const long long min, const long long max) { return (long long)(min + (xorshift() % (max - min + 1))); }
float randomF(const float min, const float max) { return min + (max - min) * ((float)(xorshift()) / 4294967295.0f); }
_Bool randomBool() { return xorshift() & 2; }

void addWeightTableItem(WeightTable* table, unsigned int weight, int value) {
    WeightTableValue weightValue;
    weightValue.weight = weight;
    weightValue.value = value;
    weightValue.ID = table->size;
    weightValue.offset = 0;

    WeightTableValue* oldWeight = (WeightTableValue*)malloc(table->size * sizeof(WeightTableValue));
    memcpy(oldWeight, table->weights, table->size * sizeof(WeightTableValue));

    free(table->weights);
    table->weights = (WeightTableValue*)malloc((table->size + 1) * sizeof(WeightTableValue));
    memcpy(table->weights, oldWeight, table->size * sizeof(WeightTableValue));
    table->weights[table->size] = weightValue;
    table->size++;
}

void removeItem(WeightTable* table, unsigned int ID) {
    WeightTableValue* oldWeight = (WeightTableValue*)malloc(table->size * sizeof(WeightTableValue));
    memcpy(oldWeight, table->weights, table->size * sizeof(WeightTableValue));

    free(table->weights);
    table->weights = (WeightTableValue*)malloc((table->size - 1) * sizeof(WeightTableValue));

    unsigned int weightID = 0;
    for (unsigned int i = 0; i < table->size; i++) {
        if (i != ID) {
            table->weights[weightID] = oldWeight[i];
            table->weights[weightID].ID = weightID;
            weightID++;
        }
    }
    table->size--;
}

unsigned int getRandomValue(WeightTable* table) {
    unsigned long long offset = 0;
    for (unsigned int i = 0; i < table->size; i++) {
        table->weights[i].offset = offset;
        offset += table->weights[i].weight;
    }

    unsigned long long number = randomNum(0LL, offset);
    for (unsigned int i = 0; i < table->size; i++) { if (number >= table->weights[i].offset && number < table->weights[i].offset + table->weights[i].weight) { return table->weights[i].value; }}
    return 0;
}

// Averages
unsigned int average(const unsigned int* items, const unsigned int itemSize) {
    unsigned long long sum = 0;
    for (unsigned int i = 0; i < itemSize; i++) { sum += items[i]; }
    return (unsigned int)(sum / (unsigned long long)itemSize);
}
_Bool averageBool(const _Bool* bools, const unsigned int boolSize) {
    unsigned int average[2] = {0};
    for (unsigned int i = 0; i < boolSize; i++) {
        average[bools[i]]++;
    }
    return average[0] > average[1];
}
unsigned int averageMinMax(const unsigned int* items, const unsigned int itemSize) {
    unsigned int minVal = items[0];
    unsigned int maxVal = items[0];
    unsigned long long sum = 0;

    for (unsigned int i = 0; i < itemSize; i++) {
        if (items[i] < minVal) { minVal = items[i]; }
        else if (items[i] > maxVal) { maxVal = items[i]; }
        else { sum += items[i]; }
    }
    return (unsigned int)(sum / (unsigned long long)items);
}
float averageMinMaxF(const float* floats, const unsigned int floatSize) {
    float minVal = floats[0], maxVal = floats[0], sum = 0.f;

    for (unsigned int i = 0; i < floatSize; i++) {
        if (floats[i] < minVal) { minVal = floats[i]; }
        else if (floats[i] > maxVal) { maxVal = floats[i]; }
        else { sum += floats[i]; }
    }
    return sum / floatSize;
}

// Advanced
_Bool prime(unsigned long long num) {
    unsigned char divisors = 0;
    for (unsigned long long i = 1; i <= num; i++) {
        if (num % i == 0) {
            if (++divisors == 3) { return 0; }
        }
    }
    return 1;
}

const char* findFactor(signed char add, signed char multiply) {
    signed char x = 0;
    signed char y = 0;
    if (add > 0) {
        for (signed char i = add; i >= -add * 3; i--) {
            x = i;
            y = add - i;
            if (x + y == add && x * y == multiply) { goto factor; }
        }
        return "none";
    }
    else {
        for (signed char i = add; i <= -add * 3; i++) {
            x = i;
            y = add - i;
            if (x + y == add && x * y == multiply) { goto factor; }
        }
        return "none";
    }
    factor:
    char* result = (char*)malloc(20);
    snprintf(result, 20, "x = %d | x = %d", -x, -y);
    return (const char*)result;
}

// Conversions
float radians(const float degrees) { return degrees * ZMATH_PIR; }
float degrees(const float radians) { return radians * ZMATH_PID; }
float celsius(const float fahrenheit) { return (fahrenheit - 32.f) * .5555555555f; }
float fahrenheit(const float celsius) { return (celsius / .5555555555f) + 32.f; }

#endif // ZMATH_IMPLEMENTATION
#endif // ZMATH_H

