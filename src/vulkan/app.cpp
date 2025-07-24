#include "app.hpp"
#include "sprite.hpp"
#include "global.hpp"

#include <stdexcept>
#include <chrono>
#include <string>
#include <thread>

static std::chrono::high_resolution_clock::time_point CPSlastTime;
static std::chrono::high_resolution_clock::time_point CPScurrentTime;
static std::chrono::duration<float> CPSelapsed;

static std::chrono::high_resolution_clock::time_point FPSlastTime;
static std::chrono::high_resolution_clock::time_point FPScurrentTime;
static std::chrono::duration<float> FPSelapsed;
static float FPSdelta = 0.f;

static float updateAccumulator = 0.f;

bool shouldClose = false;

App::App() {
    pipeline = std::make_unique<Pipeline>(device, *renderSystem, renderer, "vulkan/shaders/triangle.vert.spv", "vulkan/shaders/triangle.frag.spv");
    pipeline->loadSprites();
    renderSystem = std::make_unique<RenderSystem>(device, window, keyboard, program, renderer, push, pipeline->getDescriptorSetLayout());
    renderSystem->initialize();
}

void App::run() {
    std::thread update(&App::render, this);
    update.detach();

    CPSlastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose) {
        CPScurrentTime = std::chrono::high_resolution_clock::now();
        CPSelapsed = CPScurrentTime - CPSlastTime;
        deltaTime = CPSelapsed.count();
        CPSlastTime = CPScurrentTime;

        updateAccumulator += deltaTime;
        if (updateAccumulator > 1.f) {
            window.setWindowName("CPS: " + std::to_string(static_cast<int>(1.f / deltaTime)) + " - FPS: " + std::to_string(static_cast<int>(1.f / FPSdelta)));
            updateAccumulator = 0.f;
        }

        glfwPollEvents();
        renderSystem->updateSprites();
        shouldClose = window.shouldClose();
    }

    vkDeviceWaitIdle(device.device());
}

void App::render() {
    FPSlastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose) {
        FPScurrentTime = std::chrono::high_resolution_clock::now();
        FPSelapsed = FPScurrentTime - FPSlastTime;
        FPSdelta = FPSelapsed.count();
        FPSlastTime = FPScurrentTime;
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }
}