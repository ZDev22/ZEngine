#define STB_TRUETYPE_IMPLEMENTATION
#include "../deps/stb_truetype.h"

#include "font.hpp"
#include "pipeline.hpp"
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::vector<unsigned char> loadTTF(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) { throw std::runtime_error("Failed to open font file: " + filepath); }

    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

std::unique_ptr<Texture> createFontTexture(Device& device, Pipeline& pipeline, const std::string& ttfPath, float pixelHeight, int atlasSize, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool) {
    std::vector<unsigned char> ttfData = loadTTF(ttfPath);
    std::vector<unsigned char> grayscale(atlasSize * atlasSize);
    stbtt_bakedchar charData[96];

    int result = stbtt_BakeFontBitmap(ttfData.data(), 0, pixelHeight, grayscale.data(), atlasSize, atlasSize, 32, 96, charData);
    if (result <= 0) {throw std::runtime_error("Failed to bake font bitmap."); }

    return std::make_unique<Texture>(device, grayscale.data(), atlasSize, descriptorSetLayout, descriptorPool, pipeline);
}