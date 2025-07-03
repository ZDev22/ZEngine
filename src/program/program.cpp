#include "program.hpp"
#include "../main.hpp"
#include "../vulkan/global.hpp"
#include "functions/keyboard.hpp"

// #define MINIAUDIO_IMPLEMENTATION
// #include "functions/deps/miniaudio.h"

#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <string>

Program::Program(Keyboard& keyboard) : keyboard(keyboard) {
    // ma_engine engine;
    // ma_engine_init(NULL, &engine);
    // ma_sound sound;
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

    for (size_t i = 1; i < sprites.size(); i++) {
        sprites[i].translation.x -= .5f * deltaTime;
        if (sprites[i].translation.x < -1.5f) {
            sprites[i].translation.x = 1.5f;
        }
    }


    // ma_sound_init_from_file(&engine, "audio.wav", 0, NULL, NULL, &sound);
    // ma_sound_start(&sound);

    //Reset stuff for next frame
    keyboard.resetKeys();
}