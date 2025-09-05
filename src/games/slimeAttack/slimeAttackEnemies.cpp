#include "slimeAttackEnemies.hpp"
#include "slimeAttack.hpp"

#include "../../deps/ZDev/collision.hpp"

SlimeAttackEnemies::SlimeAttackEnemies(SlimeAttack& slimeAttack, Pipeline& pipeline) : pipeline(pipeline) {
    while (enemies.size() < sprites.size()) {
        Enemy enemy;
        enemy.skip = true;
        enemies.push_back(enemy);
    }
}

void SlimeAttackEnemies::spawnNewWave() {
    if (aliveEnemies == 0) { continue; }
    switch (wave) {
    case 1:
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        break;
    case 2:
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        break;
    case 3:
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        break;
    case 4:
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        break;
    case 5:
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_OGRE);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BAT);
        spawnEnemy(SLIMEATTACK_ENEMY_TYPE_BOSS);
        break;
    }
}

void SlimeAttackEnemies::spawnEnemy(const int type) {
    aliveEnemies++;
    float x = randomBool() ? -.9f : .9f;

    switch(type) {

    case SLIMEATTACK_ENEMY_TYPE_SLIME:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(x, -.5f), 0.f, glm::vec4(1.f));

        Enemy slime;
        slime.health = 2;
        slime.cooldown = 1.f;
        slime.spawnCooldown = randomFloat(0.f, 3.f);
        slime.speed = glm::vec2(0.f);
        slime.skip = false;

        enemies.push_back(slime);
        break;

    case SLIMEATTACK_ENEMY_TYPE_BAT:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(x, -.5f), 0.f, glm::vec4(1.f));
        
        Enemy bat;
        bat.health = 3;
        bat.cooldown = 1.f;
        bat.spawnCooldown = randomFloat(0.f, 3.f);
        bat.speed = glm::vec2(0.f);
        bat.skip = false;

        enemies.push_back(bat);
        break;

    case SLIMEATTACK_ENEMY_TYPE_OGRE:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(x, -.5f), 0.f, glm::vec4(1.f));

        Enemy ogre;
        ogre.health = 6;
        ogre.cooldown = 1.f;
        ogre.spawnCooldown = randomFloat(0.f, 3.f);
        ogre.speed = glm::vec2(0.f);
        ogre.skip = false;

        enemies.push_back(ogre);
        break;
    
    case SLIMEATTACK_ENEMY_TYPE_BOSS:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(0, -1.5f), 0.f, glm::vec4(1.f));

        Enemy boss;
        boss.health = 25;
        boss.cooldown = .5f;
        boss.spawnCooldown = randomFloat(0.f, 3.f);
        boss.speed = glm::vec2(0.f, .1f);
        boss.skip = false;

        enemies.push_back(boss);
        break;
    }
}

void SlimeAttackEnemies::simulateEnemies() {
    for (size_t i = 0; i < sprites.size(); i++) {
        if (!enemies[i].skip) {
            enemies[i].cooldown += deltaTime;

            switch(sprites[i].textureIndex) {

            case SLIMEATTACK_ENEMY_TYPE_SLIME:
                if (enemies[i].cooldown > 2.5f) {
                    enemies[i].speed = glm::vec2(sprites[0].position.x >= sprites[i].position.x ? .2f : -.2f, -10.f);
                    enemies[i].cooldown = 0.f;
                }
                enemies[i].speed.y += 1.6f * deltaTime;
                sprites[i].position += enemies[i].speed * glm::vec2(deltaTime);

                if (enemies[i].speed.y <= -1.5f) { enemies[i].speed.y = -1.5f; }

                if (checkSquareCollision(spriteCPU[1], sprites[1], spriteCPU[i], sprites[i])) { 
                    sprites[i].position -= enemies[i].speed * glm::vec2(deltaTime);
                    enemies[i].speed = glm::vec2(0.f); 
                }
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
            case SLIMEATTACK_ENEMY_TYPE_BOSS:
                break;
            }
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
                aliveEnemies--;
            }
        }
    }
}