/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

An example implementation on how to init and use zengine, as well as a few zdeps!

#define FPS_CAP 60.f // set the framerate, dont define for no FPS cap
*/

/* ZENGINE */
#define ZENGINE_IMPLEMENTATION
#define ZENGINE_DEPS_DEFINED
#define ZENGINE_DISABLE_VSYNC
//#define ZENGINE_DISABLE_AUDIO
#define ZENGINE_MAX_FRAMES_IN_FLIGHT 2
//#define ZENGINE_DEBUG
#define ZENGINE_MAX_SPRITES 100000
#define ZENGINE_MAX_TEXTURES 100
#include "zengine.h"

#ifndef ZENGINE_DISABLE_VSYNC
    #define FPS_CAP 60.f
#endif

/* games */
#include "examples/flappybird.h"
   
#include <time.h>

unsigned int fps = 0;
float appTimer = 0.f;
clock_t fpsTime;
clock_t fpsLastTime;
struct timespec ts;

int main() {
    /* init engine */
    zwindow = RGFW_createWindow("ZEngine", 0, 0, 720, 480, (u64)0);
    ZEngineInit();
    initGame();
    
    fpsLastTime = clock();
    while (1) {
        /* calculate fps */
        #ifdef FPS_CAP
            usleep((int)((1.0 / FPS_CAP) * 1000000.0)); 
        #endif

        fpsTime = clock();
        deltaTime = (double)(fpsTime - fpsLastTime) / CLOCKS_PER_SEC;
        fpsLastTime = fpsTime;
        appTimer += deltaTime;


        if (appTimer > 1.f) {
            char name[64];
            snprintf(name, 64, "fps: %d", fps);
            RGFW_window_setName(zwindow, name);
            appTimer = 0.f;
            fps = 0;
        }
        fps++;

        /* poll window events */
        RGFW_event event;
        while (RGFW_window_checkEvent(zwindow, &event)) {
            if (event.type == RGFW_windowResized) {
                framebufferResized = 1;
                break;
            }
            else if (event.type == RGFW_quit) {
                RGFW_window_close(zwindow);
                zwindow = NULL;
                ZEngineDeinit();
                exit(0);
            }
        }

        tickGame();

        ZEngineRender();
    }
}

