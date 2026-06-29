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
#include "zengine.h"

//#define FPS_CAP 180.f
//#define TRACK_FPS

/* games */
#include "examples/flappybird.h"

#include <time.h>
#include <unistd.h>

#ifdef TRACK_FPS
    #include <stdio.h>
#endif

unsigned short fps = 0;
float appTimer = 0.f;
struct timespec fpsTime;
struct timespec fpsLastTime;

int main() {
    /* init engine */
    setenv("__GL_YIELD", "USLEEP", 1);
    setenv("MESA_NO_ERROR", "1", 1);

    RGFW_init("z", RGFW_initVulkan);
    zwindow = RGFW_createWindow("...", 0, 0, 720, 480, 0);
    ZEngineInit();
    initGame();

#ifdef TRACK_FPS
    clock_gettime(CLOCK_MONOTONIC, &fpsLastTime);
#endif
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

    #ifdef TRACK_FPS
        if (appTimer > 1.f) {
            char name[6];
            snprintf(name, 6, "%hu", fps);
            RGFW_window_setName(zwindow, name);
            appTimer = 0.f;
            fps = 0;
        }
        fps++;
#endif

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
    RGFW_deinit();
}

