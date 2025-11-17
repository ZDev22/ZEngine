#pragma once

#include "window.hpp"

#include <vector>
#include <string>

class Device;
class Pipeline;
struct Texture {
public:
    Texture(Device& device, const std::string& filepath, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    Texture(Device& device, const unsigned char* pixelData, unsigned int size, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    ~Texture();

    VkImageView getImageView() { return imageView; }
    VkSampler getSampler() { return sampler; }
    VkImageLayout getImageLayout() { return imageLayout; }
    unsigned int getArrayLayers() const { return arrayLayers; }

    int getTexWidth() const { return texWidth; }
    int getTexHeight() const { return texHeight; }

    std::string name;

private:
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

    Device& device;
    Pipeline& pipeline;
    VkImageLayout imageLayout;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler sampler;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    unsigned int arrayLayers {1};
    int texWidth, texHeight, texChannels;
    std::vector<void*> pixelsArray;
};