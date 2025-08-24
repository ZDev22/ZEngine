#include "app.hpp"

#include <thread>

std::chrono::high_resolution_clock::time_point CPSlastTime;
std::chrono::high_resolution_clock::time_point CPScurrentTime;
std::chrono::duration<float> CPSelapsed;

float timer = 0.f;
int cps = 0;
int fps = 0;
bool shouldClose = false;

App::App() {
    pipeline = std::make_unique<Pipeline>(device, *renderSystem, renderer, "texture");
    pipeline->loadSprites();
    renderSystem = std::make_unique<RenderSystem>(device, window, renderer, push, pipeline->getDescriptorSetLayout());
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

        timer += deltaTime;
        if (timer > 1.f) {
            window.setWindowName("CPS: " + std::to_string(cps) + " - FPS: " + std::to_string(fps));
            timer = 0.f;
            cps = 0;
            fps = 0;
        }

        glfwPollEvents();

        //flappyBird.tick();
        slimeAttack.tick();
        renderSystem->updateSprites();
        shouldClose = window.shouldClose();

        cps++;
    }

    vkDeviceWaitIdle(device.device());
}

void App::render() {
    while (!shouldClose) {
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer, pipeline->getPipelineLayout());
            vkCmdEndRenderPass(commandBuffer);
            renderer.endFrame();
            fps++;
        }
    }
}