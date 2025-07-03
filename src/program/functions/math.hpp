#ifndef MATH_HPP
#define MATH_HPP

#include "deps/exprtk.hpp"

#include <cmath>
#include <array>
#include <vector>
#include <limits>
#include <string>
#include <cstdint>
#include <cstring>
#include <climits>
#include <algorithm>

using namespace std;

// Interpolation
float lerp(float& a, float& b, float& t) { return a + (b - a) * t; }
float smoothstep(float& edge0, float& edge1, float& x) { float t = clamp((x - edge0) / (edge1 - edge0), .0f, 1.f); return t * t * (3 - 2 * t); }
float smootherstep(float& edge0, float& edge1, float& x) { float t = clamp((x - edge0) / (edge1 - edge0), .0f, 1.f); return t * t * t * (t * (t * 6 - 15) + 10); }
float easeInSine(float& t) { return 1 - cos((t * 3.14159265358979323846) / 2); }
float easeOutSine(float& t) { return sin((t * 3.14159265358979323846) / 2); }
float easeInOutSine(float& t) { return -(cos(3.14159265358979323846 * t) - 1) / 2; }
float easeInExpo(float& t) { return (t == 0) ? 0 : pow(2, 10 * (t - 1)); }
float easeOutExpo(float& t) { return (t == 1) ? 1 : 1 - pow(2, -10 * t); }
float easeInOutExpo(float& t) {
    if (t == 0) return 0;
    if (t == 1) return 1;
    if (t < .5f) return pow(2, 20 * t - 10) / 2;
    return (2 - pow(2, -20 * t + 10)) / 2;
}
float easeInCirc(float& t) { return 1 - sqrt(1 - t * t); }
float easeOutCirc(float& t) { return sqrt(1 - (t - 1) * (t - 1)); }
float easeInOutCirc(float& t) { if (t < .5f) return (1 - sqrt(1 - 4 * t * t)) / 2; return (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2; }
float easeOutBounce(float& t) {
    if (t < 4 / 11.f) { return (121 * t * t) / 16.f; } 
    else if (t < 8 / 11.f) { return (363 / 40.f * t * t) - (99 / 10.f * t) + 17 / 5.f; }
    else if (t < 9 / 10.f) { return (4356 / 361.f * t * t) - (35442 / 1805.f * t) + 16061 / 1805.f; }
    return (54 / 5.f * t * t) - (513 / 25.f * t) + 268 / 25.f;
}

// Random
uint32_t state = 730182364;
uint32_t xorshift32() {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

template<typename T>
T randomNum(const T& min, const T& max) { return min + (xorshift32() % (max - min + 1)); }
float randomFloat(float min, float max) { return min + (max - min) * (xorshift32() / 4294967295.0f); }
bool randomBool() { return (xorshift32() & 1) == 0; }

// Averages
template<typename T>
T averageNum(const vector<T>& nums) {
    double average = 0.0;
    for (const T& a : nums) { average += a; }
    return static_cast<T>(average / nums.size());
}

bool averageBool(vector<bool>& bools) {
    int averageFalse = 0, averageTrue = 0;

    for (const int a : bools) { if (bools[a]) { averageTrue++; } else { averageFalse++; } }
    if (averageTrue > averageFalse) { return true; }
    if (averageFalse > averageTrue) { return false; }
    return randomBool();
}

template<typename T>
T averageMinMaxNum(const vector<T>& nums) {
    double minVal = 0.0, maxVal = 0, average = 0.0;
    for (const T& a : nums) {
        if (a < minVal) { minVal = a; }
        else if (a > maxVal) { maxVal = a; }
    }
    for (const T& a : nums) { if (a != minVal && a != maxVal) { average += a; } }
    return static_cast<T>(average / nums.size());
}

// Solve
float solve(const std::string& expression) {
    typedef exprtk::symbol_table<float> symbol_table_t;
    typedef exprtk::expression<float> expression_t;
    typedef exprtk::parser<float> parser_t;

    symbol_table_t symbol_table;
    expression_t expr;
    expr.register_symbol_table(symbol_table);

    parser_t parser;
    if (!parser.compile(expression, expr)) { return 0.f; }
    return expr.value();
}

float solveWithVariables(const std::string& expression, const std::map<std::string, float>& variables) {
    typedef exprtk::symbol_table<float> symbol_table_t;
    typedef exprtk::expression<float> expression_t;
    typedef exprtk::parser<float> parser_t;

    symbol_table_t symbol_table;

    for (const auto& var : variables) { symbol_table.add_variable(var.first, const_cast<float&>(var.second)); }

    expression_t expr;
    expr.register_symbol_table(symbol_table);

    parser_t parser;
    if (!parser.compile(expression, expr)) { return 0.f; }
    return expr.value();
}

double solvePrecise(const std::string& expression) {
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    symbol_table_t symbol_table;
    expression_t expr;
    expr.register_symbol_table(symbol_table);

    parser_t parser;
    if (!parser.compile(expression, expr)) { return 0.0; }
    return expr.value();
}

double solveWithVariablesPrecise(const std::string& expression, const std::map<std::string, double>& variables) {
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;

    symbol_table_t symbol_table;

    for (const auto& var : variables) { symbol_table.add_variable(var.first, const_cast<double&>(var.second)); }

    expression_t expr;
    expr.register_symbol_table(symbol_table);

    parser_t parser;
    if (!parser.compile(expression, expr)) { return 0.0; }
    return expr.value();
}

// Values
template<typename T>
void setValuesInRange(std::vector<T>& vec, const T& value, int minIndex, int maxIndex) { std::fill(vec.begin() + minIndex, vec.begin() + maxIndex, value); }
template<typename T>
void setZero(std::vector<T>& vec, size_t startIndex, size_t count) { std::memset(vec.data() + startIndex, 0, count * sizeof(T)); }

#endif