#pragma once

#include "../deps/stb_truetype.h"
#include "device.hpp"
#include "texture.hpp"

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>

class Pipeline;

std::vector<unsigned char> loadTTF(const std::string& filepath);
std::unique_ptr<Texture> createFontTexture(Device& device, Pipeline& pipeline, const std::string& ttfPath, float pixelHeight, int atlasSize, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, std::vector<stbtt_bakedchar>& outCharData);