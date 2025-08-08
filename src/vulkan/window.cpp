#include "window.hpp"

#include <stdexcept>

AppWindow::AppWindow(int w, int h) : width{ w }, height{ h } { initWindow(); }
AppWindow::~AppWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void AppWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(width, height, "loading...", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void AppWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) { if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) { throw std::runtime_error("failed to create window surface!"); }}

void AppWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto vulkanWindow = reinterpret_cast<AppWindow*>(glfwGetWindowUserPointer(window));
    vulkanWindow->framebufferResized = true;
    vulkanWindow->width = width;
    vulkanWindow->height = height;
}