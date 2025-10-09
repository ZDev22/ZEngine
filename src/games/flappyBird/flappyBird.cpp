#include "flappyBird.hpp"

FlappyBird::FlappyBird(Keyboard& keyboard, ma_engine& audio, Pipeline& pipeline, Collision& collision, Push& vertex) : keyboard(keyboard), audio(audio), pipeline(pipeline), collision(collision), vertex(vertex) {}

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
            if (keyboard.keyHit(GLFW_KEY_SPACE)) {
                flappyBirdSpeedY = -1.3f;
                sprites[0].rotation = 60.f;
                sprites[0].textureIndex = 0;

                playSound(&audio, "assets/sounds/chirp.mp3");
                pipeline.createSprite(pipeline.getSquareModel(), 3, 0.f, 0.f, .5f, .2f, 0.f, 1.f, 1.f, 1.f, 1.f);
                sprites[sprites.size() - 1].setText("ZDEV", 0, 32.f, pipeline);
                vertex.cameraZoom[0] -= .025f;
                vertex.cameraZoom[1] -= .025f;
            }

            if (sprites[0].position[1] > 1.f || sprites[0].position[1] < -1.f) {
                flappyBirdDead = true;
                playSound(&audio, "assets/sounds/hit.mp3");
                sprites[sprites.size() - 1].setText("Something", 0, 32.f, pipeline);
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
    else { if (keyboard.keyPressed(GLFW_KEY_SPACE)) { flappyBirdStarted = true; }}

    for (unsigned int i = 1; i < sprites.size() / 2; i++) {
        unsigned int index = ((i - 1) * 2) + 1;
        sprites[index].position[0] -= .5f * deltaTime;
        if (sprites[index].position[0] < -1.5f) {
            sprites[index].position[0] = 1.5f;
            sprites[index].position[1] = Random(4, 14) / 10.f;
        }
        sprites[index + 1].position[0] = sprites[index].position[0];
        sprites[index + 1].position[1] = sprites[index].position[1] - 2.f;

        if (flappyBirdStarted && !flappyBirdDead && (collision.checkSquareCollision(spriteCPU[index], sprites[index], spriteCPU[0], sprites[0]) || collision.checkSquareCollision(spriteCPU[index + 1], sprites[index + 1], spriteCPU[0], sprites[0]))) {
            flappyBirdDead = true;
            flappyBirdSpeedY = -1.5f;
            sprites[0].rotation = 60.f;
            sprites[0].textureIndex = 1;

            playSound(&audio, "assets/sounds/hit.mp3");
        }
    }
}