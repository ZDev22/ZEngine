#include "slimeAttack.hpp"
#include "../../deps/ZDev/math.hpp"
#include "../../deps/ZDev/collision.hpp"

#include <glm/glm.hpp>

SlimeAttack::SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Push& push) : keyboard(keyboard), audio(audio), pipeline(pipeline), push(push) {}

glm::vec2 slimeAttackSpeed = glm::vec2(0.f);
bool slimeAttackTouchingGround = false;
bool slimeAttackIsDead = false;
float slimeAttackTimer = 0.f;
float slimeAttackAttackTimer = 0.f;

void SlimeAttack::tick() {

    //slimeAttackEnemies.spawnNewWave(pipeline);
    slimeAttackEnemies.simulateEnemies();

    if (slimeAttackIsDead) {
        slimeAttackTimer += deltaTime;
        if (slimeAttackTimer >= 1.f) {
            slimeAttackIsDead = false;
            slimeAttackTimer = 0.f;
            slimeAttackSpeed = glm::vec2(0.f);
            sprites[0].position = glm::vec2(0.f, -.5f);
        }
    }
    else {

        if (keyboard.keyPressed(GLFW_KEY_SPACE)) { slimeAttackAttackTimer += deltaTime; }
        else { slimeAttackAttackTimer = 0.f; }

        if (slimeAttackAttackTimer > 0.f && slimeAttackAttackTimer < .5f) { slimeAttackEnemies.damageEnemies(); }
        else if (slimeAttackEnemies.isTouchingEnemies()) {
            slimeAttackIsDead = true;
            slimeAttackSpeed = glm::vec2(0.f, .5f);
            sprites[0].position = glm::vec2(1.5f);
            slimeAttackSpeed = glm::vec2(0.f);
        }

        if(slimeAttackTouchingGround) {
            if (keyboard.keyPressed(GLFW_KEY_W)) {
                slimeAttackSpeed.y = -4.f;
                slimeAttackTouchingGround = false; 
            }
        }
    
        if (keyboard.keyHit(GLFW_KEY_Q)) { slimeAttackEnemies.spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline); }
        if(keyboard.keyPressed(GLFW_KEY_A)) { slimeAttackSpeed.x = -1.f; }
        else if(keyboard.keyPressed(GLFW_KEY_D)) { slimeAttackSpeed.x = 1.f; }
        else { slimeAttackSpeed.x = 0.f; }

        slimeAttackSpeed.y += 15.f * deltaTime;
        sprites[0].position += slimeAttackSpeed * glm::vec2(deltaTime);
    
        if (checkSquareCollision(spriteCPU[0], sprites[0], spriteCPU[1], sprites[1])) {
            slimeAttackTouchingGround = true;
            sprites[0].position.y -= slimeAttackSpeed.y * deltaTime;
            slimeAttackSpeed.y = 0;
        }
        else { slimeAttackTouchingGround = false; }
    }
    
    //Reset stuff for next frame
    keyboard.resetKeys();
}