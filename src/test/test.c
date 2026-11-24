#include "test.h"
#include "../zutils.h"

void initTest(void) {
    createPNGTexture("assets/img/flappyBird.png", 0);
    for (unsigned int i = 0; i < ZENGINE_MAX_SPRITES; i++) {
        createSprite(0.f, 0.f, .5f, .5f, 0.f, 0);
    }
}

void tickTest(void) {
    for (unsigned int i = 0; i < ZENGINE_MAX_SPRITES; i++) {
        sprites[i].position[0] = (((float)(rand() % 2000) / 1000.f) - 1.f) * camera.aspect;
        sprites[i].position[1] = ((float)(rand() % 2000) / 1000.f) - 1.f;
        sprites[i].rotation = (float)(rand() % 360000) / 1000.f;
    }
}

void deinitTest(void) {}
