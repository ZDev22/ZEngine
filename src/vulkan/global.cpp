#include "global.hpp"
#include "sprite.hpp"

std::shared_ptr<Model> squareModel;
vector<bool> queue;
vector<Sprite::SpriteData> sprites;
vector<Sprite::Sprite> spriteCPU;
vector<std::unique_ptr<Texture>> spriteTextures;

float deltaTime = .0f;

uint16_t imageCount;