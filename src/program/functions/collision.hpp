#pragma once

#include "../../vulkan/model.hpp"
#include "../../vulkan/sprite.hpp"

#include <glm/glm.hpp>

#include <array>
#include <limits>
#include <vector>
#include <algorithm>

// Can run ~16000 times at ~60 fps
bool checkFastCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {
    dataA.rotationMatrix = glm::mat2(cos(dataA.rotation), -sin(dataA.rotation), sin(dataA.rotation),  cos(dataA.rotation));
    dataB.rotationMatrix = glm::mat2(cos(dataB.rotation), -sin(dataB.rotation), sin(dataB.rotation),  cos(dataB.rotation));

    glm::vec2 aMin(3.402823466e+38f, 3.402823466e+38f);
    glm::vec2 aMax(-3.402823466e+38f, -3.402823466e+38f);
    glm::vec2 bMin(3.402823466e+38f, 3.402823466e+38f);
    glm::vec2 bMax(-3.402823466e+38f, -3.402823466e+38f);

    for (const auto& vertex : spriteA.model->getVertices()) {
        glm::vec2 transformed = (dataA.rotationMatrix * (vertex.position * dataA.scale)) + dataA.translation;

        aMin.x = transformed.x < aMin.x ? transformed.x : aMin.x;
        aMin.y = transformed.y < aMin.y ? transformed.y : aMin.y;
        aMax.x = transformed.x > aMax.x ? transformed.x : aMax.x;
        aMax.y = transformed.y > aMax.y ? transformed.y : aMax.y;
    }

    for (const auto& vertex : spriteB.model->getVertices()) {
        glm::vec2 transformed = (dataB.rotationMatrix * (vertex.position * dataB.scale)) + dataB.translation;

        bMin.x = transformed.x < bMin.x ? transformed.x : bMin.x;
        bMin.y = transformed.y < bMin.y ? transformed.y : bMin.y;
        bMax.x = transformed.x > bMax.x ? transformed.x : bMax.x;
        bMax.y = transformed.y > bMax.y ? transformed.y : bMax.y;
    }

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) && (aMin.y <= bMax.y && aMax.y >= bMin.y);
}