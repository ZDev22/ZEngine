#pragma once

#include "pipeline.hpp"
#include "buffer.hpp"

struct RenderSystem {
public:
    RenderSystem(Device& device, AppWindow& window, Renderer& renderer, Push& vertex, VkDescriptorSetLayout descriptorSetLayout);
    ~RenderSystem();

    void renderSprites(VkCommandBuffer commandBuffer);
    void updateSprites();

    Pipeline& getPipeline() { return pipeline; }

private:
    void createPipelineLayout();
    void initializeSpriteData();
    void createTextureArrayDescriptorSet();
    void updateAllTextures();
    void updateTexture(unsigned char index);

    Device& device;
    AppWindow& window;
    Renderer& renderer;
    Push& vertex;
    Pipeline pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unique_ptr<Buffer> spriteDataBuffer;
    VkDescriptorSet spriteDataDescriptorSet;
    VkDescriptorSet textureArrayDescriptorSet;
};