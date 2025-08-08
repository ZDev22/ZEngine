#pragma once

#include "window.hpp"

#include <glm/glm.hpp>

extern float deltaTime;
extern uint16_t imageCount;

struct Push { 
    glm::mat4 projection; 
    glm::vec2 camera;
    uint16_t textures;
};