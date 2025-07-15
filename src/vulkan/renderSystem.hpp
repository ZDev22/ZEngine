#pragma once

#include "device.hpp"
#include "pipeline.hpp"
#include "global.hpp"
#include "buffer.hpp"
#include "sprite.hpp"

#include "../program/program.hpp"
#include "../program/functions/keyboard.hpp"

#include <memory>
#include <vector>

class RenderSystem {
public:
    RenderSystem(Device& device, AppWindow& window, Keyboard& keyboard, Program& program, Renderer& renderer, Global& global, Push& push, VkDescriptorSetLayout descriptorSetLayout);
    ~RenderSystem();

    RenderSystem(const RenderSystem&) = delete;
    RenderSystem& operator=(const RenderSystem&) = delete;

    void initialize();
    void reset(VkDescriptorSetLayout newDescriptorSetLayout);
    void renderSprites(VkCommandBuffer commandBuffer);
    void updateSprites();

private:
    void createPipeline();
    void createPipelineLayout();
    void initializeSpriteData();
    void createTextureArrayDescriptorSet();

    Device& device;
    AppWindow& window;
    Keyboard& keyboard;
    Program& program;
    Renderer& renderer;
    Global& global;
    Push& push;
    std::unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    std::unique_ptr<Buffer> spriteDataBuffer;
    VkDescriptorSet spriteDataDescriptorSet;
    VkDescriptorSet textureArrayDescriptorSet;
};