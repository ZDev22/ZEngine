#include "test.h"
#include "../zengine.h"

void initTest(void) {
    createTexture("assets/img/flappyBird.png", 0);
    for (unsigned int i = 0; i < ZENGINE_MAX_SPRITES; i++) {
        createSprite(0.f, 0.f, .5f, .5f, 0.f, 0);
    }
}

void tickTest(void) {
    for (unsigned int i = 0; i < ZENGINE_MAX_SPRITES; i++) {
        sprites[i].position[0] = ((float)(rand() % 2000) / 1000.f) - 1.f;
        sprites[i].position[1] = ((float)(rand() % 2000) / 1000.f) - 1.f;
    }
}

void deinitTest(void) {}
