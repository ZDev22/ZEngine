#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/sprite.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>

bool checkCollision(const Sprite& spriteA, const SpriteData& dataA, const Sprite& spriteB, const SpriteData& dataB) {
    auto transformVertices = [](const std::vector<Model::Vertex>& vertices, const SpriteData& data) {
        std::vector<glm::vec2> transformed;
        transformed.reserve(vertices.size());

        for (const auto& vertex : vertices) {
            // Apply scale, rotation, then translation
            glm::vec2 scaled = vertex.position * data.scale;
            glm::vec2 rotated = data.rotationMatrix * scaled;
            glm::vec2 translated = rotated + data.translation;
            transformed.push_back(translated);
        }
        return transformed;
    };

    // Helper function to compute AABB from vertices
    auto getAABB = [](const std::vector<glm::vec2>& vertices) {
        if (vertices.empty()) {
            return std::pair<glm::vec2, glm::vec2>(glm::vec2(0.0f), glm::vec2(0.0f));
        }
        glm::vec2 min = vertices[0];
        glm::vec2 max = vertices[0];

        for (size_t i = 1; i < vertices.size(); ++i) {
            min.x = std::min(min.x, vertices[i].x);
            min.y = std::min(min.y, vertices[i].y);
            max.x = std::max(max.x, vertices[i].x);
            max.y = std::max(max.y, vertices[i].y);
        }
        return std::pair<glm::vec2, glm::vec2>(min, max);
    };

    // Get transformed vertices for both sprites
    auto verticesA = transformVertices(spriteA.model->getVertices(), dataA);
    auto verticesB = transformVertices(spriteB.model->getVertices(), dataB);

    // Compute AABBs
    auto [aMin, aMax] = getAABB(verticesA);
    auto [bMin, bMax] = getAABB(verticesB);

    // AABB collision check
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
           (aMin.y <= bMax.y && aMax.y >= bMin.y);
}