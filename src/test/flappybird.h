/* this is a single-header example of how to use ZEngine. It is not compiled into the executable by default.
 * This example includes textures, text and audio.
 */

#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "../zutils.h"
#include "../zaudio.h"
#include "../zcollide.h"

#include <stdbool.h>

float flappyBirdSpeedY = 0.f;
bool flappyBirdDead = 0;
bool flappyBirdStarted = 0;
int flappyBirdScore = 0;

Sprite* bird;
Sprite* pipes[100];
unsigned char pipeSize = 0;

void createPipe(unsigned char index) {
    pipes[pipeSize] = createSprite(0.f, 0.f, .4f, .1f, 0.f, index);
    pipeSize++;
}

void initGame() {
    ZAudioInit();

    srand(100);
    loadFont("assets/fonts/Minecraft.png", 7, 0);

    createPNGTexture("assets/img/flappyBird.png", 0);
    createPNGTexture("assets/img/pipe.png", 1);
    createText("ZDev", 2, 0);

    bird = createSprite(-.7f, 0.f, .1f, .1f, 0.f, 0);
    for (unsigned char i = 0; i < 10; i++) { createPipe(1); }
}

void tickGame() {
    ZAudioTick();
    if (flappyBirdStarted) {
        flappyBirdSpeedY += 3.8f * deltaTime;
        bird->position[1] += flappyBirdSpeedY * deltaTime;
        bird->rotation -= 90.f * deltaTime;
        if (!flappyBirdDead) {
            if (RGFW_isKeyPressed(RGFW_keySpace)) {
                flappyBirdSpeedY = -1.3f;
                bird->rotation = 60.f;
                bird->textureIndex = 0;

                createPipe(2);
                ZAudioPlay("assets/sounds/bird.mp3");
            }

            if (bird->position[1] > 1.f || bird->position[1] < -1.f) {
                flappyBirdDead = 1;
                createText("aa", 3, 0);
                ZAudioPlay("assets/sounds/death.mp3");
            }
        }
        else if (bird->position[1] > 2.5f) {
            bird->textureIndex = 0;
            bird->position[1] = -.25f;
            bird->rotation = 0.f;
            flappyBirdSpeedY = 0.f;
            flappyBirdDead = 0;
        }

        for (unsigned int i = 0; i < pipeSize; i++) {
            pipes[i]->position[0] -= .5f * deltaTime;
            if (pipes[i]->position[0] < -camera.aspect) {
                pipes[i]->position[0] = camera.aspect;
                pipes[i]->position[1] = ((float)(rand() % 2000) / 1000.f) - 1.f;
            }

            if (flappyBirdStarted && !flappyBirdDead && (zcollide_squareCollision(pipes[i]->position[0], pipes[i]->position[1], pipes[i]->scale[0], pipes[i]->scale[1], bird->position[0], bird->position[1], bird->scale[0], bird->scale[1]))) {
                flappyBirdDead = 1;
                flappyBirdSpeedY = -1.5f;
                bird->rotation = 60.f;
                bird->textureIndex = 1;
                ZAudioPlay("assets/sounds/death.mp3");
            }
        }
    }
    else {
        if (RGFW_isKeyPressed(RGFW_keySpace)) {
            flappyBirdStarted = 1;
            flappyBirdSpeedY = -1.3f;
            bird->rotation = 60.f;
            ZAudioPlay("assets/sounds/bird.mp3");
        }
    }
}

void deinitGame(void) {
    ZAudioDeinit();
}

#endif // FLAPPYBIRD_H
