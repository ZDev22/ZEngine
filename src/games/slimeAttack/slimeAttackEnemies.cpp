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
    float x = Random() ? -.9f : .9f;

    switch(type) {

    case SLIMEATTACK_ENEMY_TYPE_SLIME:

        pipeline.createSprite(pipeline.getSquareModel(), type, x, -.5f, .15f, .15f, 0.f, 1.f, 1.f, 1.f, 1.f);

        Enemy slime;
        slime.health = 2;
        slime.cooldown = 1.f;
        slime.spawnCooldown = Random(0.f, 3.f);
        slime.speed[0] = 0.f;
        slime.speed[1] = 0.f;
        slime.skip = false;

        enemies.push_back(slime);
        break;

    case SLIMEATTACK_ENEMY_TYPE_BAT:

        pipeline.createSprite(pipeline.getSquareModel(), type, x, -.5f, .15f, .15f, 0.f, 1.f, 1.f, 1.f, 1.f);
        
        Enemy bat;
        bat.health = 3;
        bat.cooldown = 1.f;
        bat.spawnCooldown = Random(0.f, 3.f);
        bat.speed[0] = 0.f;
        bat.speed[1] = 0.f;
        bat.skip = false;

        enemies.push_back(bat);
        break;

    case SLIMEATTACK_ENEMY_TYPE_OGRE:

        pipeline.createSprite(pipeline.getSquareModel(), type, x, -.5f, .15f, .15f, 0.f, 1.f, 1.f, 1.f, 1.f);

        Enemy ogre;
        ogre.health = 6;
        ogre.cooldown = 1.f;
        ogre.spawnCooldown = Random(0.f, 3.f);
        ogre.speed[0] = 0.f;
        ogre.speed[1] = 0.f;
        ogre.skip = false;

        enemies.push_back(ogre);
        break;
    
    case SLIMEATTACK_ENEMY_TYPE_BOSS:

        pipeline.createSprite(pipeline.getSquareModel(), type, 0.f, 1.5f, 1.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f);

        Enemy boss;
        boss.health = 25;
        boss.cooldown = .5f;
        boss.spawnCooldown = Random(0.f, 3.f);
        boss.speed[0] = 0.f;
        boss.speed[1] = .1f;
        boss.skip = false;

        enemies.push_back(boss);
        break;
    }
}

void SlimeAttackEnemies::simulateEnemies() {
    for (unsigned int i = 0; i < sprites.size(); i++) {
        if (!enemies[i].skip) {

            switch(sprites[i].textureIndex) {

            case SLIMEATTACK_ENEMY_TYPE_SLIME:
                enemies[i].cooldown += deltaTime;
                if (enemies[i].cooldown > 2.5f) {
                    enemies[i].speed[0] = sprites[0].position[0] >= sprites[i].position[0] ? .2f : -.2f;
                    enemies[i].speed[1] = -10.f;
                    enemies[i].cooldown = 0.f;
                }
                enemies[i].speed[1] += 1.6f * deltaTime;
                sprites[i].position[0] += enemies[i].speed[0] * deltaTime;
                sprites[i].position[1] += enemies[i].speed[1] * deltaTime;

                if (collision.checkSquareCollision(spriteCPU[1], sprites[1], spriteCPU[i], sprites[i])) { 
                    sprites[i].position[0] -= enemies[i].speed[0] * deltaTime;
                    sprites[i].position[1] -= enemies[i].speed[1] * deltaTime;
                    enemies[i].speed[0] = 0.f; 
                    enemies[i].speed[1] = 0.f;
                }
                break;

            case SLIMEATTACK_ENEMY_TYPE_BAT:
                enemies[i].cooldown += deltaTime;
                if (enemies[i].cooldown > 2.f) {
                    enemies[i].speed[0] = sprites[0].position[0] >= sprites[i].position[0] ? .5f : -.5f;
                    enemies[i].speed[1] = 1.5f;
                    enemies[i].speed[1] -=  1.f * deltaTime;
                    if (absolute(sprites[i].position[0]) >= .9f) { enemies[i].speed[0] = 0.f; }
                }
                break;

            case SLIMEATTACK_ENEMY_TYPE_OGRE:
                enemies[i].cooldown += deltaTime;
                if (enemies[i].cooldown > 2.5f) { if (absolute(sprites[0].position[0] - sprites[i].position[0]) <= .2f) { slimeAttack.knockback(sprites[i].position[0]); }}
                enemies[i].speed[0] += (sprites[0].position[0] >= sprites[i].position[0] ? -.1f : .1f) * deltaTime;
                break;

            case SLIMEATTACK_ENEMY_TYPE_BOSS:
                break;
            }

            if (enemies[i].speed[1] <= -1.5f) { enemies[i].speed[1] = -1.5f; }
        }
    }
}

bool SlimeAttackEnemies::isTouchingEnemies() {
    for (unsigned int i = 0; i < sprites.size(); i++) {
        if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME) {
            if (collision.checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) { return true; }
        }
    }
    return false;
}

void SlimeAttackEnemies::damageEnemies() {
    for (unsigned int i = 0; i < sprites.size(); i++) {
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