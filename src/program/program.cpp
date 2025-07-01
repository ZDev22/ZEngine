#include "program.hpp"
#include "../main.hpp"
#include "../vulkan/global.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

float speedY = 0.f;

Program::Program(Keyboard& keyboard) : keyboard(keyboard) {
    // Currently no initialization to be seen
}

void Program::tick() {
    speedY += 2.9f * deltaTime;
    sprites[0].translation.y += speedY * deltaTime;
    sprites[0].rotation -= .1f;
    
    if (keyboard.keyHit(GLFW_KEY_SPACE)) {
        speedY = -1.5f;
        sprites[0].rotation = -25.f;
    }

    //Reset stuff for next frame
    keyboard.resetKeys();
}