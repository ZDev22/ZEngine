/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/README.md for current license

An example implementation on how to init and use zengine, as well as a few zdeps!

#define FPS_CAP_SET // set if the program will use the FPS cap
#define FPS_CAP 60.f // set the framerate (.5f / FPS_CAP)
*/

#if true // render the screen and tick the game (disable if it's a terminal game)

#define ZENGINE_IMPLEMENTATION
#define ZENGINE_DISABLE_VSYNC
#define ZENGINE_DEBUG 3 
#define ZENGINE_MAX_SPRITES 1000
#define ZENGINE_MAX_TEXTURES 10
#include "zengine.hpp"

/* ZDEPS */
#define ZCOLLIDE_IMPLEMENTATION
#define ZTEXT_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_ASSERT
#include "zcollide.hpp"
#include "zwindow.hpp"
#include "ztext.hpp"

/* graphical applications */
#include "games/flappyBird.hpp"
//#include "games/slimeAttack.hpp"

#include <thread>

//#define FPS_CAP_SET
#define FPS_CAP 180.f

/* vars for calculating fps and deltaTime */
int fps = 0;
float appTimer = 0.f;
float appWait = 0.f;
std::chrono::high_resolution_clock::time_point fpsTime;
std::chrono::high_resolution_clock::time_point fpsLastTime;

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

    ZEngineInit();

    ma_engine audio;
    ma_engine_init(nullptr, &audio);

    Game game{zwindow, audio, camera};

    while (true) {
        #ifdef FPS_CAP_SET
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(((1.f / FPS_CAP) * 1000) - appWait)));
            fpsTime = std::chrono::high_resolution_clock::now();
            appTimer += 1.f / FPS_CAP;
        #else
            fpsTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float>(fpsTime - fpsLastTime).count();
            appTimer += deltaTime;
            fpsLastTime = fpsTime;
        #endif
        if (appTimer > 1.f) {
            RGFW_window_setName(windowdata, ("fps: " + std::to_string(fps)).c_str());
            appTimer = 0.f;
            fps = 0;
        }
        fps++;

        switch(zwindow.pollEvents()) {
        case 0:
            ma_engine_uninit(&audio);
            ZEngineDeinit();
            return 0;
        case 1:
            framebufferResized = true;
            break;
        }

        game.tick();

        zcollide_clearAABB();

        #ifdef FPS_CAP_SET
            fpsLastTime = std::chrono::high_resolution_clock::now();
            appWait = std::chrono::duration<float>(fpsLastTime - fpsTime).count();
        #endif

        ZEngineRender();
    }
}

#else

/* terminal applications */
#include "games/terminalCalculator.hpp"

int main() { Terminal(); }

#endif
