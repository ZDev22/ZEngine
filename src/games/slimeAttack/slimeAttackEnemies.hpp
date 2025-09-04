#pragma once

#include <vector>

#include <glm/glm.hpp>

#define SLIMEATTACK_ENEMY_TYPE_DEATH 3
#define SLIMEATTACK_ENEMY_TYPE_SLIME 4
#define SLIMEATTACK_ENEMY_TYPE_BAT 5
#define SLIMEATTACK_ENEMY_TYPE_OGRE 6

class Pipeline;
class SlimeAttack;
class SlimeAttackEnemies {
public:
    SlimeAttackEnemies(SlimeAttack& slimeAttack);

    void spawnNewWave(Pipeline& pipeline);
    void spawnEnemy(const int type, Pipeline& pipeline);
    void simulateEnemies();
    bool isTouchingEnemies();
    void damageEnemies();
private:
    struct Enemy { 
        int health;
        int coinDrop;
        int defence;
        float cooldown;
        glm::vec2 speed;
        bool skip;
    };

    std::vector<Enemy> enemies = {};
    int wave = 1;
};