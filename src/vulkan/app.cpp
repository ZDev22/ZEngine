#include "app.hpp"

#include <thread>

#define AUDIOPLAYER_IMPLEMENTATION
#include "../deps/ZDev/audio.hpp"

//#include "../games/flappyBird/flappyBird.hpp"
#include "../games/slimeAttack/slimeAttack.hpp"
//#include "../games/terminalCalculator/terminalCalculator.hpp"

std::chrono::high_resolution_clock::time_point appcpslastTime;
std::chrono::high_resolution_clock::time_point appcpscurrentTime;
std::chrono::duration<float> appcpselapsed;

float apptimer = 0.f;
int appcps = 0;
int appfps = 0;

bool shouldClose = false;

App::App() : pipeline(device, renderer, "texture") {
    pipeline.loadSprites();
    renderSystem = std::make_unique<RenderSystem>(device, window, renderer, push, pipeline.getDescriptorSetLayout());
}

void App::run() {

    AudioPlayer audio;
    Collision collision;

    //FlappyBird flappyBird{keyboard, audio, pipeline, collision, push};
    SlimeAttack slimeAttack{keyboard, audio, pipeline, collision, push};
    //TerminalCalculator terminalCalculator{};

    std::thread render(&App::render, this);
    render.detach();

    appcpslastTime = std::chrono::high_resolution_clock::now();
    while (!shouldClose) {
        appcpscurrentTime = std::chrono::high_resolution_clock::now();
        appcpselapsed = appcpscurrentTime - appcpslastTime;
        deltaTime = appcpselapsed.count();
        appcpslastTime = appcpscurrentTime;

        apptimer += deltaTime;
        if (apptimer > 1.f) {
            window.setWindowName("cps: " + std::to_string(appcps) + " - fps: " + std::to_string(appfps));
            apptimer = 0.f;
            appcps = 0;
            appfps = 0;
        }

        glfwPollEvents();
        keyboard.updateMouse();

        //flappyBird.tick();
        slimeAttack.tick();
        //terminalCalculator.tick();

        keyboard.resetKeys();
        collision.clearAABB();
        renderSystem->updateSprites();
        shouldClose = window.shouldClose();

        appcps++;
    }

    vkDeviceWaitIdle(device.device());
}

void App::render() {
    while (!shouldClose) {
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer);
            vkCmdEndRenderPass(commandBuffer);
            renderer.endFrame();
            appfps++;
        }
    }
}