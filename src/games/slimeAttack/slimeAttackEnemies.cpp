#include "slimeAttackEnemies.hpp"

#include "../../deps/ZDev/collision.hpp"

namespace slimeattack {

    std::vector<SlimeAttackEnemyStruct> slimeAttackEnemyVector;

    void slimeAttackEnemyInit() {
        while (slimeAttackEnemyVector.size() < sprites.size()) {
            SlimeAttackEnemyStruct enemy;
            enemy.skip = true;
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
            slime.cooldown = 0.f;
            slime.speed = glm::vec2(0.f);
            slime.skip = false;

            slimeAttackEnemyVector.push_back(slime);
            break;

        case SLIMEATTACK_ENEMY_TYPE_BAT:

            pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
            
            SlimeAttackEnemyStruct bat;
            bat.health = 3;
            bat.coinDrop = 4;
            bat.defence = 0;
            bat.cooldown = 0.f;
            slime.speed = glm::vec2(0.f);
            bat.skip = false;

            slimeAttackEnemyVector.push_back(bat);
            break;

        case SLIMEATTACK_ENEMY_TYPE_OGRE:

            pipeline.createSprite(pipeline.getSquareModel(), type, glm::vec2(0.f, 0.f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));
            
            SlimeAttackEnemyStruct ogre;
            ogre.health = 6;
            ogre.coinDrop = 8;
            ogre.defence = 1;
            ogre.cooldown = 0.f;
            slime.speed = glm::vec2(0.f);
            ogre.skip = false;

            slimeAttackEnemyVector.push_back(ogre);
            break;
        }
    }
    
    void slimeAttackSimulateEnemies() {
        for (size_t i = 0; i < sprites.size(); i++) {
            if (!slimeAttackEnemyVector[i].skip) {
                slimeAttackEnemyVector[i].cooldown += 1.f * deltaTime;

                switch(sprites[i].textureIndex) {

                case SLIMEATTACK_ENEMY_TYPE_SLIME:
                    if (slimeAttackEnemyVector[i].cooldown > 2.5f) {
                        slimeAttackEnemyVector[i].speed += glm::vec2(.3f, -.5f);
                        slimeAttackEnemyVector[i].cooldown = 0.f;
                    }
                    slimeAttackEnemyVector[i].speed.y += .3f * deltaTime;
                    slimeAttackEnemyVector[i].speed.x *= .8f * deltaTime;

                    if (checkSquareCollision(spriteCPU[1], sprites[1], spriteCPU[i], sprites[i])) { slimeAttackEnemyVector[i].speed = glm::vec2(0.f); }
                    sprites[i].position += slimeAttackEnemyVector[i].speed * glm::vec2(deltaTime); 
                    break;
                case SLIMEATTACK_ENEMY_TYPE_BAT:
                    break;
                case SLIMEATTACK_ENEMY_TYPE_OGRE:
                    break;
                }
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