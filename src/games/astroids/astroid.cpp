#include "astroid.hpp"

Astroid::Astroid(Pipeline& pipeline, Collision& collision) : pipeline(pipeline), collision(collision) {};

void Astroid::tick() {
    if (Random(1, 150) == 1) {
        pipeline.createSprite(pipeline.getSquareModel(), 0, 0.f, 0.f, .1f, .1f, 0.f, 1.f, 1.f, 1.f, 1.f);

        Stats stats;
        stats.ID = sprites.size();
        stats.slope[0] = -sprites[stats.ID].position[0] / 180;
        stats.slope[1] = -sprites[stats.ID].position[1] / 180;
        stats.speed = Random(.1f, .2f);
        stats.health = Random(1, 5);
        astroids.push_back(stats);
    }

    for (unsigned int i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex > 0) {
            sprites[i].position[0] += astroids[i].stats.slope[0];
            sprites[i].position[1] += astroids[i].stats.slope[1];
        }
    }
}

bool Astroid::checkCollision() {
    for (unsigned int i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex == 1) {
            if (collision.checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) { return true; }
        }
    }
    return false;
}