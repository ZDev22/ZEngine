#include "vulkan/app.hpp"
#include "vulkan/shaders.hpp"

#include "deps/glfw/glfw3.h"

#include "deps/ZDev/string.hpp"

void error_callback(int error, const char* description) { std::cerr << "GLFW Error (" << error << "): " << description << std::endl; }
int main() {
    compile();

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) { throw("GLFW failed to initialize!"); }

    App app{};
    app.run();
}