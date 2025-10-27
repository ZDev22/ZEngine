#include "engine/app.hpp"

#if true // render the screen and tick the game (disable if it's a terminal game)

#include "engine/shaders.hpp"
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "../glfw/glfw3.h"

int main() {
    compileShaders();
    glfwInit();
    App app{};
    app.run();
}

#else

int main() { Terminal(); }

#endif