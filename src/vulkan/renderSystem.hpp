#pragma once

#include "pipeline.hpp"
#include "buffer.hpp"

#include "../deps/ZDev/keyboard.hpp"

class RenderSystem {
public:
    RenderSystem(Device& device, AppWindow& window, Push& push, VkDescriptorSetLayout descriptorSetLayout);
    ~RenderSystem();

    void renderSprites(VkCommandBuffer commandBuffer);
    void updateSprites();

    Pipeline& getPipeline() { return pipeline; }
    
private:
    void createPipeline();
    void createPipelineLayout();
    void initializeSpriteData();
    void createTextureArrayDescriptorSet();

    Device& device;
    AppWindow& window;
    Renderer& renderer;
    Push& push;
    Pipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unique_ptr<Buffer> spriteDataBuffer;
    VkDescriptorSet spriteDataDescriptorSet;
    VkDescriptorSet textureArrayDescriptorSet;
};