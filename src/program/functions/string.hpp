#pragma once

#include <string>
#include <vector>

using namespace std;

int getItemOfStringFromVector(const string& target, const vector<string>& list) {
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] == target) return static_cast<int>(i);
    }
    return 0;
}