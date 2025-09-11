#pragma once

#include "model.hpp"
#include "texture.hpp"

#include "../deps/ZDev/math.hpp"

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

struct alignas(16) SpriteData {
    glm::vec2 position;
    glm::vec2 scale;
    glm::mat2 rotationMatrix;
    ///glm::vec4 color;

    uint32_t textureIndex;
    float rotation;

    constexpr void setRotationMatrix() { rotationMatrix = glm::mat2(cos(radians(rotation))), -sin(radians(rotation)), sin(radians(rotation)), cos(radians(rotation)); }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;

    bool visible;
    bool AABB;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;