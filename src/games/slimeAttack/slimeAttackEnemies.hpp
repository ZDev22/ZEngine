#pragma once

#include "slimeAttack.hpp"

#define SLIMEATTACK_ENEMY_TYPE_DEATH 3
#define SLIMEATTACK_ENEMY_TYPE_SLIME 4
#define SLIMEATTACK_ENEMY_TYPE_BAT 5
#define SLIMEATTACK_ENEMY_TYPE_OGRE 6

class SlimeAttackEnemies {
public:

    SlimeAttackEnemies(SlimeAttack& slimeAttack);
    void spawnNewWave(Pipeline& pipeline);
    void spawnEnemy(const int type, Pipeline& pipeline);
    void simulateEnemies();
    bool isTouchingEnemies();
    void damageEnemies();

private:

    struct Enemies {
        uint16_t health;
        uint16_t coinDrop;
        uint16_t defence;
        float cooldown;
        float spawnTimer;
        glm::vec2 speed;
        bool skip;
    };

    SlimeAttack& slimeAttack;

}