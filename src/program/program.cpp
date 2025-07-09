#include "program.hpp"
#include "../vulkan/global.hpp"
#include "../vulkan/texture.hpp"
#include "functions/keyboard.hpp"
#include "functions/math.hpp"
#include "functions/collision.hpp"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

Program::Program(Keyboard& keyboard, Device& device, Pipeline& pipeline, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool) : keyboard(keyboard), device(device), pipeline(pipeline), descriptorSetLayout(descriptorSetLayout), descriptorPool(descriptorPool) {
    texture = Texture(device, filepaths, descriptorSetLayout, descriptorPool, pipeline);
}

float speedY = 0.f;
bool dead = false;
int score = 0;

void Program::tick() {
    speedY += 3.8f * deltaTime;
    sprites[0].position.y += speedY * deltaTime;
    sprites[0].rotation -= 90.f * deltaTime;

    if (!dead && keyboard.keyHit(GLFW_KEY_SPACE)) {
        speedY = -1.3f;
        sprites[0].rotation = 60.f;
    }

    for (size_t i = 1; i < (sprites.size()) / 2; i++) {
        size_t index = ((i - 1) * 2) + 1; // A reliable method of searching every other index in a list! (You can multiply by 3 for every third item, and so on)
        sprites[index].position.x -= .5f * deltaTime;
        if (sprites[index].position.x < -1.5f) {
            sprites[index].position.x = 1.5f;
            sprites[index].position.y = randomFloat(.4f, 1.4f);
            score++;
        }
        sprites[index + 1].position.x = sprites[index].position.x;
        sprites[index + 1].position.y = sprites[index].position.y - 2.f;

        if (!dead && (checkCollision(spriteCPU[index], sprites[index], spriteCPU[0], sprites[0]) || checkCollision(spriteCPU[index + 1], sprites[index + 1], spriteCPU[0], sprites[0]))) { 
            dead = true; 
            speedY = -1.5f; 
            sprites[0].rotation = 60.f;
            sprites[0].textureIndex = Texture::switchTexture(spriteCPU[0], "pipe.png");
        }
    }

    if (dead && sprites[0].position.y > 2.5f) {
        sprites[0].position.y = -.25f;
        sprites[0].rotation = 0.f;
        speedY = 0.f;
        dead = false;
    }



    //Reset stuff for next frame
    keyboard.resetKeys();
}