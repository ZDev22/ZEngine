#pragma once

#define RGFW_VULKAN
#define RGFW_NO_API
#include "RGFW.h"

#include <vulkan/vulkan.h>

struct AppWindow {
public:
    AppWindow(int width, int height) {
        window = RGFW_createWindow("loading...", 100, 100, 720, 480, (u64)0);
        u8 icon[4 * 3 * 3] = {0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF};
        RGFW_window_setIcon(window, icon, 3, 3, 4);
    }

    void pollEvents() {
		RGFW_event e;
        while (RGFW_window_checkEvent(window, &e)) {
            if (e.type == RGFW_windowResized) {
				unsigned int new_width, new_height;
				RGFW_window_getSize(window, &width, &height);
				if (new_width != width || new_height != height) {
					width = new_width;
					height = new_height;
				}
                framebufferResized = true;
            }
            else if (e.type == RGFW_quit) {
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
