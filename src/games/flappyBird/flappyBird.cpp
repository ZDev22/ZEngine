#include "flappyBird.hpp"
#include "../../deps/ZDev/math.hpp"

FlappyBird::FlappyBird(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Collision& collision, Push& push) : keyboard(keyboard), audio(audio), pipeline(pipeline), collision(collision), push(push) {}

float flappyBirdSpeedY = 0.f;
bool flappyBirdDead = false;
bool flappyBirdStarted = false;
int flappyBirdScore = 0;

void FlappyBird::tick() {
    if (flappyBirdStarted) {
        flappyBirdSpeedY += 3.8f * deltaTime;
        sprites[0].position.y += flappyBirdSpeedY * deltaTime;
        sprites[0].rotation -= 90.f * deltaTime;
        if (!flappyBirdDead) {
            if (keyboard.keyHit(GLFW_KEY_SPACE)) {
                flappyBirdSpeedY = -1.3f;
                sprites[0].rotation = 60.f;
                sprites[0].textureIndex = 0;

                audio.play("assets/sounds/chirp.mp3");
            }

            if (sprites[0].position.y > 1.f || sprites[0].position.y < -1.f) {
                flappyBirdDead = true;
                audio.play("assets/sounds/hit.mp3");
            }
        }
        else if (sprites[0].position.y > 2.5f) {
            sprites[0].textureIndex = 0;
            sprites[0].position.y = -.25f;
            sprites[0].rotation = 0.f;
            flappyBirdSpeedY = 0.f;
            flappyBirdDead = false;
        }
    }
    else { if (keyboard.keyPressed(GLFW_KEY_SPACE)) { flappyBirdStarted = true; }}

    for (size_t i = 1; i < (sprites.size()) / 2; i++) {
            size_t index = ((i - 1) * 2) + 1;
            sprites[index].position.x -= .5f * deltaTime;
            if (sprites[index].position.x < -1.5f) {
                sprites[index].position.x = 1.5f;
                sprites[index].position.y = randomFloat(.4f, 1.4f);
            }
            sprites[index + 1].position.x = sprites[index].position.x;
            sprites[index + 1].position.y = sprites[index].position.y - 2.f;

            if (flappyBirdStarted && !flappyBirdDead && (collision.checkSquareCollision(spriteCPU[index], sprites[index], spriteCPU[0], sprites[0]) || collision.checkSquareCollision(spriteCPU[index + 1], sprites[index + 1], spriteCPU[0], sprites[0]))) {
                flappyBirdDead = true;
                flappyBirdSpeedY = -1.5f;
                sprites[0].rotation = 60.f;
                sprites[0].textureIndex = 1;

                audio.play("assets/sounds/hit.mp3");
            }
        }

    //Reset stuff for next frame
    keyboard.resetKeys();
    collision.clearAABB();
}