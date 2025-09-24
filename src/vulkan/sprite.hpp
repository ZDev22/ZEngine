#pragma once

#include "model.hpp"
#include "texture.hpp"

#include "../deps/ZDev/math.hpp"

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

struct alignas(16) SpriteData {
    float[2] position;
    float[2] scale;
    float[4] rotationMatrix;
    float[4] color;

    uint32_t textureIndex;
    float rotation;

    constexpr void setRotationMatrix() { rotationMatrix = {cos(radians(rotation)), -sin(radians(rotation)), sin(radians(rotation)), cos(radians(rotation))}; }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;

    bool visible;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;