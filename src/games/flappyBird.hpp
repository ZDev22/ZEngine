#pragma once

#include "deps/miniaudio.h"

#include "zwindow.hpp"
#include "zengine.hpp"
#include "zcollide.hpp"
#include "zmath.hpp"
#include "ztext.hpp"

float flappyBirdSpeedY = 0.f;
bool flappyBirdDead = false;
bool flappyBirdStarted = false;
int flappyBirdScore = 0;

Sprite* bird;

struct Game {
public:
    Game(ZWindow& zwindow, ma_engine& audio, Camera& camera)  : zwindow(zwindow), audio(audio), camera(camera) {
        bird = createSprite();
        bird->position[0] = -.7f;
        bird->position[1] = -.2f;
        updateTexture(bird->textureIndex, std::make_unique<Texture>("flappyBird.png"));

        for (unsigned char i = 0; i < 10; i++) { createSprite(squareModel, 1, -.7f, -.2f, .1f, .1f, 0.f); }
        updateTexture(1, std::make_unique<Texture>("pipe.png"));
    }

    void tick() {
        if (flappyBirdStarted) {
            flappyBirdSpeedY += 3.8f * deltaTime;
            bird->position[1] += flappyBirdSpeedY * deltaTime;
            bird->rotation -= 90.f * deltaTime;
            if (!flappyBirdDead) {
                if (zwindow.keyHit(RGFW_space)) {
                    flappyBirdSpeedY = -1.3f;
                    bird->rotation = 60.f;
                    bird->textureIndex = 0;

                    playSound(&audio, "assets/sounds/chirp.mp3");
                    createSprite(squareModel, 3, 0.f, 0.f, .5f, .2f, 0.f);
                    updateTexture(3, createText("ZDev"));
                    camera.zoom[0] -= .007f;
                    camera.zoom[1] -= .007f;
                }

                if (bird->position[1] > 1.f || bird->position[1] < -1.f) {
                    flappyBirdDead = true;
                    playSound(&audio, "assets/sounds/hit.mp3");
                    updateTexture(3, createText("Something"));
                }
            }
            else if (bird->position[1] > 2.5f) {
                bird->textureIndex = 0;
                bird->position[1] = -.25f;
                bird->rotation = 0.f;
                flappyBirdSpeedY = 0.f;
                flappyBirdDead = false;
            }

            for (unsigned int i = 1; i < spritesSize / 2; i++) {
                unsigned int index = ((i - 1) * 2) + 1;
                sprites[index].position[0] -= .5f * deltaTime;
                if (sprites[index].position[0] < -1.5f) {
                    sprites[index].position[0] = 1.5f;
                    sprites[index].position[1] = Random(4, 14) / 10.f;
                }
                sprites[index + 1].position[0] = sprites[index].position[0];
                sprites[index + 1].position[1] = sprites[index].position[1] - 2.f;

                 if (flappyBirdStarted && !flappyBirdDead && (zcollide_checkSquareCollision(&sprites[index], bird) || zcollide_checkSquareCollision(&sprites[index + 1], bird))) {
                    flappyBirdDead = true;
                    flappyBirdSpeedY = -1.5f;
                    bird->rotation = 60.f;
                    bird->textureIndex = 1;

                    playSound(&audio, "assets/sounds/hit.mp3");
                }
            }
        }
        else { if (zwindow.keyPressed(RGFW_space)) { flappyBirdStarted = true; }}
    }
private:
    ZWindow& zwindow;
    ma_engine& audio;
    Camera& camera;
};
