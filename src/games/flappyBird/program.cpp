#define AUDIOPLAYER_IMPLEMENTATION
#include "../functions/audio.hpp"

#include "program.hpp"
#include "../functions/math.hpp"
#include "../functions/collision.hpp"

#include <glm/glm.hpp>

Program::Program(Keyboard& keyboard, Pipeline& pipeline, Push& push) : keyboard(keyboard), pipeline(pipeline), push(push) {}

float speedY = 0.f;
bool dead = false;
bool started = false;
int score = 0;

void Program::tick() {
    if (started) {
        speedY += 3.8f * deltaTime;
        sprites[0].position.y += speedY * deltaTime;
        sprites[0].rotation -= 90.f * deltaTime;
        if (!dead) {
            if (keyboard.keyHit(GLFW_KEY_SPACE)) {
                speedY = -1.3f;
                sprites[0].rotation = 60.f;
                sprites[0].textureIndex = 0;

                audio.play("chirp.mp3");
            }

            if (sprites[0].position.y > 1.f || sprites[0].position.y < -1.f) {
                dead = true;
                audio.play("hit.mp3");
            }
        }
        else if (sprites[0].position.y > 2.5f) {
            sprites[0].textureIndex = 0;
            sprites[0].position.y = -.25f;
            sprites[0].rotation = 0.f;
            speedY = 0.f;
            dead = false;
        }
    }
    else { if (keyboard.keyPressed(GLFW_KEY_SPACE)) { started = true; }}

    for (size_t i = 1; i < (sprites.size()) / 2; i++) {
            size_t index = ((i - 1) * 2) + 1;
            sprites[index].position.x -= .5f * deltaTime;
            if (sprites[index].position.x < -1.5f) {
                sprites[index].position.x = 1.5f;
                sprites[index].position.y = randomFloat(.4f, 1.4f);
            }
            sprites[index + 1].position.x = sprites[index].position.x;
            sprites[index + 1].position.y = sprites[index].position.y - 2.f;

            if (started && !dead && (checkSquareCollision(spriteCPU[index], sprites[index], spriteCPU[0], sprites[0]) || checkSquareCollision(spriteCPU[index + 1], sprites[index + 1], spriteCPU[0], sprites[0]))) {
                dead = true;
                speedY = -1.5f;
                sprites[0].rotation = 60.f;
                sprites[0].textureIndex = 1;

                audio.play("hit.mp3");
            }
        }

    //Reset stuff for next frame
    keyboard.resetKeys();
}