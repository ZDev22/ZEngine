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

struct Game {
public:
    Game(ZWindow& zwindow, ma_engine& audio, Camera& camera)  : zwindow(zwindow), audio(audio), camera(camera) {
        loadFont("assets/fonts/Bullpen3D.ttf", 32.f);
        for (unsigned int i = 0; i < 10; i++) { createSprite(squareModel, 1, -.7f, -.2f, .1f, .1f, 0.f); }
   
        sprites[0].position[0] = -.7f;
        sprites[0].position[1] = -.2f;
        sprites[0].setTexture(std::make_unique<Texture>("flappyBird.png"));
        sprites[1].setTexture(std::make_unique<Texture>("pipe.png"));
    }

    void tick() {
        std::cout << zwindow.getMouseX() << " - " << zwindow.getMouseY();
        if (flappyBirdStarted) {
            flappyBirdSpeedY += 3.8f * deltaTime;
            sprites[0].position[1] += flappyBirdSpeedY * deltaTime;
            sprites[0].rotation -= 90.f * deltaTime;
            if (!flappyBirdDead) {
                if (zwindow.keyHit(RGFW_space)) {
                    flappyBirdSpeedY = -1.3f;
                    sprites[0].rotation = 60.f;
                    sprites[0].textureIndex = 0;

                    playSound(&audio, "assets/sounds/chirp.mp3");
                    createSprite(squareModel, 3, 0.f, 0.f, .5f, .2f, 0.f);
                    sprites[sprites.size() - 1].setTexture(createText(0, "ZDEV", 32));
                    camera.zoom[0] -= .025f;
                    camera.zoom[1] -= .025f;
                }

                if (sprites[0].position[1] > 1.f || sprites[0].position[1] < -1.f) {
                    flappyBirdDead = true;
                    playSound(&audio, "assets/sounds/hit.mp3");
                    sprites[sprites.size() - 1].setTexture(createText(0, "something", 32));
                }
            }
            else if (sprites[0].position[1] > 2.5f) {
                sprites[0].textureIndex = 0;
                sprites[0].position[1] = -.25f;
                sprites[0].rotation = 0.f;
                flappyBirdSpeedY = 0.f;
                flappyBirdDead = false;
            }

            for (unsigned int i = 1; i < sprites.size() / 2; i++) {
                unsigned int index = ((i - 1) * 2) + 1;
                sprites[index].position[0] -= .5f * deltaTime;
                if (sprites[index].position[0] < -1.5f) {
                    sprites[index].position[0] = 1.5f;
                    sprites[index].position[1] = Random(4, 14) / 10.f;
                }
                sprites[index + 1].position[0] = sprites[index].position[0];
                sprites[index + 1].position[1] = sprites[index].position[1] - 2.f;

                 if (flappyBirdStarted && !flappyBirdDead && (zcollide_checkSquareCollision(sprites[index], sprites[0]) || zcollide_checkSquareCollision(sprites[index + 1], sprites[0]))) {
                    flappyBirdDead = true;
                    flappyBirdSpeedY = -1.5f;
                    sprites[0].rotation = 60.f;
                    sprites[0].textureIndex = 1;

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
