#pragma once

#include "../deps/stb_truetype.h"
#include "device.hpp"
#include "texture.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <memory>

class Pipeline;
struct Font {
    stbtt_fontinfo info;
    std::vector<unsigned char> ttfBuffer;
    float scale;
    int ascent, descent, lineGap;
};

std::unique_ptr<Texture> createFontTexture(Device& device, Pipeline& pipeline, const std::string& ttfPath, float pixelHeight, int atlasSize, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);