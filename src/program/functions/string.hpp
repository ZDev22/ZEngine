#pragma once

#include <string>
#include <vector>

using namespace std;

int getItemOfStringFromVector(const string& target, const vector<string>& list) {
    for (int i = 0; i < list.size(); i++) { if (list[i] == target) { return i; }}
    return 0;
}