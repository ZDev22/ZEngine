#define STB_TRUETYPE_IMPLEMENTATION
#include "../deps/stb_truetype.h"

#include "font.hpp"
#include "pipeline.hpp"

unsigned char* loadTTF(const std::string& filepath, size_t& fileSize) {
    FILE* file = std::fopen(filepath.c_str(), "rb");
    if (!file) { throw("Failed to open font file: " + filepath); }

    std::fseek(file, 0, SEEK_END);
    fileSize = static_cast<size_t>(std::ftell(file));
    std::rewind(file);

    unsigned char* buffer = new unsigned char[fileSize];
    if (std::fread(buffer, 1, fileSize, file) != fileSize) {
        std::fclose(file);
        delete[] buffer;
        throw("Failed to read font file: " + filepath);
    }

    std::fclose(file);
    return buffer;
}

std::unique_ptr<Texture> createFontTexture(Device& device, Pipeline& pipeline, const std::string& ttfPath, float pixelHeight, int atlasSize, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, std::vector<stbtt_bakedchar>& outCharData) {
    size_t fileSize = 0;
    unsigned char* ttfData = loadTTF("assets/fonts/" + ttfPath, fileSize);

    std::vector<unsigned char> grayscale(atlasSize * atlasSize);
    stbtt_bakedchar charData[96];

    int result = stbtt_BakeFontBitmap(ttfData, 0, pixelHeight, grayscale.data(), atlasSize, atlasSize, 32, 96, charData);
    delete[] ttfData;

    if (result <= 0) throw("Failed to bake font bitmap.");

    outCharData.assign(charData, charData + 96);

    return std::make_unique<Texture>(device, grayscale.data(), atlasSize, descriptorSetLayout, descriptorPool, pipeline);
}