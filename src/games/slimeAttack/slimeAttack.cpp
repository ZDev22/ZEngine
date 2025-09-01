#include "slimeAttack.hpp"
#include "slimeAttackEnemies.hpp"
#include "../../deps/ZDev/math.hpp"
#include "../../deps/ZDev/collision.hpp"

#include <glm/glm.hpp>

SlimeAttack::SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Push& push) : keyboard(keyboard), audio(audio), pipeline(pipeline), push(push) {
    slimeattack::slimeAttackEnemyInit();
}

glm::vec2 slimeAttackSpeed = glm::vec2(0.f);
bool slimeAttackTouchingGround = false;
bool slimeAttackIsDead = false;
float slimeAttacktimer = 0.f;

void SlimeAttack::tick() {

    slimeattack::slimeAttackSimulateEnemies();

    if (slimeAttackIsDead) {
        slimeAttackSpeed.y += .00015f * deltaTime;
        sprites[0].position += slimeAttackSpeed;

        slimeAttacktimer += deltaTime;

        if (slimeAttacktimer >= 1.5f) {
            slimeAttackIsDead = false;
            slimeAttacktimer = 0.f;
            slimeAttackSpeed = glm::vec2(0.f);
            sprites[0].position = glm::vec2(0.f, -.5f);
        }
    }
    else {
        if (slimeattack::slimeAttackIsTouchingEnemies()) { 
            slimeAttackIsDead = true;
            slimeAttackSpeed = glm::vec2(0.f, .5f);
        }

        if(slimeAttackTouchingGround) { 
            if (keyboard.keyPressed(GLFW_KEY_W)) { 
                slimeAttackSpeed.y = -.00002f;
                slimeAttackTouchingGround = false; 
            }
        }
    
        if (keyboard.keyHit(GLFW_KEY_Q)) { slimeattack::slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline); }
        if(keyboard.keyPressed(GLFW_KEY_A)) { slimeAttackSpeed.x -= 5.f * deltaTime; }
        if(keyboard.keyPressed(GLFW_KEY_D)) { slimeAttackSpeed.x += 5.f * deltaTime; }
    
        slimeAttackSpeed.x *= .1f;
        slimeAttackSpeed.y += .0001f * deltaTime;
        sprites[0].position += slimeAttackSpeed;
    
        if (checkSquareCollision(spriteCPU[0], sprites[0], spriteCPU[1], sprites[1])) { 
            slimeAttackTouchingGround = true;
            sprites[0].position.y -= slimeAttackSpeed.y;
            slimeAttackSpeed.y = 0;
        }
        else { slimeAttackTouchingGround = false; }
    }
    
    //Reset stuff for next frame
    keyboard.resetKeys();
}