#pragma once

#include <vector>
#include "deps/ZDeps/collision.hpp"
#include "deps/ZDeps/zmath.hpp"

#define SLIMEATTACK_ENEMY_TYPE_DEATH 3
#define SLIMEATTACK_ENEMY_TYPE_SLIME 4
#define SLIMEATTACK_ENEMY_TYPE_BAT 5
#define SLIMEATTACK_ENEMY_TYPE_OGRE 6
#define SLIMEATTACK_ENEMY_TYPE_BOSS 7

struct Pipeline;
struct SlimeAttack;
struct SlimeAttackEnemies {
public:
    SlimeAttackEnemies(SlimeAttack& slimeAttack, Collision& collision, Pipeline& pipeline);

    void spawnNewWave();
    void spawnEnemy(const int type);
    void simulateEnemies();
    bool isTouchingEnemies();
    void damageEnemies();
private:
    SlimeAttack& slimeAttack;
    Collision& collision;
    Pipeline& pipeline;

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