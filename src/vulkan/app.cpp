#include "app.hpp"
#include "sprite.hpp"
#include "global.hpp"

#include <stdexcept>
#include <chrono>
#include <string>
#include <thread>

static std::chrono::high_resolution_clock::time_point CPSlastTime;
static std::chrono::high_resolution_clock::time_point CPScurrentTime;
static float CPSupdateAccumulator = 0.f;

bool shouldClose = false;

App::App() {
    pipeline = std::make_unique<Pipeline>(device, *renderSystem, renderer, "vulkan/shaders/triangle.vert.spv", "vulkan/shaders/triangle.frag.spv");
    pipeline->loadSprites();
    global = std::make_unique<Global>(window);
    renderSystem = std::make_unique<RenderSystem>(device, window, keyboard, program, renderer, *global, push, pipeline->getDescriptorSetLayout());
    renderSystem->initialize();
}

void App::run() {
    std::thread update(&App::render, this);
    update.detach();

    CPSlastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose) {
        CPScurrentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> CPSelapsed = CPScurrentTime - CPSlastTime;
        deltaTime = CPSelapsed.count();
        CPSlastTime = CPScurrentTime;
        CPSupdateAccumulator += deltaTime;

        if (CPSupdateAccumulator > 1.f) {
            window.setWindowName("vulkan - CPS: " + std::to_string(1 / deltaTime));
            CPSupdateAccumulator = 0.f;
        }

        glfwPollEvents();
        renderSystem->updateSprites();
        shouldClose = window.shouldClose();
    }

    vkDeviceWaitIdle(device.device());
}

void App::render() {
    while (!shouldClose) {
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }
}