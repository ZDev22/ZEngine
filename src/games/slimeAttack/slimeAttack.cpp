#include "slimeAttack.hpp"
#include "slimeAttackEnemies.hpp"
#include "../../deps/ZDev/math.hpp"
#include "../../deps/ZDev/collision.hpp"

#include <glm/glm.hpp>

slimeAttackEnemies.SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Push& push) : keyboard(keyboard), audio(audio), pipeline(pipeline), push(push) {
    slimeAttackEnemies = std::make_unique<SlimeAttackEnemies>(*this);
}

glm::vec2 slimeAttackSpeed = glm::vec2(0.f);
bool slimeAttackTouchingGround = false;
bool slimeAttackIsDead = false;
float slimeAttackTimer = 0.f;
float slimeAttackAttackTimer = 0.f;
float slimeAttackHitTimer = 0.f;
int health = 3;

void slimeAttackEnemies.tick() {

    slimeAttackEnemies.slimeAttackSpawnNewWave(pipeline);
    slimeAttackEnemies.slimeAttackSimulateEnemies();

    if (slimeAttackIsDead) {
        slimeAttackSpeed.y += .00015f * deltaTime;
        sprites[0].position += slimeAttackSpeed;

        slimeAttackTimer += deltaTime;

        if (slimeAttackTimer >= 1.5f) {
            slimeAttackIsDead = false;
            slimeAttackTimer = 0.f;
            slimeAttackSpeed = glm::vec2(0.f);
            sprites[0].position = glm::vec2(0.f, -.5f);
        }
    }
    else {

        if (keyboard.keyPressed(GLFW_KEY_SPACE)) { slimeAttackAttackTimer += deltaTime; }
        else { slimeAttackAttackTimer = 0.f; }

        if (slimeAttackAttackTimer > 0.f && slimeAttackAttackTimer < .5f) { slimeAttackEnemies.slimeAttackDamageEnemies(); }
        else if (slimeAttackHitTimer > 1.f && slimeAttackEnemies.slimeAttackIsTouchingEnemies()) {
            health--;
            slimeAttackHitTimer = 0.f
        }

        if (health <= 0) {
            slimeAttackIsDead = true;
            slimeAttackSpeed = glm::vec2(0.f, .5f);
        }

        if(slimeAttackTouchingGround) {
            if (keyboard.keyPressed(GLFW_KEY_W)) {
                slimeAttackSpeed.y = -.00002f;
                slimeAttackTouchingGround = false; 
            }
        }
    
        if (keyboard.keyHit(GLFW_KEY_Q)) { slimeAttackEnemies.slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline); }
        if(keyboard.keyPressed(GLFW_KEY_A)) { slimeAttackSpeed.x -= 5.f * deltaTime; }
        if(keyboard.keyPressed(GLFW_KEY_D)) { slimeAttackSpeed.x += 5.f * deltaTime; }
    
        slimeAttackSpeed.x *= 1.f * deltaTime;
        slimeAttackSpeed.y += 1.5f * deltaTime;
        sprites[0].position += slimeAttackSpeed * glm::vec2(deltaTime);
    
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

void slimeAttackEnemies.knockBack(float origin) {
    if (slimeAttackHitTimer > 1.f) {
        if (origin >= sprites[0].position.x) { slimeAttackSpeed = glm::vec2(-.5f, 1.f); }
        else { slimeAttackSpeed = glm::vec2(.5f, 1.f); }
        health--;  
        slimeAttackHitTimer = 0.f;  
    }
}