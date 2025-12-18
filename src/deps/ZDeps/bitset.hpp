/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/ for current license

v2.3.2

bitset.hpp is a lightweight cross-platform single-header cpp library for creating large vectors of 1's and 0's!
Aims to be an alternative to <bitset> with more features and customizability to come!

HOW TO USE: Bitset bits(16) - creates a bitset with 16 variables

#define BITSET_NO_INCLUDE - Decreases compile times but enables less features and slower alternatives
#define BITSET_SET_SIZE - Controls the size of the bitset if BITSET_NO_INCLUDE is enabled (default = 32)
#define BITSET_DYNAMIC_SIZE - Gives the bitset the ability to dynamically allocate more memory
#define BITSET_VARIABLE_TYPE - Stores the bits in whatever variable type you want
#define BITSET_USE_UINT - Apparently people prefer uints, so this option is here.
*/

#pragma once

#ifndef BITSET_NO_INCLUDE
    #include <stdlib.h>
    #include <string.h>
    #ifdef BITSET_DYNAMIC_SIZE
        #include <vector>
    #endif
#endif

#ifndef BITSET_SET_SIZE
    #define BITSET_SET_SIZE 32
#endif

#ifndef BITSET_VARIABLE_TYPE
    #if defined(BITSET_USE_UINT) && !defined(BITSET_NO_INCLUDE)
        #include <stdint.h>
        #define BITSET_VARIABLE_TYPE uint32_t
    #else
        #define BITSET_VARIABLE_TYPE unsigned int
    #endif
#endif

#ifdef __clang__ /* clang dosen't like constexpr for some reason */
    #define BITSET_CONSTEXPR 
#else
    #define BITSET_CONSTEXPR constexpr
#endif

struct Bitset {
    #if !defined(BITSET_NO_INCLUDE) && defined(BITSET_DYNAMIC_SIZE)
        std::vector<BITSET_VARIABLE_TYPE> data;

        BITSET_CONSTEXPR Bitset(BITSET_VARIABLE_TYPE amount) {
            data.reserve(amount);
            for (BITSET_VARIABLE_TYPE i = 0; i < amount; i++) { data.push_back(0); }
        }
    #else
        BITSET_VARIABLE_TYPE data[BITSET_SET_SIZE] = {0};
        BITSET_CONSTEXPR Bitset() {}
        BITSET_CONSTEXPR Bitset(BITSET_VARIABLE_TYPE amount) {} /* Takes an input to be compatible if you are switching between defining BITSET_NO_INCLUDE and not defining it */
    #endif
    #ifdef BITSET_NO_INCLUDE
        void on() { memset(&data, 1, (data.size() + 63) / 64); } // turn ALL bits on
        void off() { memset(&data, 0, (data.size() + 63) / 64); } // turn ALL bits off
    #else
        void on() { for (BITSET_VARIABLE_TYPE i = 0; i < BITSET_SET_SIZE; i++) { data[i] = (BITSET_VARIABLE_TYPE)18446744073709551615ULL; }} // the max # a uint64 can be. As it is 111111111, converting it to a less amount of bits should still be the max that variable type can hold
        void off() { for (BITSET_VARIABLE_TYPE i = 0; i < BITSET_SET_SIZE; i++) { data[i] = 0; }};
    #endif
    void set(BITSET_VARIABLE_TYPE pos) { data[pos / 64] |= (1ULL << (pos % 64)); } // set a bit to 1
    void reset(BITSET_VARIABLE_TYPE pos) { data[pos / 64] &= ~(1ULL << (pos % 64)); } // set a bit to 0
    void flip(BITSET_VARIABLE_TYPE pos) { data[pos / 64] ^= (1ULL << (pos % 64)); } // flip a bit
    bool check(BITSET_VARIABLE_TYPE pos) const { return data[pos / 64] & (1ULL << (pos % 64)); } // check a bit's value

    BITSET_VARIABLE_TYPE valueOf(BITSET_VARIABLE_TYPE pos) { return data[pos]; }
    void clearFirstSetBit() { /*learned this trick in "The Advent of Compiler Optimizations"*/
        for (auto bit : data) {
            if (bit != 0) { bit &= bit - 1; }
        }
    }
    void setFirstUnsetBit() {
        for (auto bit : data) {
            if (bit != (BITSET_VARIABLE_TYPE)18446744073709551615ULL) { bit &= bit + 1; }
        }
    }

    #if !defined(BITSET_NO_INCLUDE) && defined(BITSET_DYNAMIC_SIZE)
        void add() { data.push_back(0); } // add more bits
        void remove() { data.erase(data.begin() + data.size()); } // remove bits
        std::vector<BITSET_VARIABLE_TYPE> value() { return data; } // Get the vector

        const char* toString() const { // get the bitset in a const char*
            char* buffer = (char*)malloc(data.size() + 1);
            for (BITSET_VARIABLE_TYPE i = 0; i < data.size(); i++) { buffer[i] = check(data.size() - i - 1) ? '1' : '0'; }
            buffer[data.size()] = '\0';
            return buffer;
        }
    #else
        BITSET_VARIABLE_TYPE* value() { return data; } // get the array
        const char* toString() const { // get the bitset in a const char*
            static char buffer[BITSET_SET_SIZE];
            for (BITSET_VARIABLE_TYPE i = 0; i < BITSET_SET_SIZE; i++) { buffer[i] = check(BITSET_SET_SIZE - i - 1) ? '1' : '0'; }
            buffer[BITSET_SET_SIZE - 1] = '\0';
            return buffer;
        }
    #endif
};