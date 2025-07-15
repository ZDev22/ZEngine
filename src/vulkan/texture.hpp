#pragma once

#include <string>
#include <vector>

#include <vulkan/vulkan.h>

class Device;
class Pipeline;

class Texture {
public:
    Texture(Device& device, const std::string& filepath, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    Texture(Device& device, const std::vector<std::string>& filepaths, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    Texture(Device& device, const unsigned char* pixelData, int size, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline);
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    VkImageView getImageView() { return imageView; }
    VkSampler getSampler() { return sampler; }
    VkImageLayout getImageLayout() { return imageLayout; }
    bool getIsArray() const { return isArray; }
    uint32_t getArrayLayers() const { return arrayLayers; }
    void createDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool);

private:
    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void createTextureArray(const std::vector<std::string>& filepaths);

    Device& device;
    Pipeline& pipeline;
    VkImageLayout imageLayout;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler sampler;
    VkFormat imageFormat;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDescriptorSet descriptorSet{};
    bool isArray{ false };
    uint32_t arrayLayers{ 1 };
    int texWidth, texHeight, texChannels;
    std::vector<void*> pixelsArray;
};