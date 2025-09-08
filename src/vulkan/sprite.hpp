#pragma once

#include "model.hpp"
#include "texture.hpp"

#include "../../deps/ZDev/vector.hpp"

#define MAX_SPRITES 100000
#define MAX_TEXTURES 66

class Sprite {
public:
    struct alignas(16) SpriteData {
        glm::vec2 position;
        glm::vec2 scale;
        glm::mat2 rotationMatrix;
        glm::vec4 color;

        unsigned int textureIndex;
        float rotation;

        constexpr void setRotationMatrix() { rotationMatrix = glm::mat2(cos(glm::radians(rotation)), -sin(glm::radians(rotation)), sin(glm::radians(rotation)), cos(glm::radians(rotation))); }
    };

    struct Sprite {
        std::shared_ptr<Model> model;
        Texture* texture;

        bool visible;
    };

    Sprite() {
        if (sprites.size() == 0) {
            ID = queue.size();
            queue.push_back(false); 
        }
        else {
            if (sprites.size() >= MAX_SPRITES) { throw("Maximum number of sprites exceeded!"); }
            Sprite sprite;
            SpriteData spriteData;

            sprite.model = squareModel;
            sprite.texture = spriteTextures[0].get();
            sprite.visible = true;

            spriteData.position = glm::vec2(0.f);
            spriteData.scale = glm::vec2(.25f);
            spriteData.rotation = 0.f;
            spriteData.color = glm::vec4(1.f);
            spriteData.textureIndex = 0;

            ID = sprites.size();
            sprites.push_back(spriteData);
            spriteCPU.push_back(sprite);
        }
    }

    constexpr inline glm::vec2 position() const { return sprites[ID].position; }
    constexpr inline glm::vec2 scale() const { return sprites[ID].scale; }
    constexpr inline float rotation() const { return sprites[ID].rotation; }
    constexpr inline glm::vec4 color() const { return sprites[ID].color; }
    constexpr inline unsigned int textureIndex() const { return sprites[ID].textureIndex; }

    constexpr inline void position(glm::vec2 position) const { return sprites[ID].position = position; }
    constexpr inline void scale(glm::vec2 scale) const { return sprites[ID].scale = scale; }
    constexpr inline void rotation(float rotation) const { return sprites[ID].rotation = rotation; }
    constexpr inline void color(glm::vec4 color) const { return sprites[ID].color = color; }
    constexpr inline void textureIndex(unsigned int textureIndex) const { return sprites[ID].textureIndex = textureIndex; }
private:
    unsigned int ID;
};

namespace sprite {
    void setBaseSpriteModel(std::shared_ptr<Model> model) { squareModel = model; }
    void clearQueue() {
        for (unsigned int i = 0; i < queue.size(); i++) {
            Sprite sprite;
            SpriteData spriteData;

            sprite.model = squareModel;
            sprite.texture = spriteTextures[0].get();
            sprite.visible = true;

            spriteData.position = glm::vec2(0.f);
            spriteData.scale = glm::vec2(.25f);
            spriteData.rotation = 0.f;
            spriteData.color = glm::vec4(1.f);
            spriteData.textureIndex = 0;

            ID = sprites.size();
            sprites.push_back(spriteData);
            spriteCPU.push_back(sprite);
        }
    }
}

extern vector<bool> queue;
extern std::shared_ptr<Model> squareModel;
extern vector<Sprite::SpriteData> sprites;
extern vector<Sprite::Sprite> spriteCPU;
extern vector<std::unique_ptr<Texture>> spriteTextures;