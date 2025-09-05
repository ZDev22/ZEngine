#pragma once

#include <vector>

#define SLIMEATTACK_ENEMY_TYPE_DEATH 3
#define SLIMEATTACK_ENEMY_TYPE_SLIME 4
#define SLIMEATTACK_ENEMY_TYPE_BAT 5
#define SLIMEATTACK_ENEMY_TYPE_OGRE 6
#define SLIMEATTACK_ENEMY_TYPE_BOSS 7

class Pipeline;
class SlimeAttack;
class SlimeAttackEnemies {
public:
    SlimeAttackEnemies(SlimeAttack& slimeAttack, Pipeline& pipeline);

    void spawnNewWave();
    void spawnEnemy(const int type);
    void simulateEnemies();
    bool isTouchingEnemies();
    void damageEnemies();
private:
    Pipeline& pipeline;

    struct Enemy { 
        int health;
        float cooldown;
        float spawnCooldown;
        glm::vec2 speed;
        bool skip;
    };
    std::vector<Enemy> enemies = {};

    int wave = 1;
    int aliveEnemies = 0;
};