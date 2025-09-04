#include "slimeAttackEnemies.hpp"

#include "../../deps/ZDev/collision.hpp"

std::vector<Enemies> enemies = {};
uint8_t wave = 1;

SlimeAttackEnemies::SlimeAttackEnemies(SlimeAttack& slimeAttack) : slimeAttack(slimeAttack) {
    while (enemies.size() < sprites.size()) {
        Enemies enemy;
        enemy.skip = true;
        enemies.push_back(enemy);
    }
}

void SlimeAttackEnemies::spawnNewWave(Pipeline& pipeline) {
    int i = sprites.size();
    for (; i > 0; i--) { if (!sprites[i].visible) { break; }}

    if (i < sprites.size()) {
        while (true) {
            i = sprites.size();
            if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME) { 
                sprites.erase(i);
                spriteCPU.erase(i);
                enemies.erase(i);
            }
            else { break; }
        }

        switch (wave) {
        case 1:
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline);
            break;
        case 2:
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline);
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT, pipeline);
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE, pipeline);
            break;
        case 3:
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline);
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT, pipeline);
            slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE, pipeline);
            break;
        }
        wave++;
    }
}

void SlimeAttackEnemies::spawnEnemy(const int type, Pipeline& pipeline) {
    float x = randomBool() ? -.9f : .9f;
    switch(type) {

    case SLIMEATTACK_ENEMY_TYPE_SLIME:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(x, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
        sprites[sprites.size() - 1].visible = false;

        Enemies slime;
        slime.health = 2;
        slime.coinDrop = 1;
        slime.defence = 0;
        slime.cooldown = 0.f;
        slime.spawnTimer = randomFloat(0.f, 3.f);
        slime.speed = glm::vec2(0.f);
        slime.skip = false;

        enemies.push_back(slime);
        break;

    case SLIMEATTACK_ENEMY_TYPE_BAT:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(x, -.5f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
        sprites[sprites.size() - 1].visible = false;

        Enemies bat;
        bat.health = 3;
        bat.coinDrop = 4;
        bat.defence = 0;
        bat.cooldown = 0.f;
        bat.spawnTimer = randomFloat(0.f, 3.f);
        bat.speed = glm::vec2(0.f);
        bat.skip = false;

        enemies.push_back(bat);
        break;

    case SLIMEATTACK_ENEMY_TYPE_OGRE:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(x, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
        sprites[sprites.size() - 1].visible = false;

        Enemies ogre;
        ogre.health = 6;
        ogre.coinDrop = 8;
        ogre.defence = 1;
        ogre.cooldown = 0.f;
        ogre.spawnTimer = randomFloat(0.f, 3.f);
        ogre.speed = glm::vec2(0.f);
        ogre.skip = false;

        enemies.push_back(ogre);
        break;
    }
}

void SlimeAttackEnemies::simulateEnemies() {
    for (size_t i = 0; i < sprites.size(); i++) {
        if (!enemies[i].skip) {
            enemies[i].cooldown += 1.f * deltaTime;

            switch(sprites[i].textureIndex) {

            case SLIMEATTACK_ENEMY_TYPE_SLIME:
                if (enemies[i].cooldown > 2.5f) {
                    enemies[i].speed = glm::vec2(sprites[0].position.x >= sprites[i].position.x ? .2f : -.2f, -1.2f);
                    enemies[i].cooldown = 0.f;
                }
                enemies[i].speed.y += 1.5f * deltaTime;
                break;
            case SLIMEATTACK_ENEMY_TYPE_BAT:
                if (enemies[i].cooldown > 2.f) {
                    enemies[i].speed = glm::vec2(sprites[0].position.x >= sprites[i].position.x ? .5f : -.5f, 1.5f);
                    enemies[i].speed.y -=  1.f * deltaTime;

                    if (absoulteFloat(sprites[i].position.x) >= .9f) { enemies[i].speed.x = 0.f; }
                }
                break;
            case SLIMEATTACK_ENEMY_TYPE_OGRE:
                if (enemies[i].cooldown > 2.5f) { if (absoluteFloat(sprites[0].position - sprites[i].position) <= .2f) { slimeAttack.knockBack(sprites[i].position.x); }}
                enemies[i].speed.x += (sprites[0].position.x >= sprites[i].position.x ? -.1f : .1f) * deltaTime;
                break;
            }

            sprites[i].position += enemies[i].speed * glm::vec2(deltaTime);
            if (checkSquareCollision(spriteCPU[1], sprites[1], spriteCPU[i], sprites[i])) { enemies[i].speed = glm::vec2(0.f); }
        }
    }
}

bool SlimeAttackEnemies::isTouchingEnemies() {
    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME) {
            if (checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) { return true; }
        }
    }
    return false;
}

void SlimeAttackEnemies::damageEnemies() {
    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME && checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) {
            enemies[i].health--;
            if (enemies[i].health <= 0) { 
                enemies[i].skip = true;
                sprites[i].textureIndex = SLIMEATTACK_ENEMY_TYPE_DEATH;
                spriteCPU[i].visible = false;
            }
        }
    }
}