#include "program.hpp"
#include "../vulkan/global.hpp"
#include "../vulkan/pipeline.hpp"
#include "functions/keyboard.hpp"
#include "functions/math.hpp"
#include "functions/collision.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

Program::Program(Keyboard& keyboard, Pipeline& pipeline) : keyboard(keyboard), pipeline(pipeline) {

}

float speedY = 0.f;

void Program::tick() {
    speedY += 3.8f * deltaTime;
    sprites[0].position.y += speedY * deltaTime;
    sprites[0].rotation -= 90.f * deltaTime;
    std::vector<int> keys = {GLFW_KEY_SPACE, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D};
    if (keyboard.keyHit(keys[0])) {
        speedY = -1.3f;
        sprites[0].rotation = 60.f;
        sprites[0].textureIndex = 0;
    }

    for (size_t i = 1; i < (sprites.size()) / 2; i++) {
        size_t index = ((i - 1) * 2) + 1; // A reliable method of searching every other index in a list! (You can multiply by 3 for every third item, and so on)
        sprites[index].position.x -= .5f * deltaTime;
        if (sprites[index].position.x < -1.5f) {
            sprites[index].position.x = 1.5f;
            sprites[index].position.y = randomFloat(.4f, 1.4f);
        }
        sprites[index + 1].position.x = sprites[index].position.x;
        sprites[index + 1].position.y = sprites[index].position.y - 2.f;

        if (checkCollision(spriteCPU[index], sprites[index], spriteCPU[0], sprites[0]) || checkCollision(spriteCPU[index + 1], sprites[index + 1], spriteCPU[0], sprites[0])) { sprites[0].textureIndex = 1; }
    }

    //Reset stuff for next frame
    keyboard.resetKeys();
}