#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/sprite.hpp"

#include <glm/glm.hpp>
#include <array>
#include <limits>
#include <vector>
#include <algorithm>

bool checkFastCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {
    dataA.rotationMatrix = glm::mat2(cos(dataA.rotation), -sin(dataA.rotation), sin(dataA.rotation),  cos(dataA.rotation));
    dataB.rotationMatrix = glm::mat2(cos(dataB.rotation), -sin(dataB.rotation), sin(dataB.rotation),  cos(dataB.rotation));

    glm::vec2 aMin(3.402823466e+38f, 3.402823466e+38f);
    glm::vec2 aMax(-3.402823466e+38f, -3.402823466e+38f);
    glm::vec2 bMin(3.402823466e+38f, 3.402823466e+38f);
    glm::vec2 bMax(-3.402823466e+38f, -3.402823466e+38f);

    const auto& verticesA = spriteA.model->getVertices();
    const auto& verticesB = spriteB.model->getVertices();

    if (verticesA.size() > verticesB.size()) {
        for (size_t i = 0; i < verticesB.size(); ++i) {
            glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            aMin.x = std::min(aMin.x, transformedA.x);
            aMin.y = std::min(aMin.y, transformedA.y);
            aMax.x = std::max(aMax.x, transformedA.x);
            aMax.y = std::max(aMax.y, transformedA.y);

            bMin.x = std::min(bMin.x, transformedB.x);
            bMin.y = std::min(bMin.y, transformedB.y);
            bMax.x = std::max(bMax.x, transformedB.x);
            bMax.y = std::max(bMax.y, transformedB.y);
        }
    }
    else if (verticesA.size() < verticesB.size()) {
        for (size_t i = 0; i < verticesB.size(); ++i) {
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            if (i < verticesA.size()) {
                glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;

                aMin.x = std::min(aMin.x, transformedA.x);
                aMin.y = std::min(aMin.y, transformedA.y);
                aMax.x = std::max(aMax.x, transformedA.x);
                aMax.y = std::max(aMax.y, transformedA.y);
            }

            bMin.x = std::min(bMin.x, transformedB.x);
            bMin.y = std::min(bMin.y, transformedB.y);
            bMax.x = std::max(bMax.x, transformedB.x);
            bMax.y = std::max(bMax.y, transformedB.y);
        }
    }
    else {
        for (size_t i = 0; i < verticesA.size(); ++i) {
            glm::vec2 transformedA = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.translation;
            glm::vec2 transformedB = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.translation;

            aMin.x = std::min(aMin.x, transformedA.x);
            aMin.y = std::min(aMin.y, transformedA.y);
            aMax.x = std::max(aMax.x, transformedA.x);
            aMax.y = std::max(aMax.y, transformedA.y);

            bMin.x = std::min(bMin.x, transformedB.x);
            bMin.y = std::min(bMin.y, transformedB.y);
            bMax.x = std::max(bMax.x, transformedB.x);
            bMax.y = std::max(bMax.y, transformedB.y);
        }
    }

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y);
}