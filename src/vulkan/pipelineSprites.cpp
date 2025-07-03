#include "pipeline.hpp"
//#include "../program/functions/math.hpp"
#include <iostream>

void Pipeline::setTexture(int textureID) {
    spriteTextures[textureID] = std::make_unique<Texture>(
        device,
        texturePaths,
        descriptorSetLayout,
        descriptorPool,
        *this
    );
}

std::shared_ptr<Model> Pipeline::makeModel(const std::vector<glm::vec2>& positions) {
    glm::vec2 baseMin = {-0.5f, -0.5f};
    glm::vec2 baseSize = {1.0f, 1.0f};

    std::vector<Model::Vertex> vertices;
    for (const auto& pos : positions) {
        glm::vec2 uv = (pos - baseMin) / baseSize;
        vertices.push_back({ pos, uv });
    }

    std::vector<uint32_t> indices(positions.size());
    for (uint32_t i = 0; i < positions.size(); ++i) { indices[i] = i; }

    return std::make_shared<Model>(device, vertices, indices);
}

void Pipeline::loadSprites() {
    std::cout << "Starting sprite loading...\n";
    for (size_t f = 0; f < texturePaths.size(); f++) { texturePaths[f] = "images/" + texturePaths[f]; }

    sprites.clear();
    spriteCPU.clear();

    auto quadModel = makeModel({
        {-0.5f, -0.5f}, // Bottom-Left  (Vertex 0)
        { 0.5f, -0.5f}, // Bottom-Right (Vertex 1)
        {-0.5f,  0.5f}, // Top-Right    (Vertex 2)
        { 0.5f,  0.5f}  // Top-Left     (Vertex 3)
    });
    auto texture = std::make_unique<Texture>(device, texturePaths[0], descriptorSetLayout, descriptorPool, *this);

    Sprite sprite;
    SpriteData spriteData;

    sprite.model = quadModel;
    sprite.texture = texture.get();
    sprite.textureIndex = 0;

    spriteData.translation = glm::vec2(-.7f, -.2f);
    spriteData.scale = glm::vec2(.1f, .1f);
    spriteData.rotation = 0.0f;
    spriteData.color = glm::vec4(1.0f);
    spriteData.textureIndex = 0;

    sprites.push_back(spriteData);
    spriteCPU.push_back(sprite);
    spriteTextures.push_back(std::move(texture));

    // Re-use some values for the second sprite
    texture = std::make_unique<Texture>(device, texturePaths[1], descriptorSetLayout, descriptorPool, *this);
    sprite.texture = texture.get();
    sprite.textureIndex = 1;

    spriteData.textureIndex = 1;
    spriteData.scale = glm::vec2(.15f, 1.5f);

    for (int i = 1; i < 4; i++) {
        spriteData.translation = glm::vec2(1.f + (i / 2), 0.f);

        sprites.push_back(spriteData);
        spriteCPU.push_back(sprite);
        spriteTextures.push_back(std::move(texture));
    }

    std::cout << "Sprites created: " << sprites.size() << std::endl;
}