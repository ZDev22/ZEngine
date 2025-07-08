#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/sprite.hpp"

#include <glm/glm.hpp>

#include <limits>
#include <vector>
#include <algorithm>

// Currently really slow! IDK how to fix it tho
bool checkCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {

    dataA.rotationMatrix = glm::mat2(cos(dataA.rotation), -sin(dataA.rotation), sin(dataA.rotation), cos(dataA.rotation));
    dataB.rotationMatrix = glm::mat2(cos(dataB.rotation), -sin(dataB.rotation), sin(dataB.rotation), cos(dataB.rotation));

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
    auto getAABB = [](const std::vector<glm::vec2>& vertices) -> std::pair<glm::vec2, glm::vec2> {
        glm::vec2 min(3.402823466e+38f, 3.402823466e+38f);
        glm::vec2 max(-3.402823466e+38f, -3.402823466e+38f);

        for (const auto& vertex : vertices) {
            min.x = vertex.x < min.x ? vertex.x : min.x;
            min.y = vertex.y < min.y ? vertex.y : min.y;
            max.x = vertex.x > max.x ? vertex.x : max.x;
            max.y = vertex.y > max.y ? vertex.y : max.y;
        }

        return {min, max};
    };

    // Get transformed vertices for both sprites
    auto verticesA = transformVertices(spriteA.model->getVertices(), dataA);
    auto verticesB = transformVertices(spriteB.model->getVertices(), dataB);

    // Compute AABBs
    auto [aMin, aMax] = getAABB(verticesA);
    auto [bMin, bMax] = getAABB(verticesB);

    // AABB collision check
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y);
}