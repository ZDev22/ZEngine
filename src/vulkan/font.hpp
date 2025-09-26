#pragma once

#include "../deps/stb_truetype.h"
#include "device.hpp"
#include "texture.hpp"

#include <memory>

class Pipeline;

unsigned char* loadTTF(const std::string& filepath, unsigned int fileSize);
std::unique_ptr<Texture> createFontTexture(Device& device, Pipeline& pipeline, const std::string& ttfPath, float pixelHeight, int atlasSize, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, std::vector<stbtt_bakedchar>& outCharData);