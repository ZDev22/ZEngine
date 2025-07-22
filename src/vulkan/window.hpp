#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../deps/glfw/glfw3.h"

#include <string>

class AppWindow {
public:
    AppWindow(int w, int h);
    ~AppWindow();

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }
    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    void waitEvents() { glfwWaitEvents(); }
    void setWindowName(std::string name) { glfwSetWindowTitle(window, name.c_str()); }
    GLFWwindow* getWindow() { return window; }

private:
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    int width;
    int height;
    bool framebufferResized = false;
    GLFWwindow* window;
};