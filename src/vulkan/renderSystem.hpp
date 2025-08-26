#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "global.hpp"
#include "buffer.hpp"
#include "sprite.hpp"

#include "../deps/ZDev/keyboard.hpp"

class RenderSystem {
public:
    RenderSystem(Device& device, AppWindow& window, Renderer& renderer, Push& push);
    ~RenderSystem();

    void initialize();
    void renderSprites(VkCommandBuffer commandBuffer);
    void updateSprites();

    Pipeline& getPipeline() { return *pipeline; }

private:
    void createPipelineLayout();
    void initializeSpriteData();
    void createTextureArrayDescriptorSet();

    Device& device;
    AppWindow& window;
    Renderer& renderer;
    std::unique_ptr<Pipeline> pipeline;
    Push& push;
    VkPipelineLayout pipelineLayout;
    std::unique_ptr<Buffer> spriteDataBuffer;
    VkDescriptorSet spriteDataDescriptorSet;
    VkDescriptorSet textureArrayDescriptorSet;
};