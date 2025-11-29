#pragma once

#define RGFW_IMPLEMENTATION
#include "RGFW.h"

struct AppWindow {
public:
    AppWindow(int width, int height) {
        window = RGFW_createWindow("loading...", RGFW_RECT(100, 100, 720, 480), (u64)0);
        u8 icon[4 * 3 * 3] = {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};
        RGFW_window_setIcon(window, icon, RGFW_AREA(3, 3), 4);
    }

    void pollEvents() {
        while (RGFW_window_checkEvent(window)) {
            if (window->event.type == RGFW_windowResized && (window->event.point.x != width || window->event.point.y != height)) {
                width = window->event.point.x;
                height = window->event.point.y;
                framebufferResized = true;
            }
            else if (window->event.type == RGFW_quit) {
                RGFW_window_close(window);
                window = nullptr;
            }
        }
    }

    inline bool shouldClose() { return window->_flags & 1 << 25; }
    inline VkExtent2D getExtent() { return { (unsigned int)width, (unsigned int)height }; }
    inline bool resized() { return framebufferResized; }
    inline void resetResized() { framebufferResized = false; }
    inline RGFW_window* get() { return window; }

private:
    int width, height;
    bool framebufferResized = false;
    RGFW_window* window;
};
