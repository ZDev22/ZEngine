#pragma once

#include "../../vulkan/sprite.hpp"
#include "math.hpp"

class Collision {
public:
    struct AABBS {
        glm::vec2 min(3.402823466e+38f, 3.402823466e+38f);
        glm::vec2 max(-3.402823466e+38f, -3.402823466e+38f);
        unsigned int ID:
    };

    bool checkSquareCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) { // Collision for square objects like textures
        dataA.setRotationMatrix();
        dataB.setRotationMatrix();

        return absoluteFloat(dataA.position.x - dataB.position.x) <= (absoluteFloat(dataA.rotationMatrix[0][0]) * (dataA.scale.x * .5f) + absoluteFloat(dataA.rotationMatrix[0][1]) * (dataA.scale.y * .5f) + absoluteFloat(dataB.rotationMatrix[0][0]) * (dataB.scale.x * .5f) + absoluteFloat(dataB.rotationMatrix[0][1]) * (dataB.scale.y * .5f)) && absoluteFloat(dataA.position.y - dataB.position.y) <= (absoluteFloat(dataA.rotationMatrix[0][1]) * (dataA.scale.x * .5f) + absoluteFloat(dataA.rotationMatrix[0][0]) * (dataA.scale.y * .5f) + absoluteFloat(dataB.rotationMatrix[0][1]) * (dataB.scale.x * .5f) + absoluteFloat(dataB.rotationMatrix[0][0]) * (dataB.scale.y * .5f));
    }

    bool checkCollision(const Sprite& spriteA, SpriteData& dataA, const unsigned int spriteAID, const Sprite& spriteB, SpriteData& dataB, const unsigned int spriteBID) { // Calculate AABB for sprites with more than 4 verticies, save data to be used later the same frame
        int usingAAABB = -1;
        int usingBAABB = -1;
        for (unsigned int i = 0; i < AABB.size(); i++) {
            if (AABB[i].ID == spriteAID) { usingAAABB = i; }
            else if (AABB[i].ID == spriteBID) { usingBAABB = i; }
        }

        if (usingAAABB > -1 && usingBAABB > -1) { return (AABB[usingAAABB].min.x <= AABB[usingBAABB].max.x && AABB[usingAAABB].max.x >= AABB[usingBAABB].min.x) && (AABB[usingAAABB].min.y <= AABB[usingBAABB].max.y && AABB[usingAAABB].max.y >= AABB[usingBAABB].min.y); }
        else if (usingAAABB > -1 && usingBAABB == -1) {
            AABBS b;
            b.ID = spriteBID;

            const auto& verticesB = spriteB.model->getVertices();
            for (size_t i = 0; i < verticesB.size(); i++) {
                glm::vec2 transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
                if (transformed.x < b.min.x) { b.min.x = transformed.x; }
                if (transformed.y < b.min.y) { b.min.y = transformed.y; }
                if (transformed.x > b.max.x) { b.max.x = transformed.x; }
                if (transformed.y > b.max.y) { b.max.y = transformed.y; }
            }

            AABB.push_back(b);
            return (AABB[usingAAABB].min.x <= b.max.x && AABB[usingAAABB].max.x >= b.min.x) && (AABB[usingAAABB].min.y <= b.max.y && AABB[usingAAABB].max.y >= b.min.y);
        }
        else if (usingAAABB == -1 && usingBAABB > -1) {
            AABBS a;
            a.ID = spriteAID;

            const auto& verticesA = spriteA.model->getVertices();
            for (size_t i = 0; i < verticesA.size(); i++) {
                glm::vec2 transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
                if (transformed.x < a.min.x) { a.min.x = transformed.x; }
                if (transformed.y < a.min.y) { a.min.y = transformed.y; }
                if (transformed.x > a.max.x) { a.max.x = transformed.x; }
                if (transformed.y > a.max.y) { a.max.y = transformed.y; }
            }

            AABB.push_back(a);
            return (a.min.x <= AABB[usingBAABB].max.x && a.max.x >= AABB[usingBAABB].min.x) && (a.min.y <= AABB[usingBAABB].max.y && a.max.y >= AABB[usingBAABB].min.y);
        }
        else if (usingAAABB == -1 && usingBAABB == -1) {
            dataA.setRotationMatrix();
            dataB.setRotationMatrix();

            AABBS a;
            a.ID = spriteAID;
            AABBS b;
            b.ID = spriteBID;

            const auto& verticesA = spriteA.model->getVertices();
            const auto& verticesB = spriteB.model->getVertices();

            if (verticesA.size() > verticesB.size()) {
                for (size_t i = 0; i < verticesA.size(); i++) {
                    glm::vec2 transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
                    if (transformed.x < a.min.x) { a.min.x = transformed.x; }
                    if (transformed.y < a.min.y) { a.min.y = transformed.y; }
                    if (transformed.x > a.max.x) { a.max.x = transformed.x; }
                    if (transformed.y > a.max.y) { a.max.y = transformed.y; }

                    if (i < verticesB.size()) {
                        transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
                        if (transformed.x < b.min.x) { b.min.x = transformed.x; }
                        if (transformed.y < b.min.y) { b.min.y = transformed.y; }
                        if (transformed.x > b.max.x) { b.max.x = transformed.x; }
                        if (transformed.y > b.max.y) { b.max.y = transformed.y; }
                    }
                }
            } else if (verticesA.size() < verticesB.size()) {
                for (size_t i = 0; i < verticesB.size(); i++) {
                    glm::vec2 transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
                    if (transformed.x < b.min.x) { b.min.x = transformed.x; }
                    if (transformed.y < b.min.y) { b.min.y = transformed.y; }
                    if (transformed.x > b.max.x) { b.max.x = transformed.x; }
                    if (transformed.y > b.max.y) { b.max.y = transformed.y; }

                    if (i < verticesA.size()) {
                        transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
                        if (transformed.x < a.min.x) { a.min.x = transformed.x; }
                        if (transformed.y < a.min.y) { a.min.y = transformed.y; }
                        if (transformed.x > a.max.x) { a.max.x = transformed.x; }
                        if (transformed.y > a.max.y) { a.max.y = transformed.y; }
                    }
                }
            } else {
                for (size_t i = 0; i < verticesA.size(); i++) {
                    glm::vec2 transformed = (dataA.rotationMatrix * (verticesA[i].position * dataA.scale)) + dataA.position;
                    if (transformed.x < a.min.x) { a.min.x = transformed.x; }
                    if (transformed.y < a.min.y) { a.min.y = transformed.y; }
                    if (transformed.x > a.max.x) { a.max.x = transformed.x; }
                    if (transformed.y > a.max.y) { a.max.y = transformed.y; }

                    transformed = (dataB.rotationMatrix * (verticesB[i].position * dataB.scale)) + dataB.position;
                    if (transformed.x < b.min.x) { b.min.x = transformed.x; }
                    if (transformed.y < b.min.y) { b.min.y = transformed.y; }
                    if (transformed.x > b.max.x) { b.max.x = transformed.x; }
                    if (transformed.y > b.max.y) { b.max.y = transformed.y; }
                }
            }
            AABB.push_back(a);
            AABB.push_back(b);
            return (a.min.x <= b.max.x && a.max.x >= b.min.x) && (a.min.y <= b.max.y && a.max.y >= b.min.y);
        }
    }

    void clearAABB() { AABB.clear(); }
private:
    vector<AABBS> AABB;
};