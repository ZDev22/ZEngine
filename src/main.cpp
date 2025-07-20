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
    if (!glfwInit()) { std::cout << "GLFW failed to initialize!" << std::endl; return 1; }
    std::cout << "GLFW initialized successfully!" << std::endl;

    App app{};
    app.run();
}