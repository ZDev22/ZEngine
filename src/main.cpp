#include "vulkan/shaders.hpp"
#include "deps/glfw/glfw3.h"
#include "vulkan/app.hpp"

int main() {
    compileShaders();
    glfwInit();

    App app{};
    app.run();
}