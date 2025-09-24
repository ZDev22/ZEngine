#pragma once

#include "../../vulkan/sprite.hpp"
#include "math.hpp"

class Collision {
public:
    struct AABBS {
        float min[2] = {3.402823466e+38f, 3.402823466e+38f};
        float max[2] = {-3.402823466e+38f, -3.402823466e+38f};
        unsigned int ID;
    };

    bool checkSquareCollision(const Sprite& spriteA, SpriteData& dataA, const Sprite& spriteB, SpriteData& dataB) {
        dataA.setRotationMatrix();
        dataB.setRotationMatrix();
        return absolute(dataA.position[0] - dataB.position[0]) <= (absolute(dataA.rotationMatrix[0]) * (dataA.scale[0] * 0.5f) + absolute(dataA.rotationMatrix[1]) * (dataA.scale[1] * 0.5f) + absolute(dataB.rotationMatrix[0]) * (dataB.scale[0] * 0.5f) + absolute(dataB.rotationMatrix[1]) * (dataB.scale[1] * 0.5f)) && absolute(dataA.position[1] - dataB.position[1]) <= (absolute(dataA.rotationMatrix[2]) * (dataA.scale[0] * 0.5f) + absolute(dataA.rotationMatrix[3]) * (dataA.scale[1] * 0.5f) + absolute(dataB.rotationMatrix[2]) * (dataB.scale[0] * 0.5f) + absolute(dataB.rotationMatrix[3]) * (dataB.scale[1] * 0.5f));
    }

    bool checkCollision(const Sprite& spriteA, SpriteData& dataA, const unsigned int spriteAID, const Sprite& spriteB, SpriteData& dataB, const unsigned int spriteBID) {
        int usingAAABB = -1;
        int usingBAABB = -1;

        for (unsigned int i = 0; i < AABB.size(); i++) {
            if (AABB[i].ID == spriteAID) { usingAAABB = i; }
            else if (AABB[i].ID == spriteBID) { usingBAABB = i; }
        }

        if (usingAAABB != -1 && usingBAABB != -1) { return (AABB[usingAAABB].min[0] <= AABB[usingBAABB].max[0] && AABB[usingAAABB].max[0] >= AABB[usingBAABB].min[0]) && (AABB[usingAAABB].min[1] <= AABB[usingBAABB].max[1] && AABB[usingAAABB].max[1] >= AABB[usingBAABB].min[1]); }
        else if (usingAAABB != -1 && usingBAABB == -1) {
            AABBS b;
            b.ID = spriteBID;
            const auto& verticesB = spriteB.model->getVertices();

            for (size_t i = 0; i < verticesB.size(); i++) {
                float px = verticesB[i].position[0] * dataB.scale[0];
                float py = verticesB[i].position[1] * dataB.scale[1];
                float transformed[2] = { dataB.rotationMatrix[0] * px + dataB.rotationMatrix[1] * py, dataB.rotationMatrix[2] * px + dataB.rotationMatrix[3] * py};
                transformed[0] += dataB.position[0];
                transformed[1] += dataB.position[1];
                if (transformed[0] < b.min[0]) b.min[0] = transformed[0];
                if (transformed[1] < b.min[1]) b.min[1] = transformed[1];
                if (transformed[0] > b.max[0]) b.max[0] = transformed[0];
                if (transformed[1] > b.max[1]) b.max[1] = transformed[1];
            }

            AABB.push_back(b);
            return (AABB[usingAAABB].min[0] <= b.max[0] && AABB[usingAAABB].max[0] >= b.min[0]) && (AABB[usingAAABB].min[1] <= b.max[1] && AABB[usingAAABB].max[1] >= b.min[1]);
        }
        else if (usingAAABB == -1 && usingBAABB != -1) {
            AABBS a;
            a.ID = spriteAID;
            const auto& verticesA = spriteA.model->getVertices();

            for (size_t i = 0; i < verticesA.size(); i++) {
                float px = verticesA[i].position[0] * dataA.scale[0];
                float py = verticesA[i].position[1] * dataA.scale[1];
                float transformed[2] = { dataA.rotationMatrix[0] * px + dataA.rotationMatrix[1] * py, dataA.rotationMatrix[2] * px + dataA.rotationMatrix[3] * py};
                transformed[0] += dataA.position[0];
                transformed[1] += dataA.position[1];
                if (transformed[0] < a.min[0]) a.min[0] = transformed[0];
                if (transformed[1] < a.min[1]) a.min[1] = transformed[1];
                if (transformed[0] > a.max[0]) a.max[0] = transformed[0];
                if (transformed[1] > a.max[1]) a.max[1] = transformed[1];
            }

            AABB.push_back(a);
            return (a.min[0] <= AABB[usingBAABB].max[0] && a.max[0] >= AABB[usingBAABB].min[0]) && (a.min[1] <= AABB[usingBAABB].max[1] && a.max[1] >= AABB[usingBAABB].min[1]);
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
                    float px = verticesA[i].position[0] * dataA.scale[0];
                    float py = verticesA[i].position[1] * dataA.scale[1];
                    float transformed[2] = { dataA.rotationMatrix[0] * px + dataA.rotationMatrix[1] * py, dataA.rotationMatrix[2] * px + dataA.rotationMatrix[3] * py};
                    transformed[0] += dataA.position[0];
                    transformed[1] += dataA.position[1];
                    if (transformed[0] < a.min[0]) a.min[0] = transformed[0];
                    if (transformed[1] < a.min[1]) a.min[1] = transformed[1];
                    if (transformed[0] > a.max[0]) a.max[0] = transformed[0];
                    if (transformed[1] > a.max[1]) a.max[1] = transformed[1];

                    if (i < verticesB.size()) {
                        float px = verticesB[i].position[0] * dataB.scale[0];
                        float py = verticesB[i].position[1] * dataB.scale[1];
                        float transformed[2] = { dataB.rotationMatrix[0] * px + dataB.rotationMatrix[1] * py, dataB.rotationMatrix[2] * px + dataB.rotationMatrix[3] * py};
                        transformed[0] += dataB.position[0];
                        transformed[1] += dataB.position[1];
                        if (transformed[0] < b.min[0]) b.min[0] = transformed[0];
                        if (transformed[1] < b.min[1]) b.min[1] = transformed[1];
                        if (transformed[0] > b.max[0]) b.max[0] = transformed[0];
                        if (transformed[1] > b.max[1]) b.max[1] = transformed[1];
                    }
                }
            }
            else if (verticesA.size() < verticesB.size()) {
                for (size_t i = 0; i < verticesB.size(); i++) {
                    float px = verticesB[i].position[0] * dataB.scale[0];
                    float py = verticesB[i].position[1] * dataB.scale[1];
                    float transformed[2] = {
                        dataB.rotationMatrix[0] * px + dataB.rotationMatrix[1] * py,
                        dataB.rotationMatrix[2] * px + dataB.rotationMatrix[3] * py
                    };
                    transformed[0] += dataB.position[0];
                    transformed[1] += dataB.position[1];

                    if (transformed[0] < b.min[0]) b.min[0] = transformed[0];
                    if (transformed[1] < b.min[1]) b.min[1] = transformed[1];
                    if (transformed[0] > b.max[0]) b.max[0] = transformed[0];
                    if (transformed[1] > b.max[1]) b.max[1] = transformed[1];

                    if (i < verticesA.size()) {
                        float px = verticesA[i].position[0] * dataA.scale[0];
                        float py = verticesA[i].position[1] * dataA.scale[1];
                        float transformed[2] = { dataA.rotationMatrix[0] * px + dataA.rotationMatrix[1] * py, dataA.rotationMatrix[2] * px + dataA.rotationMatrix[3] * py};
                        transformed[0] += dataA.position[0];
                        transformed[1] += dataA.position[1];
                        if (transformed[0] < a.min[0]) a.min[0] = transformed[0];
                        if (transformed[1] < a.min[1]) a.min[1] = transformed[1];
                        if (transformed[0] > a.max[0]) a.max[0] = transformed[0];
                        if (transformed[1] > a.max[1]) a.max[1] = transformed[1];
                    }
                }
            }
            else {
                for (size_t i = 0; i < verticesA.size(); i++) {
                    float px = verticesA[i].position[0] * dataA.scale[0];
                    float py = verticesA[i].position[1] * dataA.scale[1];
                    float transformed[2] = { dataA.rotationMatrix[0] * px + dataA.rotationMatrix[1] * py, dataA.rotationMatrix[2] * px + dataA.rotationMatrix[3] * py};
                    transformed[0] += dataA.position[0];
                    transformed[1] += dataA.position[1];

                    if (transformed[0] < a.min[0]) a.min[0] = transformed[0];
                    if (transformed[1] < a.min[1]) a.min[1] = transformed[1];
                    if (transformed[0] > a.max[0]) a.max[0] = transformed[0];
                    if (transformed[1] > a.max[1]) a.max[1] = transformed[1];

                    float px = verticesB[i].position[0] * dataB.scale[0];
                    float py = verticesB[i].position[1] * dataB.scale[1];
                    float transformed[2] = { dataB.rotationMatrix[0] * px + dataB.rotationMatrix[1] * py, dataB.rotationMatrix[2] * px + dataB.rotationMatrix[3] * py};
                    transformed[0] += dataB.position[0];
                    transformed[1] += dataB.position[1];

                    if (transformed[0] < b.min[0]) b.min[0] = transformed[0];
                    if (transformed[1] < b.min[1]) b.min[1] = transformed[1];
                    if (transformed[0] > b.max[0]) b.max[0] = transformed[0];
                    if (transformed[1] > b.max[1]) b.max[1] = transformed[1];
                }
            }
            AABB.push_back(a);
            AABB.push_back(b);
            return (a.min[0] <= b.max[0] && a.max[0] >= b.min[0]) && (a.min[1] <= b.max[1] && a.max[1] >= b.min[1]);
        }
    }

    void clearAABB() { AABB.clear(); }
    
private:
    std::vector<AABBS> AABB;
};