/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

An example implementation on how to init and use zengine, as well as a few zdeps!

#define FPS_CAP 60.f // set the framerate, dont define for no FPS cap
*/

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L

/* ZENGINE */
#define ZENGINE_IMPLEMENTATION
#define ZENGINE_DEPS_DEFINED
#define ZENGINE_DISABLE_VSYNC
#define ZENGINE_MAX_FRAMES_IN_FLIGHT 2
#define ZENGINE_DEBUG
#define ZENGINE_MAX_SPRITES 10000
#define ZENGINE_MAX_TEXTURES 25
#include "zengine.h"

//#define FPS_CAP 180.f

/* games */
#include "examples/flappybird.h"

#include <time.h>
#include <unistd.h>

unsigned int fps = 0;
float appTimer = 0.f;
struct timespec fpsTime;
struct timespec fpsLastTime;

int main() {
    /* init engine */
    setenv("__GL_YIELD", "USLEEP", 1);
    setenv("MESA_NO_ERROR", "1", 1);

    zwindow = RGFW_createWindow("loading...", 0, 0, 720, 480, (u64)0);
    ZEngineInit();
    initGame();

    clock_gettime(CLOCK_MONOTONIC, &fpsLastTime);
    while (!RGFW_window_shouldClose(zwindow)) {
        /* calculate fps */
        #ifdef FPS_CAP
            usleep((int)((1.0 / FPS_CAP) * 1000000.0));
        #endif

        clock_gettime(CLOCK_MONOTONIC, &fpsTime);
        deltaTime = (double)(fpsTime.tv_sec - fpsLastTime.tv_sec) + (double)(fpsTime.tv_nsec - fpsLastTime.tv_nsec) / 1000000000.0;
        fpsLastTime.tv_nsec = fpsTime.tv_nsec;
        fpsLastTime.tv_sec = fpsTime.tv_sec;
        appTimer += deltaTime;

        if (appTimer > 1.f) {
            char name[32];
            snprintf(name, 32, "fps: %d", fps);
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
        }

        tickGame();

        ZEngineRender();
    }

    RGFW_window_close(zwindow);
    zwindow = NULL;
    ZEngineDeinit();
}

