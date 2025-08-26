#include "app.hpp"

#include <thread>

//#include "../games/flappyBird/flappyBird.hpp"
#include "../games/slimeAttack/slimeAttack.hpp"

std::chrono::high_resolution_clock::time_point CPSlastTime;
std::chrono::high_resolution_clock::time_point CPScurrentTime;
std::chrono::duration<float> CPSelapsed;

float timer = 0.f;
int cps = 0;
int fps = 0;
bool shouldClose = false;

App::App() : renderSystem(device, window, renderer, push) { renderSystem.initialize(); }
void App::run() {

    //FlappyBird flappyBird{keyboard, renderSystem.getPipeline(), push};
    SlimeAttack slimeAttack{keyboard, renderSystem.getPipeline(), push};

    //flappyBird.init();
    slimeAttack.init();
    
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

        renderSystem.updateSprites();
        shouldClose = window.shouldClose();

        cps++;
    }

    vkDeviceWaitIdle(device.device());
}

void App::render() {
    while (!shouldClose) {
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem.renderSprites(commandBuffer);
            vkCmdEndRenderPass(commandBuffer);
            renderer.endFrame();
            fps++;
        }
    }
}