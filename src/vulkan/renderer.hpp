#pragma once

#include "device.hpp"
#include "model.hpp"
#include "swapChain.hpp"

class Renderer {
public:
    Renderer(AppWindow& window, Device& device);
    ~Renderer();

    VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const { return commandBuffers[currentImageIndex]; }
    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    AppWindow& window;
    Device& device;
    std::unique_ptr<SwapChain> swapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    unsigned int currentImageIndex;
    bool isFrameStarted;
};