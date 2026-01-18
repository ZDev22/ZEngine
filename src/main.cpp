/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/README.md for current license

An example implementation on how to init and use zengine, as well as a few zdeps!

#define FPS_CAP_SET // set if the program will use the FPS cap
#define FPS_CAP 60.f // set the framerate (.5f / FPS_CAP)
#define USE_MULTITHREADING - multithread logic and rendering
*/

#if true // render the screen and tick the game (disable if it's a terminal game)

#define ZENGINE_IMPLEMENTATION
#define ZENGINE_DISABLE_VSYNC
#define ZENGINE_MAX_SPRITES 1000
#define ZENGINE_MAX_TEXTURES 10
#include "zengine.hpp"

/* ZDEPS */
#define ZCOLLIDE_IMPLEMENTATION
#include "zcollide.hpp"
#include "zwindow.hpp"

#undef ZCOLLIDE_IMPLEMENTATION

/* graphical applications */
#include "games/flappyBird.hpp"
//#include "games/slimeAttack.hpp"

#include <thread>

//#define FPS_CAP_SET
#define FPS_CAP 180.f
#define USE_MULTITHREADING

void render();
int main() {
    /* sanity check */
    #ifndef FPS_CAP
        #define FPS_CAP 60.f
    #endif
    #ifdef FPS_CAP_SET
        deltaTime = 1.f / FPS_CAP;
    #endif

    ZWindow zwindow{windowdata, 720, 480};

    ZEngineInit("texture");

    ma_engine audio;
    ma_engine_init(nullptr, &audio);

    Game game{zwindow, audio, vertex};

    #ifdef USE_MULTITHREADING
        std::thread renderthread(render);
        renderthread.detach();
    #endif

    while (true) {
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
                RGFW_window_setName(windowdata, ("cps: " + std::to_string(appcps) + " - fps: " + std::to_string(appfps)).c_str());
            #else
                RGFW_window_setName(windowdata, ("fps: " + std::to_string(appcps)).c_str());
            #endif
            appTimer = 0.f;
            appfps = 0;
            appcps = 0;
        }
        appcps++;

        switch(zwindow.pollEvents()) {
        case 0:
            goto deinit;
        case 1:
            framebufferResized = true;
            break;
        }

        game.tick();

        zcollide_clearAABB();
        updateSprites();

        #ifdef FPS_CAP_SET
            cpsLastTime = std::chrono::high_resolution_clock::now();
            appWait = std::chrono::duration<float>(cpsLastTime - cpsTime).count();
        #endif
#ifdef USE_MULTITHREADING
    }
    deinit:
    ma_engine_uninit(&audio);
    ZEngineDeinit();
}

void render() {
    while (!RGFW_window_shouldClose(windowdata)) {
#endif
        #if defined(FPS_CAP_SET) && defined(USE_MULTITHREADING)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(((1.f / FPS_CAP) * 1000) - appFrameWait)));
            fpsTime = std::chrono::high_resolution_clock::now();
        #endif
        if (auto commandBuffer = beginFrame()) {
            beginSwapChainRenderPass(commandBuffer);
            renderSprites(commandBuffer);
            vkCmdEndRenderPass(commandBuffer);
            endFrame();
        }
        #ifdef USE_MULTITHREADING
            appfps++;
        #endif
        #if defined(FPS_CAP_SET) && defined(USE_MULTITHREADING)
            fpsLastTime = std::chrono::high_resolution_clock::now();
            appFrameWait = std::chrono::duration<float>(fpsLastTime - fpsTime).count();
        #endif
    }
#ifndef USE_MULTITHREADING
    deinit:
    ma_engine_uninit(&audio);
    ZEngineDeinit();
#endif
}

#else

/* terminal applications */
#include "games/terminalCalculator.hpp"

int main() { Terminal(); }

#endif