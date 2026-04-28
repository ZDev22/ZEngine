/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

v3.2.10

zcollide.h is a lightweight cross-platform sigle-header c library for checking if 2D objects are colliding!

#define ZCOLLIDE_IMPLEMENTATION - define in one c ONLY

HOW TO USE:
zcollide_squareCollision(sprites[0], sprites[1]); - check the collision between two square squares
*/

#ifndef ZCOLLIDE_H
#define ZCOLLIDE_H

#include "zengine.h"

_Bool zcollide_squareCollision(Sprite* spriteA, Sprite* spriteB);
_Bool zcollide_circleCollision(Sprite* spriteA, float radiusA, Sprite* spriteB, float radiusB);

#ifdef ZCOLLIDE_IMPLEMENTATION

#include <math.h>

_Bool zcollide_squareCollision(Sprite* spriteA, Sprite* spriteB) {
    setRotationMatrix(spriteA);
    setRotationMatrix(spriteB);
    return fabs(spriteA->position[0] - spriteB->position[0]) <= (fabs(spriteA->rotationMatrix[0]) * (spriteA->scale[0] * .5f) + fabs(spriteA->rotationMatrix[1]) * (spriteA->scale[1] * .5f) + fabs(spriteB->rotationMatrix[0]) * (spriteB->scale[0] * .5f) + fabs(spriteB->rotationMatrix[1]) * (spriteB->scale[1] * .5f)) && fabs(spriteA->position[1] - spriteB->position[1]) <= (fabs(spriteA->rotationMatrix[2]) * (spriteA->scale[0] * 0.5f) + fabs(spriteA->rotationMatrix[3]) * (spriteA->scale[1] * .5f) + fabs(spriteB->rotationMatrix[2]) * (spriteB->scale[0] * .5f) + fabs(spriteB->rotationMatrix[3]) * (spriteB->scale[1] * .5f));
}

/* thanks to MidnightHammer for these collision functions! */
_Bool zcollide_circleCollision(Sprite* spriteA, float radiusA, Sprite* spriteB, float radiusB) {
    float dx = spriteA->position[0] - spriteB->position[0];
    float dy = spriteA->position[1] - spriteB->position[1];

    int radiusSum = (radiusA + radiusB);

    return (dx * dx) + (dy * dy) <= (radiusSum * radiusSum);
}

#endif // ZCOLLIDE_IMPLEMENTATION
#endif // ZCOLLIDE_H

