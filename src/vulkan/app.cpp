#include "app.hpp"
#include "sprite.hpp"
#include "global.hpp"

#include <stdexcept>
#include <chrono>
#include <string>

using HighResClock = std::chrono::high_resolution_clock;
static HighResClock::time_point lastTime;
static uint32_t fps = 0;
static float timeAccumulator = 0.0f;

App::App() {
    pipeline = std::make_unique<Pipeline>(
        device,
        "vulkan/shaders/triangle.vert.spv",
        "vulkan/shaders/triangle.frag.spv",
        renderer.getSwapChainRenderPass()
    );
    pipeline->loadSprites();
    global = std::make_unique<Global>(window);
    renderSystem = std::make_unique<RenderSystem>(
        device,
        window,
        keyboard,
        program,
        renderer.getSwapChainRenderPass(),
        pipeline->getDescriptorSetLayout(),
        *global
    );
    renderSystem->initialize();
}

void App::run() {
    lastTime = HighResClock::now();

    while (!window.shouldClose()) {
        auto currentTime = HighResClock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;

        fps = 1 / deltaTime;
        window.setWindowName("vulkan - " + std::to_string(fps));

        glfwPollEvents();
        renderSystem->updateSprites();

        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer);
            renderer.endSwapChainRenderPass(commandBuffer);
            renderer.endFrame();
        }
    }

    vkDeviceWaitIdle(device.device());
}