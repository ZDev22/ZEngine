#pragma once

#include <string>
#include <vector>

using namespace std;

inline namespace {

int vectorContainsString(const string& target, const vector<string>& list) {
    for (int i = 0; i < list.size(); i++) { if (list[i] == target) { return i; }}
    return 0;
}

bool stringContainsItem(const std::string& str, const std::string& item) {
    for (size_t i = 0; i < str.size() - item.size(); i++) {
        bool found = true;
        for (size_t j = 0; j < item.size(); j++) { if (str[i + j] != item[j]) { found = false; break; }}
        if (found) { return true; }
    }
    return false;
}

bool stringStartsWithItem(const std::string& str, const std::string& item) {
    bool found = true;
    for (size_t i = 0; i < item.size(); i++) { if (str[0] != item[0]) { found = false; break; }}
    return found;
}

bool stringEndsWithItem(const std::string& str, const std::string& item) {
    int start = str.size() - item.size();
    if (start < 0) { return 0; }

    bool found = true;
    for (size_t i = start; i < item.size(); i++) { if (str[0] != item[0]) { found = false; break; }}
    return found;
}

int itemIndexInString(const std::string& str, const std::string& item) {
    bool found = false;
    int index = 0, collision = 0;

    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == item[collision]) {
            if (collision == 0) { index = i; }
            collision++;
            if (collision == item.size()) { break; }
        }
        else { collision = 0; }
    }
    return index;
}
}