#pragma once

#include "model.hpp"
#include "texture.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <variant>

#define MAX_SPRITES 1000
#define MAX_TEXTURES 66

struct alignas(16) SpriteData {
    glm::vec2 position;
    glm::vec2 scale;
    glm::mat2 rotationMatrix;
    glm::vec4 color;
    glm::vec2 uvOffset;
    glm::vec2 uvScale;

    uint32_t textureIndex;
    float rotation;

    void setRotationMatrix() { rotationMatrix = glm::mat2(cos(glm::radians(rotation)), -sin(glm::radians(rotation)), sin(glm::radians(rotation)), cos(glm::radians(rotation))); }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;
    std::vector<std::variant<Sprite, SpriteData>> clones;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;