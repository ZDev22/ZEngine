/*
#define FPS_CAP_SET // set if the program will use the FPS cap
#define FPS_CAP 60.f // set the framerate (.5f / FPS_CAP)
#define USE_MOUSE // if your program uses the mouse, set this to true
*/
#pragma once

#include "games/flappyBird/flappyBird.hpp"
//#include "games/slimeAttack/slimeAttack.hpp"

#include "games/terminalCalculator.hpp"

#include <thread>
#if NO_DECOR && !NO_FPS
    #include <iostream>
#endif

#define MINIAUDIO_IMPLEMENTATION
#include "deps/miniaudio.h"

int appfps = 0;
int appcps = 0;
float appTimer = 0.f;
float appWait = 0.f;
float appFrameWait = 0.f;
std::chrono::high_resolution_clock::time_point fpsTime;
std::chrono::high_resolution_clock::time_point cpsTime;
std::chrono::high_resolution_clock::time_point fpsLastTime;
std::chrono::high_resolution_clock::time_point cpsLastTime;

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "renderSystem.hpp"

AppWindow window{720, 480};
Push vertex{};
Device device{window};
Renderer renderer{window, device};
Pipeline pipeline(device, renderer, "texture");
std::unique_ptr<RenderSystem> renderSystem;

inline void render();
inline void startEngine() {
    #ifndef FPS_CAP
        #define FPS_CAP 60.f
    #endif
    #ifdef FPS_CAP_SET
        deltaTime = 1.f / FPS_CAP;
    #endif
    pipeline.loadFlappyBird();
    //pipeline.loadSlimeAttack();

    renderSystem = std::make_unique<RenderSystem>(device, window, renderer, vertex, pipeline);

    Collision collision;
    Keyboard keyboard{window.get()};

    ma_engine audio;
    ma_engine_init(nullptr, &audio);

    FlappyBird flappyBird{keyboard, audio, pipeline, collision, vertex};
    //SlimeAttack slimeAttack{keyboard, pipeline, collision};

    #ifdef USE_MULTITHREADING
        std::thread renderthread(render);
        renderthread.detach();
    #endif

    while (!window.shouldClose()) {
        #ifdef FPS_CAP_SET
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(((1.f / FPS_CAP) * 1000) - appWait)));
            cpsTime = std::chrono::high_resolution_clock::now();
            appTimer += 1.f / FPS_CAP;
        #else
            cpsTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float>(cpsTime - cpsLastTime).count();
            appTimer += deltaTime;
            cpsLastTime = cpsTime;
        #endif
        if (appTimer > 1.f) {
            #ifdef USE_MULTITHREADING
                RGFW_window_setName(window.get(), ("cps: " + std::to_string(appcps) + " - fps: " + std::to_string(appfps)).c_str());
            #else
                RGFW_window_setName(window.get(), ("fps: " + std::to_string(appcps)).c_str());
            #endif
            appTimer = 0.f;
            appfps = 0;
            appcps = 0;
        }
        appcps++;

        window.pollEvents();
        #ifdef USE_MOUSE
            keyboard.updateMouse();
        #endif

        flappyBird.tick();
        //slimeAttack.tick();

        keyboard.resetKeys();
        collision.clearAABB();
        renderSystem->updateSprites();

        #ifdef FPS_CAP_SET
            cpsLastTime = std::chrono::high_resolution_clock::now();
            appWait = std::chrono::duration<float>(cpsLastTime - cpsTime).count();
        #endif
#ifdef USE_MULTITHREADING
    }
    ma_engine_uninit(&audio);
    spriteTextures.clear();
}

inline void render() {
    while (!window.shouldClose()) {
#endif
        #if defined(FPS_CAP_SET) && defined(USE_MULTITHREADING)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(((1.f / FPS_CAP) * 1000) - appFrameWait)));
            fpsTime = std::chrono::high_resolution_clock::now();
        #endif
        if (auto commandBuffer = renderer.beginFrame()) {
            renderer.beginSwapChainRenderPass(commandBuffer);
            renderSystem->renderSprites(commandBuffer);
            vkCmdEndRenderPass(commandBuffer);
            renderer.endFrame();
            #ifdef USE_MULTITHREADING
                appfps++;
            #endif
        }
        #if defined(FPS_CAP_SET) && defined(USE_MULTITHREADING)
            fpsLastTime = std::chrono::high_resolution_clock::now();
            appFrameWait = std::chrono::duration<float>(fpsLastTime - fpsTime).count();
        #endif
    }
#ifndef USE_MULTITHREADING
    ma_engine_uninit(&audio);
    spriteTextures.clear();
#endif
}