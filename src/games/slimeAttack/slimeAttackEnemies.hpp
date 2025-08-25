#ifndef SLIMEATTACKENEMIES_HPP
#define SLIMEATTACKENEMIES_HPP

#include "slimeAttack.hpp"

#define SLIMEATTACK_ENEMY_TYPE_SLIME 3
#define SLIMEATTACK_ENEMY_TYPE_BAT 4
#define SLIMEATTACK_ENEMY_TYPE_OGRE 5

namespace slimeattack { // Namespace and obscure names to not interfere with other projects
    struct SlimeAttackEnemyStruct { 
        uint16_t health;
        uint16_t coinDrop;
        uint16_t defence;
    };
    
    void slimeAttackEnemyInit();
    void slimeAttackSpawnEnemy(const int type, Pipeline& pipeline);
    void slimeAttackSimulateEnemies();
    bool slimeAttackIsTouchingEnemies();
}

#endif