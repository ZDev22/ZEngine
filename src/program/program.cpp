#include "program.hpp"
#include "../main.hpp"
#include "../vulkan/global.hpp"
#include "functions/keyboard.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

Program::Program(Keyboard& keyboard) : keyboard(keyboard) {
    // Currently no initialization to be seen
}

float speedY = 0.f;

void Program::tick() {
    speedY += 2.9f * deltaTime;
    sprites[0].translation.y += speedY * deltaTime;
    sprites[0].rotation -= .1f;
    std::vector<int> keys = {GLFW_KEY_SPACE, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D};
    if (keyboard.keyHit(keys[0])) {
        speedY = -1.5f;
        sprites[0].rotation = -25.f;
    }

    //Reset stuff for next frame
    keyboard.resetKeys();
}