#include "vulkan/app.hpp"
#include "vulkan/shaders.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "deps/glfw/glfw3.h"

void error_callback(int error, const char* description) { std::cerr << "GLFW Error (" << error << "): " << description << std::endl; }
int main() {
    compile();
    glfwSetErrorCallback(error_callback);
    std::cout << "Calling glfwInit()..." << std::endl;
    if (!glfwInit()) { std::cout << "GLFW failed to initialize!" << std::endl; return 1; }
    std::cout << "GLFW initialized successfully!" << std::endl;
    if (!glfwVulkanSupported()) { std::cerr << "Vulkan is NOT supported!" << std::endl; return 1; }
    std::cout << "GLFW Vulkan is supported." << std::endl;

    App app{};
    app.run();
}