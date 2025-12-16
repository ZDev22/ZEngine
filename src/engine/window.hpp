#pragma once

#define RGFW_VULKAN
#define RGFW_NO_API
#include "deps/RGFW.h"

#include <vulkan/vulkan.h>

struct AppWindow {
public:
    AppWindow(int width, int height) {
        window = RGFW_createWindow("loading...", 100, 100, width, height, (u64)0);
        u8 icon[4 * 3 * 3] = {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};
        RGFW_window_setIcon(window, icon, 3, 3, 4);
    }

    void pollEvents() {
		RGFW_event event;
        while (RGFW_window_checkEvent(window, &event)) {
            if (event.type == RGFW_windowResized) {
				int newWidth = 0, newHeight = 0;
				RGFW_window_getSize(window, &width, &height);
				if (newWidth != width || newHeight != height) {
					width = newWidth;
					height = newHeight;
				}
                framebufferResized = true;
            }
            else if (event.type == RGFW_quit) {
                RGFW_window_close(window);
                window = nullptr;
            }
        }
    }

    inline bool shouldClose() { return RGFW_window_shouldClose(window) == RGFW_TRUE; }
    inline VkExtent2D getExtent() { return { (unsigned int)width, (unsigned int)height }; }
    inline bool resized() { return framebufferResized; }
    inline void resetResized() { framebufferResized = false; }
    inline RGFW_window* get() { return window; }

private:
    int width, height;
    bool framebufferResized = false;
    RGFW_window* window;
};
