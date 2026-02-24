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
        loadFont("assets/fonts/Bullpen3D.ttf", 32.f);
        for (unsigned char i = 0; i < 10; i++) { createSprite(squareModel, 1, -.7f, -.2f, .1f, .1f, 0.f); }

        bird = new Sprite();
        initSprite(bird);
        sprites[0] = bird;

        bird->position[0] = -.7f;
        bird->position[1] = -.2f;
        bird->setTexture(std::make_unique<Texture>("flappyBird.png"));
        sprites[1]->setTexture(std::make_unique<Texture>("pipe.png"));
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
                    sprites[sprites.size() - 1]->setTexture(createText(0, "ZDEV", 32));
                    camera.zoom[0] -= .007f;
                    camera.zoom[1] -= .007f;
                }

                if (bird->position[1] > 1.f || sprites[0]->position[1] < -1.f) {
                    flappyBirdDead = true;
                    playSound(&audio, "assets/sounds/hit.mp3");
                    sprites[sprites.size() - 1]->setTexture(createText(0, "something", 32));
                }
            }
            else if (bird->position[1] > 2.5f) {
                bird->textureIndex = 0;
                bird->position[1] = -.25f;
                bird->rotation = 0.f;
                flappyBirdSpeedY = 0.f;
                flappyBirdDead = false;
            }

            for (unsigned int i = 1; i < sprites.size() / 2; i++) {
                unsigned int index = ((i - 1) * 2) + 1;
                sprites[index]->position[0] -= .5f * deltaTime;
                if (sprites[index]->position[0] < -1.5f) {
                    sprites[index]->position[0] = 1.5f;
                    sprites[index]->position[1] = Random(4, 14) / 10.f;
                }
                sprites[index + 1]->position[0] = sprites[index]->position[0];
                sprites[index + 1]->position[1] = sprites[index]->position[1] - 2.f;

                 if (flappyBirdStarted && !flappyBirdDead && (zcollide_checkSquareCollision(sprites[index], bird) || zcollide_checkSquareCollision(sprites[index + 1], bird))) {
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
