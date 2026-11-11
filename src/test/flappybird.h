/* this is a single-header example of how to use ZEngine. It is not compiled into the executable by default.
 * This example includes textures, text and audio.
 */

#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "../deps/miniaudio.h"

#include "../zengine.h"
#include "../zcollide.h"
#include "../ztext.h"

#include <stdbool.h>

float flappyBirdSpeedY = 0.f;
bool flappyBirdDead = 0;
bool flappyBirdStarted = 0;
int flappyBirdScore = 0;

Sprite* bird;
Sprite* pipes[100];
unsigned char pipeSize = 0;

ma_engine audio;

void createPipe(unsigned char index) {
    pipes[pipeSize] = createSprite(0.f, 0.f, .1f, .1f, 0.f, index);
    pipeSize++;
}

void initGame() {
    srand(100);
    ma_engine_init(NULL, &audio);
    loadFont("assets/fonts/Bullpen3D.ttf", 0);

    createTexture("assets/img/flappyBird.png", 0);
    createTexture("assets/img/pipe.png", 1);
    createText("ZDev", 150, 64, 64, 0, 2);

    bird = createSprite(-.7f, 0.f, .1f, .1f, 0.f, 0);
    for (unsigned char i = 0; i < 10; i++) { createPipe(1); }
}

void tickGame() {
    if (flappyBirdStarted) {
        flappyBirdSpeedY += 3.8f * deltaTime;
        bird->position[1] += flappyBirdSpeedY * deltaTime;
        rotateSprite(bird, bird->rotation - (90.f * deltaTime));
        if (!flappyBirdDead) {
            if (RGFW_isKeyPressed(RGFW_keySpace)) {
                flappyBirdSpeedY = -1.3f;
                rotateSprite(bird, 60.f);
                bird->textureIndex = 0;

                ma_engine_play_sound(&audio, "assets/sounds/chirp.mp3", NULL);
                createPipe(2);
            }

            if (bird->position[1] > 1.f || bird->position[1] < -1.f) {
                flappyBirdDead = 1;
                ma_engine_play_sound(&audio, "assets/sounds/hit.mp3", NULL);
                createText("Something", 128, 64, 64, 0, 3);
            }
        }
        else if (bird->position[1] > 2.5f) {
            bird->textureIndex = 0;
            bird->position[1] = -.25f;
            rotateSprite(bird, 0.f);
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
                rotateSprite(bird, 60.f);
                bird->textureIndex = 1;

                ma_engine_play_sound(&audio, "assets/sounds/hit.mp3", NULL);
            }
        }
    }
    else {
        if (RGFW_isKeyPressed(RGFW_keySpace)) {
            flappyBirdStarted = 1;
            flappyBirdSpeedY = -1.3f;
            rotateSprite(bird, 60.f);
        }
    }
}


void deinitGame(void) {
    ma_engine_uninit(&audio);
}

#endif // FLAPPYBIRD_H

