#pragma once

#include "model.hpp"
#include "texture.hpp"
#include "pipeline.hpp"

#include <math.h>

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

struct alignas(16) SpriteData {
    float position[2];
    float scale[2];
    float rotationMatrix[4];
    float color[4];

    unsigned int textureIndex;
    float rotation;

    constexpr void setRotationMatrix() {
        rotationMatrix[0] = cos(rotation * .0174532925f);
        rotationMatrix[2] = sin(rotation * .0174532925f);
        rotationMatrix[1] = -rotationMatrix[2];
        rotationMatrix[3] = rotationMatrix[0];
    }
    inline void setText(const char* text, unsigned int font, float fontSize, Pipeline& pipeline) { pipeline.createText(font, text, fontSize, textureIndex); }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;

    bool visible;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;