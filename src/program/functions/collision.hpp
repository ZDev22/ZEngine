#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/sprite.hpp"

#include <glm/glm.hpp>

#include <array>
#include <limits>
#include <vector>
#include <algorithm>

// Can run 30K times at 75 fps (with 4 vertex points)
bool checkCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {

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
            glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;
            if (transformedA.x < aMin.x) { aMin.x = transformedA.x; }
            if (transformedA.y < aMin.y) { aMin.y = transformedA.y; }
            if (transformedA.x > aMax.x) { aMax.x = transformedA.x; }
            if (transformedA.y > aMax.y) { aMax.y = transformedA.y; }

            if (i < verticesB.size()) {
                glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;
                if (transformedB.x < bMin.x) { bMin.x = transformedB.x; }
                if (transformedB.y < bMin.y) { bMin.y = transformedB.y; }
                if (transformedB.x > bMax.x) { bMax.x = transformedB.x; }
                if (transformedB.y > bMax.y) { bMax.y = transformedB.y; }
            }
        }
    }
    else if (verticesA.size() < verticesB.size()) {
        for (size_t i = 0; i < verticesB.size(); i++) {
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            if (i < verticesA.size()) {
                glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;

                if (transformedA.x < aMin.x) { aMin.x = transformedA.x; }
                if (transformedA.y < aMin.y) { aMin.y = transformedA.y; }
                if (transformedA.x > aMax.x) { aMax.x = transformedA.x; }
                if (transformedA.y > aMax.y) { aMax.y = transformedA.y; }
            }

            if (transformedB.x < bMin.x) { bMin.x = transformedB.x; }
            if (transformedB.y < bMin.y) { bMin.y = transformedB.y; }
            if (transformedB.x > bMax.x) { bMax.x = transformedB.x; }
            if (transformedB.y > bMax.y) { bMax.y = transformedB.y; }
        }
    }
    else {
        for (size_t i = 0; i < verticesA.size(); i++) {
            glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            if (transformedA.x < aMin.x) { aMin.x = transformedA.x; }
            if (transformedA.y < aMin.y) { aMin.y = transformedA.y; }
            if (transformedA.x > aMax.x) { aMax.x = transformedA.x; }
            if (transformedA.y > aMax.y) { aMax.y = transformedA.y; }

            if (transformedB.x < bMin.x) { bMin.x = transformedB.x; }
            if (transformedB.y < bMin.y) { bMin.y = transformedB.y; }
            if (transformedB.x > bMax.x) { bMax.x = transformedB.x; }
            if (transformedB.y > bMax.y) { bMax.y = transformedB.y; }
        }
    }
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y);
}