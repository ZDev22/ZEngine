#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../deps/glfw/glfw3.h"

class AppWindow {
public:
    AppWindow(int width, int height);
    ~AppWindow();

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return { (unsigned int)width, (unsigned int)height }; }
    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }
    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    void setWindowName(const char* name) { glfwSetWindowTitle(window, name); }
    GLFWwindow* getWindow() { return window; }

private:
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    int width;
    int height;
    bool framebufferResized = false;
    GLFWwindow* window;
};