/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v3.4.11

zcollide.h is a lightweight cross-platform sigle-header c library for checking if 2D objects are colliding!

#define ZCOLLIDE_IMPLEMENTATION - define in one c ONLY
#define ZCOLLIDE_ZENGINE - gives extra functions that rely on zengine, put here to allow use of this file in other engines

HOW TO USE:
zcollide_squareCollision(sprites[0], sprites[1]); - check the collision between two square squares
*/

#ifndef ZCOLLIDE_H
#define ZCOLLIDE_H

#ifdef __cplusplus
    extern "C" {
#else
    #include <stdbool.h>
#endif

#ifdef ZCOLLIDE_ZENGINE
    #include "zengine.h"

    bool zcollide_squareTransformCollision(Sprite* spriteA, Sprite* spriteB);
#endif

bool zcollide_squareCollision(float positionAx, float positionAy, float scaleAx, float scaleAy, float positionBx, float positionBy, float scaleBx, float scaleBy);
bool zcollide_circleCollision(float circleAx, float circleAy, float radiusA, float circleBx, float circleBy, float radiusB);
bool zcollide_pointCircleCollision(float pointx, float pointy, float circlex, float circley, float radius);

#ifdef ZCOLLIDE_IMPLEMENTATION

#include <math.h>

#ifdef ZCOLLIDE_ZENGINE
    bool zcollide_squareTransformCollision(Sprite* spriteA, Sprite* spriteB) {
        setRotationMatrix(spriteA);
        setRotationMatrix(spriteB);
        return fabs(spriteA->position[0] - spriteB->position[0]) <= (fabs(spriteA->rotationMatrix[0]) * (spriteA->scale[0] * .5f) + fabs(spriteA->rotationMatrix[1]) * (spriteA->scale[1] * .5f) + fabs(spriteB->rotationMatrix[0]) * (spriteB->scale[0] * .5f) + fabs(spriteB->rotationMatrix[1]) * (spriteB->scale[1] * .5f)) && fabs(spriteA->position[1] - spriteB->position[1]) <= (fabs(spriteA->rotationMatrix[2]) * (spriteA->scale[0] * 0.5f) + fabs(spriteA->rotationMatrix[3]) * (spriteA->scale[1] * .5f) + fabs(spriteB->rotationMatrix[2]) * (spriteB->scale[0] * .5f) + fabs(spriteB->rotationMatrix[3]) * (spriteB->scale[1] * .5f));
    }
#endif

bool zcollide_squareCollision(float positionAx, float positionAy, float scaleAx, float scaleAy, float positionBx, float positionBy, float scaleBx, float scaleBy) {
    return (fabs(positionAx - positionBx) <= (scaleAx * .5f) + (scaleBx * .5f)) && (fabs(positionAy - positionBy) <= (scaleAy * .5f) + (scaleBy * .5f));
}

/* thanks to MidnightHammer for this collision function! */
bool zcollide_circleCollision(float circleAx, float circleAy, float radiusA, float circleBx, float circleBy, float radiusB) {
    return sqrt((circleAx - circleBx) * (circleAx - circleBx) + (circleAy - circleBy) * (circleAy - circleBy)) < radiusA + radiusB;
}

bool zcollide_pointCircleCollision(float pointx, float pointy, float circlex, float circley, float radius) {
    if (fabs(circlex - pointx) > radius) { return 0; }
    if (fabs(circley - pointy) < radius) { return 1; }
    return 0;
}

#endif // ZCOLLIDE_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif

#endif // ZCOLLIDE_H
