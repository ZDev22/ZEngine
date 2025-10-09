#include "astroids.hpp"

Astroids::Astroids(Keyboard& keyboard, ma_engine& audio, Pipeline& pipeline, Collision& collision, Push& vertex) : keyboard(keyboard), audio(audio), pipeline(pipeline), collision(collision), vertex(vertex) { astroid = std::make_unique<Astroid>(pipeline, collision); }

float speed[2];

void Astroids::tick() {
    if (keyboard.keyPressed(GLFW_KEY_W)) { speed[1] -= .02f * deltaTime; }
    else if (keyboard.keyPressed(GLFW_KEY_S)) { speed[1] += .02f * deltaTime; }
    if (keyboard.keyPressed(GLFW_KEY_A)) { speed[0] -= .02f * deltaTime; }
    else if (keyboard.keyPressed(GLFW_KEY_D)) { speed[0] += .02f * deltaTime; }
    speed[0] *= .95f;
    speed[1] *= .95f;
    sprites[0].position[0] += speed[0];
    sprites[0].position[1] += speed[1];

    astroid->tick();
    if (astroid->checkCollision()) {
        for (unsigned int i = 1; i < sprites.size();) {
            sprites.erase(sprites.begin() + i);
            spriteCPU.erase(spriteCPU.begin() + i);
        }
    }
}