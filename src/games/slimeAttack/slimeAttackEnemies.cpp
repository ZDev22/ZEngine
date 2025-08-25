#include "slimeAttackEnemies.hpp"

#include "../../deps/ZDev/collision.hpp"

namespace slimeattack {

    std::vector<SlimeAttackEnemyStruct> slimeAttackEnemyVector;

    void slimeAttackEnemyInit() {
        while (slimeAttackEnemyVector.size() < sprites.size()) {
            SlimeAttackEnemyStruct enemy;
            slimeAttackEnemyVector.push_back(enemy);
        }
    }
    
    void slimeAttackSpawnEnemy(const int type, Pipeline& pipeline) {
        switch(type) {

        case SLIMEATTACK_ENEMY_TYPE_SLIME:

            pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));

            SlimeAttackEnemyStruct slime;
            slime.health = 2;
            slime.coinDrop = 1;
            slime.defence = 0;

            slimeAttackEnemyVector.push_back(slime);
            break;

        case SLIMEATTACK_ENEMY_TYPE_BAT:

            pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
            
            SlimeAttackEnemyStruct bat;
            slime.health = 3;
            slime.coinDrop = 4;
            slime.defence = 0;

            slimeAttackEnemyVector.push_back(bat);
            break;

        case SLIMEATTACK_ENEMY_TYPE_OGRE:

            pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
            
            SlimeAttackEnemyStruct ogre;
            slime.health = 6;
            slime.coinDrop = 8;
            slime.defence = 1;

            slimeAttackEnemyVector.push_back(ogre);
            break;
        }
    }
    
    void slimeAttackSimulateEnemies() {
        for (size_t i = 0; i < sprites.size(); i++) {
            switch(sprites[i].textureIndex) {

            case SLIMEATTACK_ENEMY_TYPE_SLIME:
                break;
            case SLIMEATTACK_ENEMY_TYPE_BAT:
                break;
            case SLIMEATTACK_ENEMY_TYPE_OGRE:
                break;
            }
        }
    }
    
    bool slimeAttackIsTouchingEnemies() {
        for (size_t i = 0; i < sprites.size(); i++) {
            if (sprites[i].textureIndex >= SLIMEATTACK_ENEMY_TYPE_SLIME) {
                if (checkSquareCollision(spriteCPU[i], sprites[i], spriteCPU[0], sprites[0])) { return true; }
            }
        }
        return false;
    }
}