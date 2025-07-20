#pragma once

#include <string>
#include <vector>

using namespace std;

inline int vectorContainsString(const string& target, const vector<string>& list) {
    for (int i = 0; i < list.size(); i++) { if (list[i] == target) { return i; }}
    return 0;
}

inline bool stringContainsItem(const std::string& str, const std::string& item) {
    for (size_t i = 0; i < str.size() - item.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < item.size(); j++) { if (str[i + j] != item[j]) { found = false; break; }}
        if (found) { return true; }
    }
    return false;
}

inline bool stringStartsWithItem(const std::string& str, const std::string& item) {
    bool found = true;
    for (size_t i = 0; i < item.size(); i++) { if (str[0] != item[0]) { found = false; break; }}
    return found;
}

inline bool stringEndsWithItem(const std::string& str, const std::string& item) {
    int start = str.size() - item.size();
    if (start < 0) { return 0; }

    bool found = true;
    for (size_t i = start; i < item.size(); i++) { if (str[0] != item[0]) { found = false; break; }}
    return found;
}

inline int itemIndexInString(const std::string& str, const std::string& item) {
    bool found = false;
    int index = 0, collision = 0;

    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == item[collision]) {
            index = i;
            collision++;
        }
        else { collision = 0; }
    }
    return index;
}