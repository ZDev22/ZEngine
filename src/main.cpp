#include "vulkan/app.hpp"
#include "vulkan/shaders.hpp"

#include "deps/glfw/glfw3.h"

int main() {
    compileShaders();
    glfwInit();

    App app{};
    app.run();
}