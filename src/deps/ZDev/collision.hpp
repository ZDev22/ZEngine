#pragma once

#include "../../vulkan/sprite.hpp"
#include "math.hpp"

// Collision for square objects like textures
inline bool checkSquareCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {
    dataA.setRotationMatrix();
    dataB.setRotationMatrix();

    return absoluteFloat(dataA.position.x - dataB.position.x) <= (absoluteFloat(dataA.rotationMatrix[0][0]) * (dataA.scale.x * .5f) + absoluteFloat(dataA.rotationMatrix[0][1]) * (dataA.scale.y * .5f) + absoluteFloat(dataB.rotationMatrix[0][0]) * (dataB.scale.x * .5f) + absoluteFloat(dataB.rotationMatrix[0][1]) * (dataB.scale.y * .5f)) && absoluteFloat(dataA.position.y - dataB.position.y) <= (absoluteFloat(dataA.rotationMatrix[0][1]) * (dataA.scale.x * .5f) + absoluteFloat(dataA.rotationMatrix[0][0]) * (dataA.scale.y * .5f) + absoluteFloat(dataB.rotationMatrix[0][1]) * (dataB.scale.x * .5f) + absoluteFloat(dataB.rotationMatrix[0][0]) * (dataB.scale.y * .5f));
}

// Can run 30k times at 75fps
inline bool checkCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {
    dataA.setRotationMatrix();
    dataB.setRotationMatrix();

    glm::vec2 aMin(3.402823466e+38f, 3.402823466e+38f);
    glm::vec2 aMax(-3.402823466e+38f, -3.402823466e+38f);
    glm::vec2 bMin(3.402823466e+38f, 3.402823466e+38f);
    glm::vec2 bMax(-3.402823466e+38f, -3.402823466e+38f);

    const auto& verticesA = spriteA.model->getVertices();
    const auto& verticesB = spriteB.model->getVertices();

    if (verticesA.size() > verticesB.size()) {
        for (size_t i = 0; i < verticesA.size(); i++) {
            glm::vec2 transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
            if (transformed.x < aMin.x) { aMin.x = transformed.x; }
            if (transformed.y < aMin.y) { aMin.y = transformed.y; }
            if (transformed.x > aMax.x) { aMax.x = transformed.x; }
            if (transformed.y > aMax.y) { aMax.y = transformed.y; }

            if (i < verticesB.size()) {
                transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
                if (transformed.x < bMin.x) { bMin.x = transformed.x; }
                if (transformed.y < bMin.y) { bMin.y = transformed.y; }
                if (transformed.x > bMax.x) { bMax.x = transformed.x; }
                if (transformed.y > bMax.y) { bMax.y = transformed.y; }
            }
        }
    } else if (verticesA.size() < verticesB.size()) {
        for (size_t i = 0; i < verticesB.size(); i++) {
            glm::vec2 transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
            if (transformed.x < bMin.x) { bMin.x = transformed.x; }
            if (transformed.y < bMin.y) { bMin.y = transformed.y; }
            if (transformed.x > bMax.x) { bMax.x = transformed.x; }
            if (transformed.y > bMax.y) { bMax.y = transformed.y; }

            if (i < verticesA.size()) {
                transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
                if (transformed.x < aMin.x) { aMin.x = transformed.x; }
                if (transformed.y < aMin.y) { aMin.y = transformed.y; }
                if (transformed.x > aMax.x) { aMax.x = transformed.x; }
                if (transformed.y > aMax.y) { aMax.y = transformed.y; }
            }
        }
    } else {
        for (size_t i = 0; i < verticesA.size(); i++) {
            glm::vec2 transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
            if (transformed.x < aMin.x) { aMin.x = transformed.x; }
            if (transformed.y < aMin.y) { aMin.y = transformed.y; }
            if (transformed.x > aMax.x) { aMax.x = transformed.x; }
            if (transformed.y > aMax.y) { aMax.y = transformed.y; }

            transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
            if (transformed.x < bMin.x) { bMin.x = transformed.x; }
            if (transformed.y < bMin.y) { bMin.y = transformed.y; }
            if (transformed.x > bMax.x) { bMax.x = transformed.x; }
            if (transformed.y > bMax.y) { bMax.y = transformed.y; }
        }
    }
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y);
}