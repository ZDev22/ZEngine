#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "../deps/miniaudio.h"

#include "../zengine.h"
#include "../zcollide.h"
#include "../zmath.h"
#include "../ztext.h"

float flappyBirdSpeedY = 0.f;
_Bool flappyBirdDead = 0;
_Bool flappyBirdStarted = 0;
int flappyBirdScore = 0;

Sprite* bird;

void initGame() {
    sRandom();
    loadFont("assets/fonts/Bullpen3D.ttf");

    Texture texture;
    createTexture(&texture, "flappyBird.png", .5f);
    updateTexture(0, &texture);
    createTexture(&texture, "pipe.png", 1.f);
    updateTexture(1, &texture);

    createSprite(squareModel, 0, -.7f, 0.f, .1f, .1f, 0.f);
    bird = &sprites[spritesSize - 1];

    for (unsigned char i = 0; i < 10; i++) { createSprite(squareModel, 1, -.7f, -.3f, .1f, .1f, 0.f); }
}

void tickGame() {
    if (flappyBirdStarted) {
        flappyBirdSpeedY += 3.8f * deltaTime;
        bird->position[1] += flappyBirdSpeedY * deltaTime;
        bird->rotation -= 90.f * deltaTime;
        if (!flappyBirdDead) {
            if (RGFW_isKeyPressed(RGFW_keySpace)) {
                flappyBirdSpeedY = -1.3f;
                bird->rotation = 60.f;
                bird->textureIndex = 0;

                ma_engine_play_sound(&audio, "assets/sounds/chirp.mp3", NULL);
                createSprite(squareModel, 3, 0.f, 0.f, .5f, .2f, 0.f);
                updateTexture(3, createText("ZDev", 0, 128));
                camera.zoom[0] -= .007f;
                camera.zoom[1] -= .007f;
            }

            if (bird->position[1] > 1.f || bird->position[1] < -1.f) {
                flappyBirdDead = 1;
                ma_engine_play_sound(&audio, "assets/sounds/hit.mp3", NULL);
                updateTexture(3, createText("Something", 0, 128));
            }
        }
        else if (bird->position[1] > 2.5f) {
            bird->textureIndex = 0;
            bird->position[1] = -.25f;
            bird->rotation = 0.f;
            flappyBirdSpeedY = 0.f;
            flappyBirdDead = 0;
        }

        for (unsigned int i = 1; i < spritesSize / 2; i++) {
            unsigned int index = ((i - 1) * 2) + 1;
            sprites[index].position[0] -= .5f * deltaTime;
            if (sprites[index].position[0] < -1.5f) {
                sprites[index].position[0] = 1.5f;
                sprites[index].position[1] = randomNum(4, 14) / 10.f;
            }
            sprites[index + 1].position[0] = sprites[index].position[0];
            sprites[index + 1].position[1] = sprites[index].position[1] - 2.f;

            if (flappyBirdStarted && !flappyBirdDead && (zcollide_squareCollision(&sprites[index], bird) || zcollide_squareCollision(&sprites[index + 1], bird))) {
                flappyBirdDead = 1;
                flappyBirdSpeedY = -1.5f;
                bird->rotation = 60.f;
                bird->textureIndex = 1;

                ma_engine_play_sound(&audio, "assets/sounds/hit.mp3", NULL);
            }
        }
    }
    else {
        if (RGFW_isKeyPressed(RGFW_keySpace)) {
            flappyBirdStarted = 1;
            flappyBirdSpeedY = -1.3f;
            bird->rotation = 60.f;
        }
    }
}

#endif // FLAPPYBIRD_H

