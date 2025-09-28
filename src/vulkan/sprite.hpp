#pragma once

#include "model.hpp"
#include "texture.hpp"
#include "pipeline.hpp"

#include "../deps/ZDev/math.hpp"

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

inline void setTextSprite(const char* text, unsigned int font, float fontSize, unsigned int spriteID, Pipeline& pipeline);

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
    inline void setText(const char* text, unsigned int font, float fontSize, Pipeline& pipeline) { setTextSprite(text, font, fontSize, ID, pipeline); }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;

    bool visible;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;
extern std::vector<std::unique_ptr<Texture>> spriteTextures;

inline void setTextSprite(const char* text, unsigned int font, float fontSize, unsigned int spriteID, Pipeline& pipeline) {
    unsigned int i = 0;
    while (spriteTextures[i]->name != "e.jpg") { i++; }
    spriteTextures.erase(spriteTextures.begin() + i);
    for (unsigned int q = i; q < sprites.size(); q++) { if (sprites[q].textureIndex >= i) { sprites[q].textureIndex--; }}
    pipeline.createText(font, text, fontSize);
    sprites[spriteID].textureIndex = spriteTextures.size() - 1;
    textureUpdate = true;
}