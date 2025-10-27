#include "window.hpp"

AppWindow::AppWindow(int width, int height) : width(width), height(height) { initWindow(); }
AppWindow::~AppWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AppWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    window = glfwCreateWindow(width, height, "loading...", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void AppWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) { if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) { throw("failed to create window surface!"); }}
void AppWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto vulkanWindow = reinterpret_cast<AppWindow*>(glfwGetWindowUserPointer(window));
    vulkanWindow->framebufferResized = true;
    vulkanWindow->width = width;
    vulkanWindow->height = height;
}