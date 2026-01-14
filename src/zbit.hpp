/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/README.md for current license

v3.3.2

zbit.hpp is a lightweight cross-platform single-header cpp library for creating large vectors of 1's and 0's!
Aims to be an alternative to <bitset> with more features and customizability to come!

HOW TO USE (basics):
ZBitset bits           - creates a bitset with 16 variables
bits.value()           - returns an array of the value of the bitset
bits.string()          - returns the string of the bitset (const char*)
bits.data[0].check(3)  - checks a bit
bits.data[0].flip(3)   - flips a bit
bits.data[0].set(3)    - sets a bit
bits.data[0].clear(3)  - clears a bit
bits.data[0] = 15      - set the limb to a value
bits.data[0]           - the value of the limb

#define ZBIT_NO_INCLUDE - Decreases compile times but enables less features and slower alternatives
#define ZBIT_SET_SIZE - Controls the default of the bitset
#define ZBIT_DYNAMIC_SIZE - Gives the bitset the ability to dynamically allocate more memory
#define ZBIT_VARIABLE_TYPE - Stores the bits in whatever variable type you want
#define ZBIT_USE_UINT - Apparently people prefer uints, so this option is here.
*/

#ifndef ZBIT_HPP
#define ZBIT_HPP

#ifndef ZBIT_NO_INCLUDE /* if this is defined, the program will compile without and stdlib */
    #include <stdlib.h>
    #include <string.h>
    #ifdef ZBIT_DYNAMIC_SIZE
        #include <vector>
    #endif
#endif // ZBIT_NO_INCLUDE

#ifndef ZBIT_SET_SIZE /* the size of the bitset by default */
    #define ZBIT_SET_SIZE 32
#endif // ZBIT_SET_SIZE

#ifndef ZBIT_VARIABLE_TYPE /* the type of the variable that will be set */
    #if defined(ZBIT_USE_UINT) && !defined(ZBIT_NO_INCLUDE)
        #include <stdint.h>
        #define ZBIT_VARIABLE_TYPE uint32_t
    #else
        #define ZBIT_VARIABLE_TYPE unsigned int
    #endif
#endif // ZBIT_VARIABLE_TYPE

/* thanks stack overflow */
#define BIT_MASK(bit)         (1 << (bit))
#define SET_BIT(value, bit)   ((value) |= BIT_MASK(bit))
#define CLEAR_BIT(value, bit) ((value) &= ~BIT_MASK(bit))
#define CHECK_BIT(value, bit) ((value) & BIT_MASK(bit))

#define ZBIT_SIZE_OF(value)   (sizeof(value) * 8) /* find the amount of bits in a variable */

struct ZBit { /* the limbs of the bitset */
    ZBIT_VARIABLE_TYPE data = 0; /* the bits for the set */

    void set(unsigned char pos) { data |= (1 << pos); } /* set a bit to 1 */
    void reset(unsigned char pos) { data &= ~(1 << pos); } /* set a bit to 0 */
    void flip(unsigned char pos) { data ^= (1 << pos); } /* flip a bit */
    bool check(unsigned char pos) const { return data & (1 << pos); } /* check a bit's value */

    void clearFirstSetBit() { data &= data - 1; } /* learned this trick in "The Advent of Compiler Optimizations" set the first 1 bit to 0*/
    void setFirstUnsetBit() { data &= data + 1; } /* set the first 0 bit to 1*/
};

struct ZBitset { /* the bitset */
    #if !defined(ZBIT_NO_INCLUDE) && defined(ZBIT_DYNAMIC_SIZE)
        std::vector<ZBit> data(ZBIT_SET_SIZE); /* vector to store the bitset */

        void add() { data.push_back(0); } /* add more bits */
        void remove() { data.erase(data.begin() + data.size()); } // remove bits
        void on() { for (unsigned int i = 0; i < data.size(); i++) { data[i].data = (ZBIT_VARIABLE_TYPE)18446744073709551615ULL; }} /* turn all bits on */
        void off() { for (unsigned int i = 0; i < data.size(); i++) { data[i].data = 0; }}; /* turn all bits off */

        ZBIT_VARIABLE_TYPE* value() { /* Get the vector */
            ZBIT_VARIABLE_TYPE result[data.size()];
            for (unsigned int i = 0; i < data.size(); i++) { result[i] = data[i].value(); }
            return value;
        }

        const char* string() const { /* get the bitset in a const char* */
            char* buffer = (char*)malloc(data.size());
            for (unsigned int i = 0; i < data.size(); i++) {
                for (unsigned char j = 0; j < ZBIT_SIZE_OF(ZBIT_VARIABLE_TYPE); j++) {
                    buffer[i] = data[i].check(j) ? '1' : '0';
                }
            }
            buffer[data.size() * ZBIT_SIZE_OF(ZBIT_VARIABLE_TYPE)] = '\0';
            return buffer;
        }
    #else
        ZBit data[ZBIT_SET_SIZE]; /* array to store the bitset */

        void on() { for (unsigned int i = 0; i < ZBIT_SET_SIZE; i++) { data[i].data = (ZBIT_VARIABLE_TYPE)18446744073709551615ULL; }} /* the max # a uint64 can be. As it is 111111111, converting it to a less amount of bits should still be the max that variable type can hold */
        void off() { for (unsigned int i = 0; i < ZBIT_SET_SIZE; i++) { data[i].data = 0; }}; /* turn all bits off*/

        ZBIT_VARIABLE_TYPE* value() { /* Get the array */
            ZBIT_VARIABLE_TYPE result[ZBIT_SET_SIZE];
            for (unsigned int i = 0; i < ZBIT_SET_SIZE; i++) { result[i] = data[i].data; }
            return result;
        }

        const char* string() const { /* get the bitset in a const char* */
            char* buffer = (char*)malloc(ZBIT_SET_SIZE);
            for (unsigned int i = 0; i < ZBIT_SET_SIZE; i++) {
                for (unsigned char j = 0; j < ZBIT_SIZE_OF(ZBIT_VARIABLE_TYPE); j++) {
                    buffer[i] = data[i].check(j) ? '1' : '0';
                }
            }
            buffer[ZBIT_SET_SIZE * ZBIT_SIZE_OF(ZBIT_VARIABLE_TYPE)] = '\0';
            return buffer;
        }
    #endif // !ZBIT_NO_INCLUDE && ZBIT_DYNAMIC_SIZE
};

#endif // ZBIT_HPP