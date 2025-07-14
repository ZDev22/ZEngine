#include "texture.hpp"
#include "pipeline.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
#include "stb_image.h"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <filesystem>

static void createTextureSampler(const Device& device, VkSampler& sampler) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST; // Changed to NEAREST for sharp pixels
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) { throw std::runtime_error("failed to create texture sampler!"); }
}

Texture::Texture(Device& device, const std::string& filepath, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline) : device(device), pipeline(pipeline), imageLayout(VK_IMAGE_LAYOUT_UNDEFINED),
    image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE),
    sampler(VK_NULL_HANDLE), descriptorSet(VK_NULL_HANDLE), isArray(false), arrayLayers(1) {

    stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    if (!pixels) { throw std::runtime_error("failed to load texture image: " + filepath); }

    if (texWidth == 0 || texHeight == 0) {
        stbi_image_free(pixels);
        throw std::runtime_error("Image has invalid size (zero width or height): " + filepath);
    }

    std::cout << "Texture loaded: " << filepath << ", Width: " << texWidth << ", Height: " << texHeight << ", Channels: " << texChannels << std::endl;

    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (imageSize == 0) {
        stbi_image_free(pixels);
        throw std::runtime_error("Image size is zero: " + filepath);
    }

    imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    device.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(device.device(), stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(device.device(), stagingBufferMemory);

    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(texWidth);
    imageInfo.extent.height = static_cast<uint32_t>(texHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = imageFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device.copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1);

    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
    vkFreeMemory(device.device(), stagingBufferMemory, nullptr);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = imageFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) { throw std::runtime_error("failed to create texture image view!"); }
    createTextureSampler(device, sampler);
}

Texture::Texture(Device& device, const std::vector<std::string>& filepaths, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool, Pipeline& pipeline) : device(device), pipeline(pipeline), imageLayout(VK_IMAGE_LAYOUT_UNDEFINED), image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE), sampler(VK_NULL_HANDLE), descriptorSet(VK_NULL_HANDLE), isArray(true) { createTextureArray(filepaths); }
Texture::~Texture() {
    vkDestroySampler(device.device(), sampler, nullptr); sampler = VK_NULL_HANDLE;
    vkDestroyImageView(device.device(), imageView, nullptr); imageView = VK_NULL_HANDLE;
    vkDestroyImage(device.device(), image, nullptr); image = VK_NULL_HANDLE;
    vkFreeMemory(device.device(), imageMemory, nullptr); imageMemory = VK_NULL_HANDLE;
}

void Texture::createTextureArray(const std::vector<std::string>& filepaths) {
    stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
    bool stagingBufferCreated = false;

    arrayLayers = static_cast<uint32_t>(filepaths.size());
    if (arrayLayers == 0) { throw std::runtime_error("No texture file paths provided!"); }
    if (arrayLayers > device.properties.limits.maxImageArrayLayers) { throw std::runtime_error("Texture array layers (" + std::to_string(arrayLayers) + ") exceed device limit (" + std::to_string(device.properties.limits.maxImageArrayLayers) + ")"); }

    std::vector<stbi_uc*> pixelsArray(arrayLayers, nullptr);
    std::vector<bool> padded(arrayLayers, false);
    int texWidth = 0, texHeight = 0, maxWidth = 0, maxHeight = 0;
    for (const auto& filepath : filepaths) {
        int width = 0, height = 0, channels = 0;
        if (!stbi_info(filepath.c_str(), &width, &height, &channels)) { throw std::runtime_error("Failed to query image info: " + filepath); }
        if (width > maxWidth) maxWidth = width;
        if (height > maxHeight) maxHeight = height;
    }

    for (uint32_t i = 0; i < arrayLayers; ++i) {
        std::string absolutePath = std::filesystem::absolute(filepaths[i]).string();
        std::cout << "Loading texture: " << absolutePath << std::endl;
        stbi_set_flip_vertically_on_load(true);
        int width = 0, height = 0, channels = 0;
        stbi_uc* loaded = stbi_load(absolutePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (!loaded || width <= 0 || height <= 0) {
            std::cerr << "Failed to load or invalid texture: " << absolutePath << std::endl;
            for (uint32_t j = 0; j < i; ++j) {
                if (pixelsArray[j]) {
                    if (padded[j]) { delete[] pixelsArray[j]; }
                    else { stbi_image_free(pixelsArray[j]); }
                }
            }
            throw std::runtime_error("Failed to load or invalid texture: " + absolutePath);
        }

        std::cout << "Loaded texture: " << absolutePath << ", Width: " << width << ", Height: " << height << ", Channels: " << channels << std::endl;

        if (i == 0) { texWidth = maxWidth; texHeight = maxHeight; }
        if (width == maxWidth && height == maxHeight) { pixelsArray[i] = loaded; } 
        else {
            stbi_uc* paddedPixels = new stbi_uc[maxWidth * maxHeight * 4]();
            for (int row = 0; row < height; ++row) { memcpy(paddedPixels + row * maxWidth * 4, loaded + row * width * 4, width * 4); }
            pixelsArray[i] = paddedPixels;
            padded[i] = true;
            stbi_image_free(loaded);
            std::cout << "Padded texture: " << absolutePath << " to " << maxWidth << "x" << maxHeight << std::endl;
        }
    }

    VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4 * arrayLayers;
    if (imageSize == 0) {
        for (uint32_t i = 0; i < arrayLayers; ++i) {
            if (pixelsArray[i]) {
                if (padded[i]) { delete[] pixelsArray[i]; }
                else { stbi_image_free(pixelsArray[i]); }
            }
        }
        throw std::runtime_error("Calculated image size is zero!");
    }

    imageFormat = VK_FORMAT_R8G8B8A8_SRGB;

    VkResult result = device.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    if (result != VK_SUCCESS) {
        for (uint32_t i = 0; i < arrayLayers; ++i) {
            if (pixelsArray[i]) {
                if (padded[i]) { delete[] pixelsArray[i]; }
                else { stbi_image_free(pixelsArray[i]); } 
            }
        }
        throw std::runtime_error("Failed to create staging buffer: VkResult " + std::to_string(result));
    }
    stagingBufferCreated = true;

    void* data;
    result = vkMapMemory(device.device(), stagingBufferMemory, 0, imageSize, 0, &data);
    if (result != VK_SUCCESS) {
        for (uint32_t i = 0; i < arrayLayers; ++i) {
            if (pixelsArray[i]) {
                if (padded[i]) { delete[] pixelsArray[i]; }
                else { stbi_image_free(pixelsArray[i]); }
            }
        }
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
        throw std::runtime_error("Failed to map staging buffer memory: VkResult " + std::to_string(result));
    }

    for (uint32_t i = 0; i < arrayLayers; ++i) {
        size_t expectedSize = static_cast<size_t>(texWidth) * texHeight * 4;
        memcpy(static_cast<char*>(data) + i * expectedSize, pixelsArray[i], expectedSize);
        if (pixelsArray[i]) {
            if (padded[i]) { delete[] pixelsArray[i]; }
            else { stbi_image_free(pixelsArray[i]); }
            pixelsArray[i] = nullptr;
        }
    }
    vkUnmapMemory(device.device(), stagingBufferMemory);

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(texWidth);
    imageInfo.extent.height = static_cast<uint32_t>(texHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = imageFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    result = device.createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);
    if (result != VK_SUCCESS) {
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
        throw std::runtime_error("Failed to create image: VkResult " + std::to_string(result));
    }

    transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    device.copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), arrayLayers);
    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
    vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
    stagingBufferCreated = false;

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    viewInfo.format = imageFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = arrayLayers;

    if (vkCreateImageView(device.device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) { throw std::runtime_error("Failed to create texture array image view!"); }
    createTextureSampler(device, sampler);

    isArray = true;
}

void Texture::createDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(device.device(), &allocInfo, &descriptorSet) != VK_SUCCESS) { throw std::runtime_error("failed to allocate descriptor sets!"); }
    std::cout << "Texture descriptor set allocated: " << descriptorSet << std::endl;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device.device(), 1, &descriptorWrite, 0, nullptr);
}

void Texture::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = arrayLayers;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 
    else { throw std::invalid_argument("unsupported layout transition!"); }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    device.endSingleTimeCommands(commandBuffer);
    imageLayout = newLayout;
}