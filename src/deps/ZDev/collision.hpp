/* licensed under GPL v3.0 see https://github.com/ZDev22/Vulkan-Engine/ for current license

v1.2.2

collision.hpp is a lightweight cross-platform sigle-header cpp library for checking if two 2D objects are colliding!
Uses the AABB method to create a square around your object, more precise methods are planned.
Requires some sprite data from sprite.hpp, a not project-specific system is planned.

HOW TO USE: Collision collision;
collision.checkSquareCollision(spriteCPU[index], sprites[index], spriteCPU[0], sprites[0]);
collision.checkCollision requires sprite IDs which are currently not implemented yet.
*/

#pragma once

#include "engine/sprite.hpp"

struct Collision {
public:
    struct AABBS {
        float pos[4] = {3.402823466e+38f, 3.402823466e+38f, -3.402823466e+38f, -3.402823466e+38f};
        unsigned int ID;
    };

    inline float absolute(const float i) { return i < 0 ? -i : i; }

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

        if (usingAAABB != -1 && usingBAABB != -1) { return (AABB[usingAAABB].pos[0] <= AABB[usingBAABB].pos[2] && AABB[usingAAABB].pos[2] >= AABB[usingBAABB].pos[0]) && (AABB[usingAAABB].pos[1] <= AABB[usingBAABB].pos[3] && AABB[usingAAABB].pos[3] >= AABB[usingBAABB].pos[1]); }
        else if (usingAAABB != -1 && usingBAABB == -1) {
            AABBS b;
            b.ID = spriteBID;
            const auto& verticesB = spriteB.model->getVertices();

            for (unsigned int i = 0; i < verticesB.size(); i++) {
                float pxB = verticesB[i].position[0] * dataB.scale[0];
                float pyB = verticesB[i].position[1] * dataB.scale[1];
                float transformedB[2] = { dataB.rotationMatrix[0] * pxB + dataB.rotationMatrix[1] * pyB, dataB.rotationMatrix[2] * pxB + dataB.rotationMatrix[3] * pyB};
                transformedB[0] += dataB.position[0];
                transformedB[1] += dataB.position[1];
                if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];
            }

            AABB.push_back(b);
            return (AABB[usingAAABB].pos[0] <= b.pos[2] && AABB[usingAAABB].pos[2] >= b.pos[0]) && (AABB[usingAAABB].pos[1] <= b.pos[3] && AABB[usingAAABB].pos[3] >= b.pos[1]);
        }
        else if (usingAAABB == -1 && usingBAABB != -1) {
            AABBS a;
            a.ID = spriteAID;
            const auto& verticesA = spriteA.model->getVertices();

            for (unsigned int i = 0; i < verticesA.size(); i++) {
                float pxA = verticesA[i].position[0] * dataA.scale[0];
                float pyA = verticesA[i].position[1] * dataA.scale[1];
                float transformedA[2] = { dataA.rotationMatrix[0] * pxA + dataA.rotationMatrix[1] * pyA, dataA.rotationMatrix[2] * pxA + dataA.rotationMatrix[3] * pyA};
                transformedA[0] += dataA.position[0];
                transformedA[1] += dataA.position[1];
                if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];
            }

            AABB.push_back(a);
            return (a.pos[0] <= AABB[usingBAABB].pos[2] && a.pos[2] >= AABB[usingBAABB].pos[0]) && (a.pos[1] <= AABB[usingBAABB].pos[3] && a.pos[3] >= AABB[usingBAABB].pos[1]);
        }
        else {
            dataA.setRotationMatrix();
            dataB.setRotationMatrix();

            AABBS a;
            a.ID = spriteAID;
            AABBS b;
            b.ID = spriteBID;

            const auto& verticesA = spriteA.model->getVertices();
            const auto& verticesB = spriteB.model->getVertices();

            if (verticesA.size() > verticesB.size()) {
                for (unsigned int i = 0; i < verticesA.size(); i++) {
                    float pxA = verticesA[i].position[0] * dataA.scale[0];
                    float pyA = verticesA[i].position[1] * dataA.scale[1];
                    float transformedA[2] = { dataA.rotationMatrix[0] * pxA + dataA.rotationMatrix[1] * pyA, dataA.rotationMatrix[2] * pxA + dataA.rotationMatrix[3] * pyA};
                    transformedA[0] += dataA.position[0];
                    transformedA[1] += dataA.position[1];
                    if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                    if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                    if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                    if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];

                    if (i < verticesB.size()) {
                        float pxB = verticesB[i].position[0] * dataB.scale[0];
                        float pyB = verticesB[i].position[1] * dataB.scale[1];
                        float transformedB[2] = { dataB.rotationMatrix[0] * pxB + dataB.rotationMatrix[1] * pyB, dataB.rotationMatrix[2] * pxB + dataB.rotationMatrix[3] * pyB};
                        transformedB[0] += dataB.position[0];
                        transformedB[1] += dataB.position[1];
                        if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                        if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                        if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                        if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];
                    }
                }
            }
            else if (verticesA.size() < verticesB.size()) {
                for (unsigned int i = 0; i < verticesB.size(); i++) {
                    float pxB = verticesB[i].position[0] * dataB.scale[0];
                    float pyB = verticesB[i].position[1] * dataB.scale[1];
                    float transformedB[2] = {
                        dataB.rotationMatrix[0] * pxB + dataB.rotationMatrix[1] * pyB,
                        dataB.rotationMatrix[2] * pxB + dataB.rotationMatrix[3] * pyB
                    };
                    transformedB[0] += dataB.position[0];
                    transformedB[1] += dataB.position[1];

                    if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                    if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                    if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                    if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];

                    if (i < verticesA.size()) {
                        float pxA = verticesA[i].position[0] * dataA.scale[0];
                        float pyA = verticesA[i].position[1] * dataA.scale[1];
                        float transformedA[2] = { dataA.rotationMatrix[0] * pxA + dataA.rotationMatrix[1] * pyA, dataA.rotationMatrix[2] * pxA + dataA.rotationMatrix[3] * pyA};
                        transformedA[0] += dataA.position[0];
                        transformedA[1] += dataA.position[1];
                        if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                        if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                        if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                        if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];
                    }
                }
            }
            else {
                for (unsigned int i = 0; i < verticesA.size(); i++) {
                    float pxA = verticesA[i].position[0] * dataA.scale[0];
                    float pyA = verticesA[i].position[1] * dataA.scale[1];
                    float transformedA[2] = { dataA.rotationMatrix[0] * pxA + dataA.rotationMatrix[1] * pyA, dataA.rotationMatrix[2] * pxA + dataA.rotationMatrix[3] * pyA};
                    transformedA[0] += dataA.position[0];
                    transformedA[1] += dataA.position[1];

                    if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                    if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                    if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                    if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];

                    float pxB = verticesB[i].position[0] * dataB.scale[0];
                    float pyB = verticesB[i].position[1] * dataB.scale[1];
                    float transformedB[2] = { dataB.rotationMatrix[0] * pxB + dataB.rotationMatrix[1] * pyB, dataB.rotationMatrix[2] * pxB + dataB.rotationMatrix[3] * pyB};
                    transformedB[0] += dataB.position[0];
                    transformedB[1] += dataB.position[1];

                    if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                    if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                    if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                    if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];
                }
            }
            AABB.push_back(a);
            AABB.push_back(b);
            return (a.pos[0] <= b.pos[2] && a.pos[2] >= b.pos[0]) && (a.pos[1] <= b.pos[3] && a.pos[3] >= b.pos[1]);
        }
    }

    void clearAABB() { AABB.clear(); }

private:
    std::vector<AABBS> AABB;
};