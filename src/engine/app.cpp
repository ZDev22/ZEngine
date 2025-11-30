#include "app.hpp"

#include <thread>

#define MINIAUDIO_IMPLEMENTATION
#include "../deps/miniaudio.h"

#include "../games/flappyBird/flappyBird.hpp"
//#include "../games/slimeAttack/slimeAttack.hpp"

#include "../games/terminalCalculator/terminalCalculator.hpp"

#if NO_DECOR && !NO_FPS
    #include <iostream>
#endif

#if !NO_FPS
    float apptimer = 0.f;
    int appcps = 0;
    int appfps = 0;
#endif

#if FPS_CAP_SET
    bool updateFrame = false;
    float updatetimer = 0.f;
    std::chrono::high_resolution_clock::time_point appfpscurrentTime;
    std::chrono::high_resolution_clock::time_point appfpslastTime;
    float appWait = 0.f;
    float appFrameWait = 0.f;
#else
    std::chrono::duration<float> appcpselapsed;
#endif

std::chrono::high_resolution_clock::time_point appcpscurrentTime;
std::chrono::high_resolution_clock::time_point appcpslastTime;

App::App() : pipeline(device, renderer, "texture") {
    #if FPS_CAP_SET
        deltaTime = FPS_CAP * 2.f;
    #endif
    pipeline.loadFlappyBird();
    //pipeline.loadSlimeAttack();
    renderSystem = std::make_unique<RenderSystem>(device, window, renderer, vertex, pipeline.getDescriptorSetLayout());
}

void App::run() {
    Collision collision;
    Keyboard keyboard{window.get()};

    ma_engine audio;
    ma_engine_init(nullptr, &audio);

    FlappyBird flappyBird{keyboard, audio, pipeline, collision, vertex};
    //SlimeAttack slimeAttack{keyboard, pipeline, collision};

    std::thread render(&App::render, this);
    render.detach();

    #if !FPS_CAP_SET
        appcpslastTime = std::chrono::high_resolution_clock::now();
    #endif
    while (!window.shouldClose()) {
        #if FPS_CAP_SET
            std::this_thread::sleep_for(std::chrono::milliseconds((int)((FPS_CAP * 1000) - appWait)));
            appcpscurrentTime = std::chrono::high_resolution_clock::now();
            updatetimer += FPS_CAP;
            #if !NO_FPS
                apptimer += FPS_CAP;
            #endif
            if (updateFrame) {
        #else
            appcpscurrentTime = std::chrono::high_resolution_clock::now();
            appcpselapsed = appcpscurrentTime - appcpslastTime;
            deltaTime = appcpselapsed.count();
            appcpslastTime = appcpscurrentTime;
            #if !NO_FPS
                apptimer += deltaTime;
            #endif
        #endif
        #if !NO_FPS
            if (apptimer > 1.f) {
                #if NO_DECOR
                    std::cout << "cps: " << std::to_string(appcps) << " - fps: " << std::to_string(appfps) << std::endl;
                #else
                    RGFW_window_setName(window.get(), ("cps: " + std::to_string(appcps) + " - fps: " + std::to_string(appfps)).c_str());
                #endif
                apptimer = 0.f;
                appcps = 0;
                appfps = 0;
            }
            appcps++;
        #endif

        window.pollEvents();
        #if USE_MOUSE
            keyboard.updateMouse();
        #endif

        flappyBird.tick();
        //slimeAttack.tick();

        keyboard.resetKeys();
        collision.clearAABB();
        renderSystem->updateSprites();

        #if FPS_CAP_SET
            appcpslastTime = std::chrono::high_resolution_clock::now();
            auto duration = appcpslastTime - appcpscurrentTime;
            appWait = std::chrono::duration<float>(duration).count();
            updateFrame = false;
            }
            else if (updatetimer > FPS_CAP) { updateFrame = true; updatetimer = 0.f;}
        #endif
    }
    ma_engine_uninit(&audio);
    spriteTextures.clear();
}

void App::render() {
    while (!window.shouldClose()) {
        #if FPS_CAP_SET
            std::this_thread::sleep_for(std::chrono::milliseconds((int)((FPS_CAP * 1000) - appFrameWait)));
            appfpscurrentTime = std::chrono::high_resolution_clock::now();
            if (updateFrame) {
        #endif
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer);
            vkCmdEndRenderPass(commandBuffer);
            renderer.endFrame();
            #if !NO_FPS
                appfps++;
            #endif
        }
        #if FPS_CAP_SET
            appfpslastTime = std::chrono::high_resolution_clock::now();
            auto duration = appfpslastTime - appfpscurrentTime;
            appFrameWait = std::chrono::duration<float>(duration).count();
            }
        #endif
    }
}

void Terminal() {
    TerminalCalculator();
}
