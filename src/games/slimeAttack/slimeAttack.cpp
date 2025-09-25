#include "slimeAttack.hpp"
#include "../../deps/ZDev/math.hpp"
#include "../../deps/ZDev/collision.hpp"

SlimeAttack::SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Collision& collision, Push& push) : keyboard(keyboard), audio(audio), pipeline(pipeline), collision(collision), push(push), slimeAttackEnemies(*this, collision, pipeline) {}

float slimeAttackSpeed[2] = {0.f};
bool slimeAttackTouchingGround = false;
bool slimeAttackIsDead = false;
int health = 3;
float slimeAttackTimer = 0.f;
float slimeAttackAttackTimer = 0.f;
float slimeAttackHitTimer = 0.f;

void SlimeAttack::tick() {

    slimeAttackEnemies.simulateEnemies();

    if (slimeAttackIsDead) {
        slimeAttackTimer += deltaTime;
        if (slimeAttackTimer >= 1.f) {
            slimeAttackIsDead = false;
            slimeAttackTimer = 0.f;
            slimeAttackSpeed[0] = 0.f;
            slimeAttackSpeed[1] = 0.f;
            sprites[0].position[0] = 0.f;
            sprites[0].position[1] = -.5f;
        }
    }
    else {
        slimeAttackHitTimer += deltaTime;
        if (keyboard.keyPressed(GLFW_KEY_SPACE)) { slimeAttackAttackTimer += deltaTime; }
        else { slimeAttackAttackTimer = 0.f; }
        if (slimeAttackAttackTimer > 0.f && slimeAttackAttackTimer < .5f) { slimeAttackEnemies.damageEnemies(); }
        else if (slimeAttackEnemies.isTouchingEnemies()) { health--; }
        if (health <= 0) {
            slimeAttackIsDead = true;
            slimeAttackSpeed[0] = 0.f;
            slimeAttackSpeed[1] = .5f;
            sprites[0].position[0] = 1.5f;
            sprites[0].position[1] = 1.5f;
            slimeAttackSpeed[0] = 0.f;
            slimeAttackSpeed[1] = 0.f;
        }

        if(slimeAttackTouchingGround) {
            if (keyboard.keyPressed(GLFW_KEY_W)) {
                slimeAttackSpeed[1] = -4.f;
                slimeAttackTouchingGround = false; 
            }
        }
    
        if (keyboard.keyHit(GLFW_KEY_Q)) { slimeAttackEnemies.spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME); }
        if(keyboard.keyPressed(GLFW_KEY_A)) { slimeAttackSpeed[0] = -1.f; }
        else if(keyboard.keyPressed(GLFW_KEY_D)) { slimeAttackSpeed[0] = 1.f; }
        else { slimeAttackSpeed[0] = 0.f; }

        slimeAttackSpeed[1] += 15.f * deltaTime;
        sprites[0].position[0] += slimeAttackSpeed[0] * deltaTime;
        sprites[0].position[1] += slimeAttackSpeed[1] * deltaTime;
    
        if (collision.checkSquareCollision(spriteCPU[0], sprites[0], spriteCPU[1], sprites[1])) {
            slimeAttackTouchingGround = true;
            sprites[0].position[1] -= slimeAttackSpeed[1] * deltaTime;
            slimeAttackSpeed[1] = 0;
        }
        else { slimeAttackTouchingGround = false; }
    }
}

void SlimeAttack::knockback(float origin) {
    if (slimeAttackHitTimer > 1.f) {
        slimeAttackSpeed[0] = origin >= sprites[0].position[0] ? -.5f : .5f;
        slimeAttackSpeed[1] = 1.f;
        slimeAttackHitTimer = 0.f;  
        health--;
    }
}