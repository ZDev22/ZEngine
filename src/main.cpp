#include "vulkan/shaders.hpp"
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../glfw/glfw3.h"
#include "vulkan/app.hpp"

int main() {
    compileShaders();
    glfwInit();

    App app{};
    app.run();
}