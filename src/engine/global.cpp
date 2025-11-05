#include "global.hpp"
#include "sprite.hpp"

std::vector<SpriteData> sprites;
std::vector<Sprite> spriteCPU;
std::vector<std::unique_ptr<Texture>> spriteTextures;
std::vector<QueuedTexture> queuedTextures;