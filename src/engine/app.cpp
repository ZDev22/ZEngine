#include "app.hpp"

#include <thread>

#define MINIAUDIO_IMPLEMENTATION
#include "deps/miniaudio.h"

#include "games/flappyBird/flappyBird.hpp"
//#include "games/slimeAttack/slimeAttack.hpp"

#include "games/terminalCalculator.hpp"

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
#ifdef USE_MULTITHREADING
    #else
        std::chrono::duration<float> appcpselapsed;
    #endif

    std::chrono::high_resolution_clock::time_point appcpscurrentTime;
    std::chrono::high_resolution_clock::time_point appcpslastTime;
#endif

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

    #ifdef USE_MULTITHREADING
        std::thread render(&App::render, this);
        render.detach();
    #endif

    #if !FPS_CAP_SET && defined(USE_MULTITHREADING)
        appcpslastTime = std::chrono::high_resolution_clock::now();
    #endif
    while (!window.shouldClose()) {
        #if FPS_CAP_SET
            std::this_thread::sleep_for(std::chrono::milliseconds((int)((FPS_CAP * 1000) - appWait)));
            #ifdef USE_MULTITHREADING
                appcpscurrentTime = std::chrono::high_resolution_clock::now();
            #endif
            updatetimer += FPS_CAP;
            #if !NO_FPS
                apptimer += FPS_CAP;
            #endif
            if (updateFrame) {
        #else
            #ifdef USE_MULTITHREADING
                appcpscurrentTime = std::chrono::high_resolution_clock::now();
                appcpselapsed = appcpscurrentTime - appcpslastTime;
                deltaTime = appcpselapsed.count();
                appcpslastTime = appcpscurrentTime;
            #endif
            #if !NO_FPS
                apptimer += deltaTime;
            #endif
        #endif
        #if !NO_FPS
            if (apptimer > 1.f) {
                #ifdef USE_MULTITHREADING
                    #if NO_DECOR
                        std::cout << "cps: " << std::to_string(appcps) << " - fps: " << std::to_string(appfps) << std::endl;
                    #else
                        RGFW_window_setName(window.get(), ("cps: " + std::to_string(appcps) + " - fps: " + std::to_string(appfps)).c_str());
                    #endif
                #else
                    #if NO_DECOR
                        std::cout << "fps: " << std::to_string(appfps) << std::endl;
                    #else
                        RGFW_window_setName(window.get(), ("fps: " + std::to_string(appfps)).c_str());
                    #endif
                #endif
                apptimer = 0.f;
                appfps = 0;
                #ifdef USE_MULTITHREADING
                    appcps = 0;
                #endif
            }
            #ifdef USE_MULTITHREADING
                appcps++;
            #endif
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
            #ifdef USE_MULTITHREADING
                appcpslastTime = std::chrono::high_resolution_clock::now();
                auto duration = appcpslastTime - appcpscurrentTime;
                appWait = std::chrono::duration<float>(duration).count();
            #endif
            updateFrame = false;
            }
            else if (updatetimer > FPS_CAP) { updateFrame = true; updatetimer = 0.f;}
        #endif
#ifdef USE_MULTITHREADING
    }
    ma_engine_uninit(&audio);
    spriteTextures.clear();
}

void App::render() {
    while (!window.shouldClose()) {
#endif
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
#ifdef USE_MULTITHREADING
}
#else
    ma_engine_uninit(&audio);
    spriteTextures.clear();
}
#endif

void Terminal() {
    start();
}