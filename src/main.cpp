/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/README.md for current license

#define FPS_CAP_SET // set if the program will use the FPS cap
#define FPS_CAP 60.f // set the framerate (.5f / FPS_CAP)
#define USE_MULTITHREADING - multithread logic and rendering
#define USE_MOUSE // if your program uses the mouse, set this to true
*/

#if true // render the screen and tick the game (disable if it's a terminal game)

#define FPS_CAP_SET
#define FPS_CAP 60.f
#define USE_MOUSE
#define USE_MULTITHREADING

#define ZENGINE_IMPLEMENTATION
#include "zengine.hpp"
#undef ZENGINE_IMPLEMENTATION

/* graphical applications */
#include "games/flappyBird/flappyBird.hpp"
//#include "games/slimeAttack/slimeAttack.hpp"

/* terminal applications */
#include "games/terminalCalculator.hpp"

/* ZDEPS */
#include "deps/ZDeps/collision.hpp"
#include "deps/ZDeps/keyboard.hpp"

#include <thread>

void render();
int main() {

    /* sanity check */
    #ifndef FPS_CAP
        #define FPS_CAP 60.f
    #endif
    #ifdef FPS_CAP_SET
        deltaTime = 1.f / FPS_CAP;
    #endif

    initWindow(720, 480);
    initDevice();
    initRenderer();
    initPipeline("texture");

    loadFlappyBird();
    //loadSlimeAttack();

    initRenderSystem();

    Collision collision;
    Keyboard keyboard{windowdata};

    ma_engine audio;
    ma_engine_init(nullptr, &audio);

    FlappyBird flappyBird{keyboard, audio, collision, vertex};
    //SlimeAttack slimeAttack{keyboard, collision};

    #ifdef USE_MULTITHREADING
        std::thread renderthread(render);
        renderthread.detach();
    #endif

    while (!RGFW_window_shouldClose(windowdata) == RGFW_TRUE) {
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

        pollEvents();
        #ifdef USE_MOUSE
            keyboard.updateMouse();
        #endif

        flappyBird.tick();
        //slimeAttack.tick();

        keyboard.resetKeys();
        collision.clearAABB();
        updateSprites();

        #ifdef FPS_CAP_SET
            cpsLastTime = std::chrono::high_resolution_clock::now();
            appWait = std::chrono::duration<float>(cpsLastTime - cpsTime).count();
        #endif
#ifdef USE_MULTITHREADING
    }
    ma_engine_uninit(&audio);
    spriteTextures.clear();
    destroyDevice();
    destroyRenderer();
    destroyPipeline();
    destroyRenderSystem();
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
    destroyDevice();
    destroyRenderer();
    destroyPipeline();
    destroyRenderSystem();
#endif
}

#else

int main() { Terminal(); }

#endif