#pragma once

#include "model.hpp"
#include "texture.hpp"
#include "pipeline.hpp"

#include "../deps/ZDev/math.hpp"

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

struct alignas(16) SpriteData {
    float position[2];
    float scale[2];
    float rotationMatrix[4];
    float color[4];

    unsigned int textureIndex;
    float rotation;

    unsigned int ID;

    constexpr void setRotationMatrix() {
        rotationMatrix[0] = cos(radians(rotation));
        rotationMatrix[2] = sin(radians(rotation));
        rotationMatrix[1] = -rotationMatrix[2];
        rotationMatrix[3] = rotationMatrix[0];
    }
    void setText(const char* text, const char* font, float fontSize) { setTextSprite(text, font, fontSize, ID, textureIndex); }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;

    bool visible;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;

inline void setTextSprite(const char* text, const char* filepath, float fontSize, unsigned int spriteID, unsigned int oldTextureIndex) {
    for (unsigned int i = 0; i < sprites.size(); i++) { if (sprites[i].textureIndex >= sprites[spriteID].textureIndex) { sprite[i].textureIndex--; }}
    spriteTextures.erase(spriteTextures.begin() + oldTextureIndex);
    createText(text, filepath, fontSize);
    sprites[spriteID].textureIndex = spriteTextures.size() - 1;
}