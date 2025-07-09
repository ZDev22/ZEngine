#pragma once

#include "model.hpp"
#include "texture.hpp"

#include <glm/glm.hpp>

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;
};

struct alignas(16) SpriteData {
    glm::vec2 position;
    glm::vec2 scale;
    glm::mat2 rotationMatrix;
    glm::vec4 color;

    uint32_t textureIndex;
    float rotation;

    void setRotationMatrix() { rotationMatrix = glm::mat2(cos(glm::radians(rotation)), -sin(glm::radians(rotation)), sin(glm::radians(rotation)), cos(glm::radians(rotation))); }
};