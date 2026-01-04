#pragma once

#include "model.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "swapChain.hpp"

struct Renderer {
public:
    inline Renderer(AppWindow& window, Device& device) : window{ window }, device{ device } {
        recreateSwapChain();
        createCommandBuffers();
    }
    inline ~Renderer() { freeCommandBuffers(); }

    inline void recreateSwapChain() {
        auto extent = window.getExtent();
        vkDeviceWaitIdle(device.device());

        if (swapChain == nullptr) { swapChain = std::make_unique<SwapChain>(device, extent); }
        else {
            auto oldSwapChain = std::move(swapChain);
            swapChain = std::make_unique<SwapChain>(device, extent, std::move(oldSwapChain));
            if (swapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
    }

    inline void createCommandBuffers() {
        commandBuffers.resize(swapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<unsigned int>(commandBuffers.size());

        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) { throw("failed to allocate command buffers!"); }
    }

    inline void freeCommandBuffers() {
        vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<unsigned int>(commandBuffers.size()), commandBuffers.data());
        commandBuffers.clear();
    }

    inline VkCommandBuffer beginFrame() {
        if (swapChain->acquireNextImage(&currentImageIndex) == VK_ERROR_OUT_OF_DATE_KHR) { recreateSwapChain(); return nullptr; }

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        auto commandBuffer = getCurrentCommandBuffer();
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        return commandBuffer;
    }

    inline void endFrame() {
        auto commandBuffer = getCurrentCommandBuffer();
        vkEndCommandBuffer(commandBuffer);

        auto result = swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.resized()) {
            window.resetResized();
            recreateSwapChain();
        }
    }

    inline void beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = (unsigned int)swapChain->getFrameBuffer(currentImageIndex);
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        VkClearValue clearValues[2] = {};
        clearValues[0].color = VkClearColorValue{ .float32 = {.1f, .1f, .1f, 1.f} };
        clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.f, 0 };
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.f;
        viewport.y = 0.f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapChain->getSwapChainExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    VkCommandBuffer getCurrentCommandBuffer() const { return commandBuffers[currentImageIndex]; }
    VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }

private:
    AppWindow& window;
    Device& device;
    std::unique_ptr<SwapChain> swapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    unsigned int currentImageIndex;
};