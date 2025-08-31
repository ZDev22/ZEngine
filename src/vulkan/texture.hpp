#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <string>

class Device;
class Pipeline;

class Texture {
public:
    Texture(Device& device, const std::string& filepath, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    Texture(Device& device, const unsigned char* pixelData, int size, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);

    VkImageView getImageView() { return imageView; }
    VkSampler getSampler() { return sampler; }
    VkImageLayout getImageLayout() { return imageLayout; }
    uint32_t getArrayLayers() const { return arrayLayers; }
    void createDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);

    int getTexWidth() const { return texWidth; }
    int getTexHeight() const { return texHeight; }

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
    uint32_t arrayLayers {1};
    int texWidth, texHeight, texChannels;
    std::vector<void*> pixelsArray;
};