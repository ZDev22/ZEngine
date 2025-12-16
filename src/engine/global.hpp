#pragma once

#include <memory>
#include <vector>

extern float deltaTime;

struct Push {
    float camera[2];
    float cameraZoom[2];
};

struct Texture;
struct QueuedTexture {
    std::unique_ptr<Texture> texture;
    unsigned int ID;
};

extern std::vector<QueuedTexture> queuedTextures;