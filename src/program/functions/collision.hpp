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
            aMin.x = transformedA.x < aMin.x ? transformedA.x : aMin.x;
            aMin.y = transformedA.y < aMin.y ? transformedA.y : aMin.y;
            aMax.x = transformedA.x > aMax.x ? transformedA.x : aMax.x;
            aMax.y = transformedA.y > aMax.y ? transformedA.y : aMax.y;

            if (i < verticesB.size()) {
                glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;
                bMin.x = transformedB.x < bMin.x ? transformedB.x : bMin.x;
                bMin.y = transformedB.y < bMin.y ? transformedB.y : bMin.y;
                bMax.x = transformedB.x > bMax.x ? transformedB.x : bMax.x;
                bMax.y = transformedB.y > bMax.y ? transformedB.y : bMax.y;
            }
        }
    }
    else if (verticesA.size() < verticesB.size()) {
        for (size_t i = 0; i < verticesB.size(); i++) {
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            if (i < verticesA.size()) {
                glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;

                aMin.x = transformedA.x < aMin.x ? transformedA.x : aMin.x;
                aMin.y = transformedA.y < aMin.y ? transformedA.y : aMin.y;
                aMax.x = transformedA.x > aMax.x ? transformedA.x : aMax.x;
                aMax.y = transformedA.y > aMax.y ? transformedA.y : aMax.y;
            }

            bMin.x = transformedB.x < bMin.x ? transformedB.x : bMin.x;
            bMin.y = transformedB.y < bMin.y ? transformedB.y : bMin.y;
            bMax.x = transformedB.x > bMax.x ? transformedB.x : bMax.x;
            bMax.y = transformedB.y > bMax.y ? transformedB.y : bMax.y;
        }
    }
    else {
        for (size_t i = 0; i < verticesA.size(); i++) {
            glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            aMin.x = transformedA.x < aMin.x ? transformedA.x : aMin.x;
            aMin.y = transformedA.y < aMin.y ? transformedA.y : aMin.y;
            aMax.x = transformedA.x > aMax.x ? transformedA.x : aMax.x;
            aMax.y = transformedA.y > aMax.y ? transformedA.y : aMax.y;

            bMin.x = transformedB.x < bMin.x ? transformedB.x : bMin.x;
            bMin.y = transformedB.y < bMin.y ? transformedB.y : bMin.y;
            bMax.x = transformedB.x > bMax.x ? transformedB.x : bMax.x;
            bMax.y = transformedB.y > bMax.y ? transformedB.y : bMax.y;
        }
    }
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y);
}