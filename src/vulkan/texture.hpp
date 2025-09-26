#pragma once

#include <vulkan/vulkan.h>

#include <vector>

class Device;
class Pipeline;
class Texture {
public:
    Texture(Device& device, const char*, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    Texture(Device& device, const unsigned char* pixelData, int size, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);

    VkImageView getImageView() { return imageView; }
    VkSampler getSampler() { return sampler; }

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
    VkDescriptorSet descriptorSet{};
    unsigned int arrayLayers {1};
    int texWidth, texHeight, texChannels;
    std::vector<void*> pixelsArray;
};