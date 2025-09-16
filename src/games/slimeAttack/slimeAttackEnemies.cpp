#include "slimeAttackEnemies.hpp"
#include "slimeAttack.hpp"

#include "../../deps/ZDev/collision.hpp"

SlimeAttackEnemies::SlimeAttackEnemies(SlimeAttack& slimeAttack, Collision& collision, Pipeline& pipeline) : slimeAttack(slimeAttack), collision(collision), pipeline(pipeline) {
    while (enemies.size() < sprites.size()) {
        Enemy enemy;
        enemy.skip = true;
        enemies.push_back(enemy);
    }
    spawnNewWave();
}

void SlimeAttackEnemies::spawnNewWave() {
    if (aliveEnemies != 0) { return; }
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
    wave++;
}

void SlimeAttackEnemies::spawnEnemy(const int type) {
    aliveEnemies++;
    float x = randomBool() ? -.9f : .9f;

    switch(type) {

    case SLIMEATTACK_ENEMY_TYPE_SLIME:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(x, -.5f), glm::vec2(.15f, .15f), 0.f, glm::vec4(1.f));

        Enemy slime;
        slime.health = 2;
        slime.cooldown = 1.f;
        slime.spawnCooldown = randomFloat(0.f, 3.f);
        slime.speed = glm::vec2(0.f);
        slime.skip = false;

        enemies.push_back(slime);
        break;

    case SLIMEATTACK_ENEMY_TYPE_BAT:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(x, -.5f), glm::vec2(.15f, .15f), 0.f, glm::vec4(1.f));
        
        Enemy bat;
        bat.health = 3;
        bat.cooldown = 1.f;
        bat.spawnCooldown = randomFloat(0.f, 3.f);
        bat.speed = glm::vec2(0.f);
        bat.skip = false;

        enemies.push_back(bat);
        break;

    case SLIMEATTACK_ENEMY_TYPE_OGRE:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(x, -.5f), glm::vec2(.15f, .15f), 0.f, glm::vec4(1.f));

        Enemy ogre;
        ogre.health = 6;
        ogre.cooldown = 1.f;
        ogre.spawnCooldown = randomFloat(0.f, 3.f);
        ogre.speed = glm::vec2(0.f);
        ogre.skip = false;

        enemies.push_back(ogre);
        break;
    
    case SLIMEATTACK_ENEMY_TYPE_BOSS:

        pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 1.5f), glm::vec2(1.f, 1.f), 0.f, glm::vec4(1.f));

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

            switch(sprites[i].textureIndex) {

            case SLIMEATTACK_ENEMY_TYPE_SLIME:
                enemies[i].cooldown += deltaTime;
                if (enemies[i].cooldown > 2.5f) {
                    enemies[i].speed = glm::vec2(sprites[0].position.x >= sprites[i].position.x ? .2f : -.2f, -10.f);
                    enemies[i].cooldown = 0.f;
                }
                enemies[i].speed.y += 1.6f * deltaTime;
                sprites[i].position += enemies[i].speed * glm::vec2(deltaTime);

                if (collision.checkSquareCollision(spriteCPU[1], sprites[1], spriteCPU[i], sprites[i])) { 
                    sprites[i].position -= enemies[i].speed * glm::vec2(deltaTime);
                    enemies[i].speed = glm::vec2(0.f); 
                }
                break;

            case SLIMEATTACK_ENEMY_TYPE_BAT:
                enemies[i].cooldown += deltaTime;
                if (enemies[i].cooldown > 2.f) {
                    enemies[i].speed = glm::vec2(sprites[0].position.x >= sprites[i].position.x ? .5f : -.5f, 1.5f);
                    enemies[i].speed.y -=  1.f * deltaTime;

                    if (absolute(sprites[i].position.x) >= .9f) { enemies[i].speed.x = 0.f; }
                }
                break;

            case SLIMEATTACK_ENEMY_TYPE_OGRE:
                enemies[i].cooldown += deltaTime;
                if (enemies[i].cooldown > 2.5f) { if (absolute(sprites[0].position.x - sprites[i].position.x) <= .2f) { slimeAttack.knockback(sprites[i].position.x); }}
                enemies[i].speed.x += (sprites[0].position.x >= sprites[i].position.x ? -.1f : .1f) * deltaTime;
                break;

            case SLIMEATTACK_ENEMY_TYPE_BOSS:
                break;
            }

            if (enemies[i].speed.y <= -1.5f) { enemies[i].speed.y = -1.5f; }
        }
    }
}

bool SlimeAttackEnemies::isTouchingEnemies() {
    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME) {
            if (collision.checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) { return true; }
        }
    }
    return false;
}

void SlimeAttackEnemies::damageEnemies() {
    for (size_t i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME && collision.checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) {
            enemies[i].health--;
            if (enemies[i].health <= 0) { 
                enemies[i].skip = true;
                sprites[i].textureIndex = SLIMEATTACK_ENEMY_TYPE_DEATH;
                spriteCPU[i].visible = false;
                aliveEnemies--;
                if (aliveEnemies == 0) { spawnNewWave(); }
            }
        }
    }
}