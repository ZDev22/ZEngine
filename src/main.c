/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

An example implementation on how to init and use zengine, as well as a few zdeps!
*/

#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L

/* ZENGINE */
#define ZENGINE_IMPLEMENTATION
#define ZENGINE_DISABLE_VSYNC
#define ZENGINE_FAST_DEPTH
#include "zengine.h"

/* zutils is not required, if you don't want to use it remove ZUtilsInit and ZUtilsTick. If you do not include it, you will have to handle window resizing yourself */
#include "zutils.h"

//#define FPS_CAP 60.f

#ifdef TRACK_FPS
    #include <stdio.h>
#endif

/* games */
#include "test/flappybird.h"

#include <time.h>
#include <unistd.h>

unsigned short fps = 0;
float appTimer = 0.f;
struct timespec fpsTime;
struct timespec fpsLastTime;

int main() {
    /* init engine */
#ifdef __linux__
    setenv("__GL_YIELD", "USLEEP", 1);
    setenv("MESA_NO_ERROR", "1", 1);
#endif

    RGFW_init("z", RGFW_initVulkan);
    zwindow = RGFW_createWindow("...", 0, 0, 720, 480, 0);
#ifdef _WIN32
    RGFW_window_maximize(zwindow);
#endif
    ZUtilsInit();
    ZEngineInit();
    initGame();

#if defined(TRACK_FPS) || defined(FPS_CAP)
    clock_gettime(CLOCK_MONOTONIC, &fpsLastTime);
#endif
    while (!RGFW_window_shouldClose(zwindow)) {
        /* calculate fps */
        clock_gettime(CLOCK_MONOTONIC, &fpsTime);
        deltaTime = (double)(fpsTime.tv_sec - fpsLastTime.tv_sec) + (double)(fpsTime.tv_nsec - fpsLastTime.tv_nsec) / 1000000000.0;
        fpsLastTime.tv_nsec = fpsTime.tv_nsec;
        fpsLastTime.tv_sec = fpsTime.tv_sec;
        appTimer += deltaTime;

#ifdef FPS_CAP
        float sleep = ((1.f / FPS_CAP) * 1000000.f) - (deltaTime * 100000.f);
        if (sleep > 0.f) { usleep((useconds_t)sleep); }
#endif

#ifdef TRACK_FPS
        if (appTimer > 1.f) {
            char name[6];
            sprintf(name, "%hu", fps);
            RGFW_window_setName(zwindow, name);
            appTimer = 0.f;
            fps = 0;
        }
        fps++;
#endif
        ZUtilsTick();
        tickGame();
        ZEngineRender();
    }

    RGFW_window_close(zwindow);
    zwindow = NULL;
    ZEngineDeinit();
    ZUtilsDeinit();
    RGFW_deinit();
    deinitGame();
}
