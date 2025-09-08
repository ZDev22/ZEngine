#pragma once

inline size_t stringLength(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') len++;
    return len;
}

inline int strCompare(const char* a, const char* b) {
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

inline const char* stringContainsItem(const char* str, const char* sub) {
    if (!*sub) return str;
    for (; *str; str++) {
        const char* s = str;
        const char* p = sub;
        while (*s && *p && *s == *p) {
            s++;
            p++;
        }
        if (!*p) return str;
    }
    return nullptr;
}

inline int vectorContainsString(const char* target, const char** list, int listSize) {
    for (int i = 0; i < listSize; i++) {
        if (strCompare(list[i], target) == 0) { return i; }
    }
    return -1;
}

inline bool stringStartsWithItem(const char* str, const char* item) {
    size_t i = 0;
    while (item[i] != '\0') {
        if (str[i] != item[i]) return false;
        i++;
    }
    return true;
}

inline bool stringEndsWithItem(const char* str, const char* item) {
    size_t strLen = stringLength(str);
    size_t itemLen = stringLength(item);
    if (itemLen > strLen) return false;

    size_t start = strLen - itemLen;
    for (size_t i = 0; i < itemLen; i++) {
        if (str[start + i] != item[i]) return false;
    }
    return true;
}

inline int itemIndexInString(const char* str, const char* item) {
    size_t strLen = stringLength(str);
    size_t itemLen = stringLength(item);

    if (itemLen == 0 || itemLen > strLen) return -1;

    for (size_t i = 0; i <= strLen - itemLen; i++) {
        bool match = true;
        for (size_t j = 0; j < itemLen; j++) {
            if (str[i + j] != item[j]) {
                match = false;
                break;
            }
        }
        if (match) return (int)i;
    }
    return -1;
}