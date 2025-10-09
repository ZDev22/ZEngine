#pragma once

#include <string> 
#include <cstdio>
#include <cstdlib>

inline unsigned char* loadTTF(const std::string& filepath) {
    FILE* file = std::fopen(filepath.c_str(), "rb");
    if (!file) { throw("Failed to open font file"); }

    std::fseek(file, 0, SEEK_END);
    unsigned int fileSize = static_cast<unsigned int>(std::ftell(file));
    std::rewind(file);

    unsigned char* buffer = new unsigned char[fileSize];
    if (std::fread(buffer, 1, fileSize, file) != fileSize) {
        std::fclose(file);
        delete[] buffer;
        throw("Failed to read font file");
    }

    std::fclose(file);
    return buffer;
}