#pragma once

#include "vector.hpp"

struct StringIndex {
    unsigned int begin;
    unsigned int end;
};

inline unsigned int charLength(const char* str) {
    unsigned int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

inline int charCompare(const char* a, const char* b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

inline const char* stringContainsItem(const char* str, const char* sub) {
    if (!*sub) return str;
    for (; *str; str++) {
        const char* s = str;
        const char* p = sub;
        while (*s && *p && *s == *p) { s++; p++; }
        if (!*p) return str;
    }
    return nullptr;
}

inline int vectorContainsString(const char* target, const char** list, int listSize) { for (int i = 0; i < listSize; i++) { if (charCompare(list[i], target) == 0) return i; } return -1; }

inline bool stringStartsWithItem(const char* str, const char* item) {
    unsigned int i = 0;
    while (item[i] != '\0') { if (str[i] != item[i]) { return false; } i++; }
    return true;
}

inline bool stringEndsWithItem(const char* str, const char* item) {
    unsigned int strLen = charLength(str);
    unsigned int itemLen = stringLength(item);
    if (itemLen > strLen) return false;

    unsigned int start = strLen - itemLen;
    for (unsigned int i = 0; i < itemLen; i++) { if (str[start + i] != item[i]) return false; }
    return true;
}

inline int itemIndexInString(const char* str, const char* item) {
    unsigned int strLen = stringLength(str);
    unsigned int itemLen = stringLength(item);

    if (itemLen == 0 || itemLen > strLen) return -1;

    for (unsigned int i = 0; i <= strLen - itemLen; i++) {
        bool match = true;
        for (unsigned int j = 0; j < itemLen; j++) {
            if (str[i + j] != item[j]) { match = false; break; }
        }
        if (match) return (int)i;
    }
    return -1;
}

inline unsigned int howManyItemsInString(char item, const char* str, unsigned int strSize) {
    unsigned int items = 0;
    for (unsigned int i = 0; i < strSize; i++) { if (str[i] == item) { items++; }}
    return items;
}

inline vector<unsigned int> itemIndexsInString(char item, const char* str, unsigned int strSize) {
    vector<unsigned int> items;
    for (unsigned int i = 0; i < strSize; i++) { if (str[i] == item) { items.push_back(i); }}
    return items;
}

inline unsigned int howManyStringsInString(const char* item, unsigned int itemSize, const char* str, unsigned int strSize) {
    unsigned int items = 0;
    for (unsigned int i = 0; i < strSize; i++) {
        if (str[i] == item[i]) {
            for (unsigned int q = 1; q < itemSize; q++) { if (str[i + q] != item[i + q]) { break; }}
            if (q == itemSize) { items++; }
        }
    }
    return items;
}

inline vector<StringIndex> stringIndexsInString(const char* item, unsigned int itemSize, const char* str, unsigned int strSize) {
    vector<StringIndex> items = 0;
    for (unsigned int i = 0; i < strSize; i++) {
        if (str[i] == item[i]) {
            for (unsigned int q = 1; q < itemSize; q++) { if (str[i + q] != item[i + q]) { break; }}
            if (q == itemSize) {
                StringIndex strIndex;
                strIndex.begin = i;
                strIndex.end = i + q;
                items.push_back(strIndex);
            }
        }
    }
    return items;
}