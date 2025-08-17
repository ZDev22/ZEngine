#include "renderSystem.hpp"
#include "global.hpp"

#include <stdexcept>
#include <iostream>
#include <array>
#include <algorithm>
#include <execution>

#include <glm/gtc/matrix_transform.hpp>

const uint32_t batchSize = 100;
uint32_t instanceCount;

using namespace std;

RenderSystem::RenderSystem(Device& device, AppWindow& window, Keyboard& keyboard, Program& program, Renderer& renderer, Push& push, VkDescriptorSetLayout descriptorSetLayout) : device(device), window(window), keyboard(keyboard), renderer(renderer), program(program), push(push), descriptorSetLayout(descriptorSetLayout) {
    createPipelineLayout();
    createPipeline();
}

RenderSystem::~RenderSystem() {
    spriteDataBuffer->unmap();
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void RenderSystem::reset(VkDescriptorSetLayout newDescriptorSetLayout) {
    createPipeline();
    descriptorSetLayout = newDescriptorSetLayout;
    initialize();
}

void RenderSystem::initialize() {
    initializeSpriteData();
    createTextureArrayDescriptorSet();
}

void RenderSystem::createPipelineLayout() {
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

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) { throw runtime_error("failed to create pipeline layout!"); }
}

void RenderSystem::createPipeline() { pipeline = make_unique<Pipeline>(device, *this, renderer, "vulkan/shaders/triangle.vert.spv", "vulkan/shaders/triangle.frag.spv"); }
void RenderSystem::initializeSpriteData() {
    VkDeviceSize bufferSize = sizeof(SpriteData) * MAX_SPRITES;
    spriteDataBuffer = make_unique<Buffer>(device, bufferSize, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, device.properties.limits.minStorageBufferOffsetAlignment);
    spriteDataBuffer->map();
    if (!sprites.empty()) { spriteDataBuffer->writeToBuffer(sprites.data(), sizeof(SpriteData) * sprites.size()); }
    push.projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    push.camera = glm::vec2(0.f);
}

void RenderSystem::createTextureArrayDescriptorSet() {
    if (!spriteDataBuffer) { throw("spriteDataBuffer is not initialized!"); }

    std::vector<VkDescriptorImageInfo> imageInfos;
    imageInfos.reserve(MAX_TEXTURES);

    std::unordered_map<Texture*, uint32_t> textureToIndex;
    uint32_t textureIndex = 0;

    for (size_t i = 0; i < MAX_TEXTURES; i++) {
        Texture* texture = spriteTextures[i].get();

        if (texture && textureToIndex.find(texture) == textureToIndex.end()) {
            textureToIndex[texture] = textureIndex++;

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = texture->getImageView();
            imageInfo.sampler = texture->getSampler();

            imageInfos.push_back(imageInfo);
        }
    }

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pipeline->getDescriptorPool();
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(device.device(), &allocInfo, &spriteDataDescriptorSet) != VK_SUCCESS) { throw runtime_error("failed to allocate descriptor set!"); }

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
    imageWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
    imageWrite.pImageInfo = imageInfos.data();

    array<VkWriteDescriptorSet, 2> descriptorWrites = { bufferWrite, imageWrite };
    vkUpdateDescriptorSets(device.device(), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    spriteDataBuffer->writeToBuffer(sprites.data(), sizeof(SpriteData) * sprites.size());
}

void RenderSystem::renderSprites(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout) {
    pipeline->bind(commandBuffer);
    
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &spriteDataDescriptorSet, 0, nullptr);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Push), &push);

    for (size_t i = 0; i < spriteCPU.size(); i++) {
        sprites[i].setRotationMatrix();
        spriteCPU[i].model->bind(commandBuffer);
        spriteCPU[i].model->draw(commandBuffer, 1, i);
    }
}

void RenderSystem::updateSprites() {
    program.tick();

    VkDeviceSize bufferSize = sizeof(SpriteData) * sprites.size();
    spriteDataBuffer->writeToBuffer(sprites.data(), bufferSize);
}