#include "vulkan/shaders.hpp"
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../glfw/glfw3.h"
#include "vulkan/app.hpp"

int main() {
    #if true // render the screen and tick the game (disable if it's a terminal game)
    compileShaders();
    glfwInit();
    App app{};
    app.run();
    #else
    Terminal();
    #endif
}