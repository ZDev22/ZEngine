#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "global.hpp"
#include "buffer.hpp"
#include "sprite.hpp"

#include "../deps/ZDev/keyboard.hpp"

class RenderSystem {
public:
    RenderSystem(Device& device, AppWindow& window, Renderer& renderer, Pipeline& pipeline, Push& push, VkDescriptorSetLayout descriptorSetLayout);
    ~RenderSystem();

    void initialize();
    void reset(VkDescriptorSetLayout newDescriptorSetLayout);
    void renderSprites(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout);
    void updateSprites();
    
private:
    void createPipelineLayout();
    void initializeSpriteData();
    void createTextureArrayDescriptorSet();

    Device& device;
    AppWindow& window;
    Renderer& renderer;
    Pipeline& pipeline;
    Push& push;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unique_ptr<Buffer> spriteDataBuffer;
    VkDescriptorSet spriteDataDescriptorSet;
    VkDescriptorSet textureArrayDescriptorSet;
};