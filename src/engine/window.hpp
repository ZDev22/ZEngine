#pragma once

#include <vulkan/vulkan.h>
#define RGFW_IMPLEMENTATION
#include "../deps/RGFW.h"

#include <string>

struct AppWindow {
public:
    inline AppWindow(int width, int height) {
        RGFW_rectangle rect = {0, 0, width, height};
        window = RGFW_createWindow("loading...", rect, RGFW_VULKAN | RGFW_ALLOW_DND);
    }
    inline ~AppWindow() {
        RGFW_freeWindow(window);
        window = nullptr;
    }

    inline void pollEvents() {
        RGFW_readWindow(window);

        if (window->event.type == RGFW_windowAttribsChange && (window->event.x != width || window->event.y != height)) {
            width = window->event.x;
            height = window->event.y;
            framebufferResized = true;
        }
    }
    inline bool shouldClose() { return window->close; }
    inline VkExtent2D getExtent() { return { (unsigned int)width, (unsigned int)height }; }
    inline bool resized() { return framebufferResized; }
    inline void resetResized() { framebufferResized = false; }
    inline RGFW_window* get() { return window; }

private:
    int width, height;
    bool framebufferResized = false;
    RGFW_window* window;
};