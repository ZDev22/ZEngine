#ifndef SLIMEATTACKENEMIES_HPP
#define SLIMEATTACKENEMIES_HPP

#include "slimeAttack.hpp"

#define SLIMEATTACK_ENEMY_TYPE_DEATH 3
#define SLIMEATTACK_ENEMY_TYPE_SLIME 4
#define SLIMEATTACK_ENEMY_TYPE_BAT 5
#define SLIMEATTACK_ENEMY_TYPE_OGRE 6

namespace slimeattack { // Namespace and obscure names to not interfere with other projects
    struct SlimeAttackEnemyStruct { 
        uint16_t health;
        uint16_t coinDrop;
        uint16_t defence;
        float cooldown;
        glm::vec2 speed;
        bool skip;
    };
    
    void slimeAttackEnemyInit();
    void slimeAttackSpawnNewWave(Pipeline& pipeline);
    void slimeAttackSpawnEnemy(const int type, Pipeline& pipeline);
    void slimeAttackSimulateEnemies();
    bool slimeAttackIsTouchingEnemies();
    void slimeAttackDamageEnemies();
}

#endif