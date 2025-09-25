#pragma once

#include "model.hpp"
#include "texture.hpp"

#include "../deps/ZDev/math.hpp"

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

struct alignas(16) SpriteData {
    float position[2];
    float scale[2];
    float rotationMatrix[4];
    float color[4];

    uint32_t textureIndex;
    float rotation;

    constexpr void setRotationMatrix() {
        rotationMatrix[0] = cos(radians(rotation));
        rotationMatrix[1] = -sin(radians(rotation));
        rotationMatrix[2] = sin(radians(rotation));
        rotationMatrix[3] = cos(radians(rotation));
    }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;

    bool visible;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;