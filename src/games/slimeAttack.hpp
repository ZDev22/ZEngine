#pragma once

#include "zwindow.hpp"
#include "zengine.hpp"
#include "zmath.hpp"
#include "zcollide.hpp"

#include <vector>

float slimeAttackSpeed[2] = {0.f};
bool slimeAttackTouchingGround = false;
bool slimeAttackIsDead = false;
float slimeAttackTimer = 0.f;
float slimeAttackAttackTimer = 0.f;
float slimeAttackHitTimer = 0.f;
int health = 3;

#define SLIMEATTACK_ENEMY_TYPE_DEATH 3
#define SLIMEATTACK_ENEMY_TYPE_SLIME 4
#define SLIMEATTACK_ENEMY_TYPE_BAT 5
#define SLIMEATTACK_ENEMY_TYPE_OGRE 6
#define SLIMEATTACK_ENEMY_TYPE_BOSS 7

void knockback(float origin) {
    if (slimeAttackHitTimer > 1.f) {
        slimeAttackSpeed[0] = origin >= sprites[0].position[0] ? -.5f : .5f;
        slimeAttackSpeed[1] = 1.f;
        slimeAttackHitTimer = 0.f;
        health--;
    }
}

struct Game;
struct SlimeAttackEnemies {
public:
    SlimeAttackEnemies(Game& game) : game(game) {
        while (enemies.size() < sprites.size()) {
            Enemy enemy;
            enemy.skip = true;
            enemies.push_back(enemy);
        }
        spawnNewWave();
    }

    void spawnEnemy(const int type) {
        aliveEnemies++;
        float x = Random() ? -.9f : .9f;

        switch(type) {

        case SLIMEATTACK_ENEMY_TYPE_SLIME:

            createSprite(squareModel, type, x, -.5f, .15f, .15f, 0.f);

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

            createSprite(squareModel, type, x, -.5f, .15f, .15f, 0.f);

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

            createSprite(squareModel, type, x, -.5f, .15f, .15f, 0.f);

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

            createSprite(squareModel, type, 0.f, 1.5f, 1.f, 1.f, 0.f);

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

    void spawnNewWave() {
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

    void simulateEnemies() {
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

                    if (zcollide_checkSquareCollision(sprites[1], sprites[i])) {
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
                        if (abs(sprites[i].position[0]) >= .9f) { enemies[i].speed[0] = 0.f; }
                    }
                    break;

                case SLIMEATTACK_ENEMY_TYPE_OGRE:
                    enemies[i].cooldown += deltaTime;
                    if (enemies[i].cooldown > 2.5f) { if (abs(sprites[0].position[0] - sprites[i].position[0]) <= .2f) { knockback(sprites[i].position[0]); }}
                    enemies[i].speed[0] += (sprites[0].position[0] >= sprites[i].position[0] ? -.1f : .1f) * deltaTime;
                    break;

                case SLIMEATTACK_ENEMY_TYPE_BOSS:
                    break;
                }

                if (enemies[i].speed[1] <= -1.5f) { enemies[i].speed[1] = -1.5f; }
            }
        }
    }

    bool isTouchingEnemies() {
        for (unsigned int i = 0; i < sprites.size(); i++) {
            if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME) {
                if (zcollide_checkSquareCollision(sprites[i], sprites[0])) { return true; }
            }
        }
        return false;
    }

    void damageEnemies() {
        for (unsigned int i = 0; i < sprites.size(); i++) {
            if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME && zcollide_checkSquareCollision(sprites[i], sprites[0])) {
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
private:
    Game& game;

    struct Enemy {
        int health;
        float cooldown;
        float spawnCooldown;
        float speed[2];
        bool skip;
    };
    std::vector<Enemy> enemies = {};

    int wave = 1;
    int aliveEnemies = 0;
};

struct Game {
public:
    Game(ZWindow& zwindow, ma_engine& audio, Camera& camera)  : zwindow(zwindow), audio(audio), camera(camera), slimeAttackEnemies(*this) {
    sprites[0].scale[0] = .15f;
    sprites[0].scale[1] = .15f;
    createSprite(squareModel, 1, 0.f, .7f, 2.f, .15f, 0.f);
}

    void tick() {
        slimeAttackEnemies.simulateEnemies();

        if (slimeAttackIsDead) {
            slimeAttackTimer += deltaTime;
            if (slimeAttackTimer >= 1.f) {
                slimeAttackIsDead = false;
                slimeAttackTimer = 0.f;
                slimeAttackSpeed[0] = 0.f;
                slimeAttackSpeed[1] = 0.f;
                sprites[0].position[0] = 0.f;
                sprites[0].position[1] = -.5f;
            }
        }
        else {
            slimeAttackHitTimer += deltaTime;
            if (zwindow.keyPressed(RGFW_space)) { slimeAttackAttackTimer += deltaTime; }
            else { slimeAttackAttackTimer = 0.f; }
            if (slimeAttackAttackTimer > 0.f && slimeAttackAttackTimer < .5f) { slimeAttackEnemies.damageEnemies(); }
            else if (slimeAttackEnemies.isTouchingEnemies()) { health--; }
            if (health <= 0) {
                slimeAttackIsDead = true;
                slimeAttackSpeed[0] = 0.f;
                slimeAttackSpeed[1] = .5f;
                sprites[0].position[0] = 1.5f;
                sprites[0].position[1] = 1.5f;
                slimeAttackSpeed[0] = 0.f;
                slimeAttackSpeed[1] = 0.f;
            }

            if(slimeAttackTouchingGround) {
                if (zwindow.keyPressed(RGFW_w)) {
                    slimeAttackSpeed[1] = -4.f;
                    slimeAttackTouchingGround = false;
                }
            }

            if (zwindow.keyHit(RGFW_q)) { slimeAttackEnemies.spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME); }
            if(zwindow.keyPressed(RGFW_a)) { slimeAttackSpeed[0] = -1.f; }
            else if(zwindow.keyPressed(RGFW_d)) { slimeAttackSpeed[0] = 1.f; }
            else { slimeAttackSpeed[0] = 0.f; }

            slimeAttackSpeed[1] += 15.f * deltaTime;
            sprites[0].position[0] += slimeAttackSpeed[0] * deltaTime;
            sprites[0].position[1] += slimeAttackSpeed[1] * deltaTime;

            if (zcollide_checkSquareCollision(sprites[0], sprites[1])) {
                slimeAttackTouchingGround = true;
                sprites[0].position[1] -= slimeAttackSpeed[1] * deltaTime;
                slimeAttackSpeed[1] = 0;
            }
            else { slimeAttackTouchingGround = false; }
        }
    }
private:
    ZWindow& zwindow;
    ma_engine& audio;
    Camera& camera;
    SlimeAttackEnemies slimeAttackEnemies;
};
