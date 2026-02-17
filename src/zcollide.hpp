/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v2.2.7

zcollide.hpp is a lightweight cross-platform sigle-header cpp library for checking if two 2D objects are colliding!
uses the AABB method to create a square hitbox around your object, more precise methods are planned.

#define ZCOLLIDE_IMPLEMENTATION - define in MAIN.CPP ONLY

HOW TO USE:
checkSquareCollision(sprites[0], sprites[1]); - check the collision between two squares
checkCollision(sprites[0], sprites[1]); - check the collision between two sprites
zcollide_clearAABB() - reset AABB cache if sprites are moving
*/

#pragma once

#include "zengine.hpp"

struct ZCOLLIDE_AABBS {
    float pos[4] = {3.402823466e+38f, 3.402823466e+38f, -3.402823466e+38f, -3.402823466e+38f};
    unsigned int ID;
};

bool zcollide_checkSquareCollision(Sprite& dataA, Sprite& dataB);
bool zcollide_checkCollision(Sprite& dataA, Sprite& spriteB);
void zcollide_clearAABB();

#ifdef ZCOLLIDE_IMPLEMENTATION

#include <math.h>

std::vector<ZCOLLIDE_AABBS> zcollide_AABB;

bool zcollide_checkSquareCollision(Sprite& spriteA, Sprite& spriteB) {
    spriteA.setRotationMatrix();
    spriteB.setRotationMatrix();
    return abs(spriteA.position[0] - spriteB.position[0]) <= (abs(spriteA.rotationMatrix[0]) * (spriteA.scale[0] * .5f) + abs(spriteA.rotationMatrix[1]) * (spriteA.scale[1] * .5f) + abs(spriteB.rotationMatrix[0]) * (spriteB.scale[0] * .5f) + abs(spriteB.rotationMatrix[1]) * (spriteB.scale[1] * .5f)) && abs(spriteA.position[1] - spriteB.position[1]) <= (abs(spriteA.rotationMatrix[2]) * (spriteA.scale[0] * 0.5f) + abs(spriteA.rotationMatrix[3]) * (spriteA.scale[1] * .5f) + abs(spriteB.rotationMatrix[2]) * (spriteB.scale[0] * .5f) + abs(spriteB.rotationMatrix[3]) * (spriteB.scale[1] * .5f));

}

bool zcollide_checkCollision(Sprite& spriteA, Sprite& spriteB) {
    int usingAAABB = -1;
    int usingBAABB = -1;

    for (unsigned int i = 0; i < zcollide_AABB.size(); i++) {
        if (zcollide_AABB[i].ID == spriteA.ID) { usingAAABB = i; }
        else if (zcollide_AABB[i].ID == spriteB.ID) { usingBAABB = i; }
    }

    if (usingAAABB != -1 && usingBAABB != -1) { return (zcollide_AABB[usingAAABB].pos[0] <= zcollide_AABB[usingBAABB].pos[2] && zcollide_AABB[usingAAABB].pos[2] >= zcollide_AABB[usingBAABB].pos[0]) && (zcollide_AABB[usingAAABB].pos[1] <= zcollide_AABB[usingBAABB].pos[3] && zcollide_AABB[usingAAABB].pos[3] >= zcollide_AABB[usingBAABB].pos[1]); }
    else if (usingAAABB != -1 && usingBAABB == -1) {
        ZCOLLIDE_AABBS b;
        b.ID = spriteB.ID;
        const Vertex* verticesB = getVertices(spriteB.model);

        for (unsigned int i = 0; i < getVerticySize(spriteB.model); i++) {
            const float pxB = verticesB[i].position[0] * spriteB.scale[0];
            const float pyB = verticesB[i].position[1] * spriteB.scale[1];
            float transformedB[2] = { spriteB.rotationMatrix[0] * pxB + spriteB.rotationMatrix[1] * pyB, spriteB.rotationMatrix[2] * pxB + spriteB.rotationMatrix[3] * pyB};
            transformedB[0] += spriteB.position[0];
            transformedB[1] += spriteB.position[1];
            if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
            if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
            if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
            if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];
        }

        zcollide_AABB.emplace_back(b);
        return (zcollide_AABB[usingAAABB].pos[0] <= b.pos[2] && zcollide_AABB[usingAAABB].pos[2] >= b.pos[0]) && (zcollide_AABB[usingAAABB].pos[1] <= b.pos[3] && zcollide_AABB[usingAAABB].pos[3] >= b.pos[1]);
    }
    else if (usingAAABB == -1 && usingBAABB != -1) {
        ZCOLLIDE_AABBS a;
        a.ID = spriteA.ID;
        const Vertex* verticesA = getVertices(spriteA.model);

        for (unsigned int i = 0; i < getVerticySize(spriteA.model); i++) {
            const float pxA = verticesA[i].position[0] * spriteA.scale[0];
            const float pyA = verticesA[i].position[1] * spriteA.scale[1];
            float transformedA[2] = { spriteA.rotationMatrix[0] * pxA + spriteA.rotationMatrix[1] * pyA, spriteA.rotationMatrix[2] * pxA + spriteA.rotationMatrix[3] * pyA};
            transformedA[0] += spriteA.position[0];
            transformedA[1] += spriteA.position[1];
            if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
            if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
            if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
            if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];
        }

        zcollide_AABB.emplace_back(a);
        return (a.pos[0] <= zcollide_AABB[usingBAABB].pos[2] && a.pos[2] >= zcollide_AABB[usingBAABB].pos[0]) && (a.pos[1] <= zcollide_AABB[usingBAABB].pos[3] && a.pos[3] >= zcollide_AABB[usingBAABB].pos[1]);
    }
    else {
        spriteA.setRotationMatrix();
        spriteB.setRotationMatrix();

        ZCOLLIDE_AABBS a;
        a.ID = spriteA.ID;
        ZCOLLIDE_AABBS b;
        b.ID = spriteB.ID;

        const Vertex* verticesA = getVertices(spriteA.model);
        const Vertex* verticesB = getVertices(spriteB.model);
        const unsigned int verticesAsize = getVerticySize(spriteA.model);
        const unsigned int verticesBsize = getVerticySize(spriteB.model);

        if (verticesAsize > verticesBsize) {
            for (unsigned int i = 0; i < verticesAsize; i++) {
                const float pxA = verticesA[i].position[0] * spriteA.scale[0];
                const float pyA = verticesA[i].position[1] * spriteA.scale[1];
                float transformedA[2] = { spriteA.rotationMatrix[0] * pxA + spriteA.rotationMatrix[1] * pyA, spriteA.rotationMatrix[2] * pxA + spriteA.rotationMatrix[3] * pyA};
                transformedA[0] += spriteA.position[0];
                transformedA[1] += spriteA.position[1];
                if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];

                if (i < verticesBsize) {
                    const float pxB = verticesB[i].position[0] * spriteB.scale[0];
                    const float pyB = verticesB[i].position[1] * spriteB.scale[1];
                    float transformedB[2] = { spriteB.rotationMatrix[0] * pxB + spriteB.rotationMatrix[1] * pyB, spriteB.rotationMatrix[2] * pxB + spriteB.rotationMatrix[3] * pyB};
                    transformedB[0] += spriteB.position[0];
                    transformedB[1] += spriteB.position[1];
                    if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                    if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                    if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                    if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];
                }
            }
        }
        else if (verticesAsize < verticesBsize) {
            for (unsigned int i = 0; i < verticesBsize; i++) {
                const float pxB = verticesB[i].position[0] * spriteB.scale[0];
                const float pyB = verticesB[i].position[1] * spriteB.scale[1];
                float transformedB[2] = { spriteB.rotationMatrix[0] * pxB + spriteB.rotationMatrix[1] * pyB, spriteB.rotationMatrix[2] * pxB + spriteB.rotationMatrix[3] * pyB };
                transformedB[0] += spriteB.position[0];
                transformedB[1] += spriteB.position[1];
                if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];

                if (i < verticesAsize) {
                    const float pxA = verticesA[i].position[0] * spriteA.scale[0];
                    const float pyA = verticesA[i].position[1] * spriteA.scale[1];
                    float transformedA[2] = { spriteA.rotationMatrix[0] * pxA + spriteA.rotationMatrix[1] * pyA, spriteA.rotationMatrix[2] * pxA + spriteA.rotationMatrix[3] * pyA};
                    transformedA[0] += spriteA.position[0];
                    transformedA[1] += spriteA.position[1];
                    if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                    if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                    if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                    if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];
                }
            }
        }
        else {
            for (unsigned int i = 0; i < verticesAsize; i++) {
                const float pxA = verticesA[i].position[0] * spriteA.scale[0];
                const float pyA = verticesA[i].position[1] * spriteA.scale[1];
                float transformedA[2] = { spriteA.rotationMatrix[0] * pxA + spriteA.rotationMatrix[1] * pyA, spriteA.rotationMatrix[2] * pxA + spriteA.rotationMatrix[3] * pyA};
                transformedA[0] += spriteA.position[0];
                transformedA[1] += spriteA.position[1];
                if (transformedA[0] < a.pos[0]) a.pos[0] = transformedA[0];
                if (transformedA[1] < a.pos[1]) a.pos[1] = transformedA[1];
                if (transformedA[0] > a.pos[2]) a.pos[2] = transformedA[0];
                if (transformedA[1] > a.pos[3]) a.pos[3] = transformedA[1];

                const float pxB = verticesB[i].position[0] * spriteB.scale[0];
                const float pyB = verticesB[i].position[1] * spriteB.scale[1];
                float transformedB[2] = { spriteB.rotationMatrix[0] * pxB + spriteB.rotationMatrix[1] * pyB, spriteB.rotationMatrix[2] * pxB + spriteB.rotationMatrix[3] * pyB};
                transformedB[0] += spriteB.position[0];
                transformedB[1] += spriteB.position[1];
                if (transformedB[0] < b.pos[0]) b.pos[0] = transformedB[0];
                if (transformedB[1] < b.pos[1]) b.pos[1] = transformedB[1];
                if (transformedB[0] > b.pos[2]) b.pos[2] = transformedB[0];
                if (transformedB[1] > b.pos[3]) b.pos[3] = transformedB[1];
            }
        }
        zcollide_AABB.emplace_back(a);
        zcollide_AABB.emplace_back(b);
        return (a.pos[0] <= b.pos[2] && a.pos[2] >= b.pos[0]) && (a.pos[1] <= b.pos[3] && a.pos[3] >= b.pos[1]);
    }
}

inline void zcollide_clearAABB() { zcollide_AABB.clear(); }

#undef ZCOLLIDE_IMPLEMENTATION
#endif // ZCOLLIDE_IMPLEMENTATION
