#pragma once

#include "pipeline.hpp"
#include "sprite.hpp"
#include "buffer.hpp"

struct RenderSystem {
public:

    #ifdef __linux__
        inline RenderSystem(Device& device, AppWindow& window, Renderer& renderer, Push& vertex, Pipeline& pipeline) : pipeline(pipeline), renderer(renderer), device(device), window(window), vertex(vertex), descriptorSetLayout(pipeline.getDescriptorSetLayout()) {
    #else
        inline RenderSystem(Device& device, AppWindow& window, Renderer& renderer, Push& vertex, VkDescriptorSetLayout descriptorSetLayout) : pipeline(device, renderer, "texture"), renderer(renderer), device(device), window(window), vertex(vertex), descriptorSetLayout(descriptorSetLayout) {
    #endif
        createPipelineLayout();
        initializeSpriteData();
        createTextureArrayDescriptorSet();
    }

    inline ~RenderSystem() {
        spriteDataBuffer->unmap();
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
        vkFreeDescriptorSets(device.device(), pipeline.getDescriptorPool(), 1, &spriteDataDescriptorSet);
    }

    inline void createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(Push);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) { throw("failed to create pipeline layout!"); }
    }

    inline void initializeSpriteData() {
        VkDeviceSize bufferSize = sizeof(SpriteData) * MAX_SPRITES;
        spriteDataBuffer = std::make_unique<Buffer>(device, bufferSize, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        spriteDataBuffer->map();
        if (!sprites.empty()) { spriteDataBuffer->writeToBuffer(sprites.data(), sizeof(SpriteData) * sprites.size()); }
        vertex.camera[0] = 0.f;
        vertex.camera[1] = 0.f;
        vertex.cameraZoom[0] = 1.f;
        vertex.cameraZoom[1] = 1.f;
    }

    inline void createTextureArrayDescriptorSet() {
        std::vector<VkDescriptorImageInfo> imageInfos(MAX_TEXTURES);

        for (unsigned int i = 0; i < MAX_TEXTURES; i++) {
            Texture* texture = spriteTextures[i].get();
            VkDescriptorImageInfo& imageInfo = imageInfos[i];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture->getImageView();
            imageInfo.sampler = texture->getSampler();
        }

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pipeline.getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &descriptorSetLayout;

        if (vkAllocateDescriptorSets(device.device(), &allocInfo, &spriteDataDescriptorSet) != VK_SUCCESS) { throw("failed to allocate descriptor set!"); }

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = spriteDataBuffer->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(SpriteData) * MAX_SPRITES;

        VkWriteDescriptorSet bufferWrite{};
        bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        bufferWrite.dstSet = spriteDataDescriptorSet;
        bufferWrite.dstBinding = 0;
        bufferWrite.dstArrayElement = 0;
        bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bufferWrite.descriptorCount = 1;
        bufferWrite.pBufferInfo = &bufferInfo;

        VkWriteDescriptorSet imageWrite{};
        imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        imageWrite.dstSet = spriteDataDescriptorSet;
        imageWrite.dstBinding = 1;
        imageWrite.dstArrayElement = 0;
        imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageWrite.descriptorCount = MAX_TEXTURES;
        imageWrite.pImageInfo = imageInfos.data();

        VkWriteDescriptorSet descriptorWrites[2] = { bufferWrite, imageWrite };
        vkUpdateDescriptorSets(device.device(), 2, descriptorWrites, 0, nullptr);
    }

    inline void updateAllTextures() {
        std::vector<VkDescriptorImageInfo> imageInfos(MAX_TEXTURES);

        for (unsigned int i = 0; i < MAX_TEXTURES; i++) {
            Texture* texture = spriteTextures[i].get();
            VkDescriptorImageInfo& imageInfo = imageInfos[i];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture->getImageView();
            imageInfo.sampler = texture->getSampler();
        }

        VkWriteDescriptorSet imageWrite{};
        imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        imageWrite.dstSet = spriteDataDescriptorSet;
        imageWrite.dstBinding = 1;
        imageWrite.dstArrayElement = 0;
        imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageWrite.descriptorCount = MAX_TEXTURES;
        imageWrite.pImageInfo = imageInfos.data();

        vkUpdateDescriptorSets(device.device(), 1, &imageWrite, 0, nullptr);
    }

    inline void updateTexture(unsigned char index) {
        Texture* texture = spriteTextures[index].get();
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->getImageView();
        imageInfo.sampler = texture->getSampler();

        VkWriteDescriptorSet imageWrite{};
        imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        imageWrite.dstSet = spriteDataDescriptorSet;
        imageWrite.dstBinding = 1;
        imageWrite.dstArrayElement = index;
        imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        imageWrite.descriptorCount = 1;
        imageWrite.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device.device(), 1, &imageWrite, 0, nullptr);
    }

    inline void renderSprites(VkCommandBuffer commandBuffer) {
        if (queuedTextures.size() != 0) {
            if (queuedTextures.size() == 1) {
                spriteTextures[queuedTextures[0].ID] = std::move(queuedTextures[0].texture);
                updateTexture(queuedTextures[0].ID);
                queuedTextures.clear();
            }
            else {
                while (queuedTextures.size() != 0) {
                    spriteTextures[queuedTextures[0].ID] = std::move(queuedTextures[0].texture);
                    queuedTextures.erase(queuedTextures.begin());
                }
                updateAllTextures();
            }
        }

        pipeline.bind(commandBuffer);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(), 0, 1, &spriteDataDescriptorSet, 0, nullptr);
        vkCmdPushConstants(commandBuffer, pipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Push), &vertex);

        for (unsigned int i = 0; i < spriteCPU.size(); i++) {
            if (spriteCPU[i].visible) {
                spriteCPU[i].model->bind(commandBuffer);
                spriteCPU[i].model->draw(commandBuffer, 1, i);
            }
        }
    }

    inline void updateSprites() { spriteDataBuffer->writeToBuffer(sprites.data(), sizeof(SpriteData) * sprites.size()); }float position[2];

private:

    Device& device;
    AppWindow& window;
    Renderer& renderer;
    Push& vertex;
    #ifdef __linux__
        Pipeline& pipeline;
    #else
        Pipeline pipeline;
    #endif
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unique_ptr<Buffer> spriteDataBuffer;
    VkDescriptorSet spriteDataDescriptorSet;
    VkDescriptorSet textureArrayDescriptorSet;
};