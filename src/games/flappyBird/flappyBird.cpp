#include "flappyBird.hpp"

#include "zcollide.hpp"
#include "zmath.hpp"

FlappyBird::FlappyBird(ZWindow& zwindow, ma_engine& audio, Push& vertex) : zwindow(zwindow), audio(audio), vertex(vertex) {}

float flappyBirdSpeedY = 0.f;
bool flappyBirdDead = false;
bool flappyBirdStarted = false;
int flappyBirdScore = 0;

void FlappyBird::tick() {
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
                //sprites[sprites.size() - 1].setText("ZDEV", 0, 32.f);
                vertex.cameraZoom[0] -= .025f;
                vertex.cameraZoom[1] -= .025f;
            }

            if (sprites[0].position[1] > 1.f || sprites[0].position[1] < -1.f) {
                flappyBirdDead = true;
                playSound(&audio, "assets/sounds/hit.mp3");
                sprites[sprites.size() - 1].setText("Something", 0, 32.f);
            }
        }
        else if (sprites[0].position[1] > 2.5f) {
            sprites[0].textureIndex = 0;
            sprites[0].position[1] = -.25f;
            sprites[0].rotation = 0.f;
            flappyBirdSpeedY = 0.f;
            flappyBirdDead = false;
        }
    }
    else { if (zwindow.keyPressed(RGFW_space)) { flappyBirdStarted = true; }}

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