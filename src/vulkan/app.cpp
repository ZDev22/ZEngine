#include "app.hpp"
#include "sprite.hpp"
#include "global.hpp"

#include <stdexcept>
#include <chrono>
#include <string>
#include <thread>

static std::chrono::high_resolution_clock::time_point lastTime;
static float timeAccumulator = 0.f;
static float titleUpdateAccumulator = 0.f;

bool shouldClose = false;

App::App() {
    pipeline = std::make_unique<Pipeline>(device, *renderSystem, renderer, "vulkan/shaders/triangle.vert.spv", "vulkan/shaders/triangle.frag.spv");
    pipeline->loadSprites();
    global = std::make_unique<Global>(window);
    renderSystem = std::make_unique<RenderSystem>(device, window, keyboard, program, renderer, pipeline->getDescriptorSetLayout(), *global);
    renderSystem->initialize();
}

void App::run() {
    lastTime = std::chrono::high_resolution_clock::now();

    std::thread update(&App::render, this);
    update.detach();

    while (!shouldClose) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;
        titleUpdateAccumulator += deltaTime;

        if (titleUpdateAccumulator > 1.f) {
            window.setWindowName("vulkan - " + std::to_string(1 / deltaTime));
            titleUpdateAccumulator = .0f;
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