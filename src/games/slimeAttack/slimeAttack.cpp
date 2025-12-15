#include "slimeAttack.hpp"
#include "deps/ZDev/math.hpp"
#include "deps/ZDev/collision.hpp"

SlimeAttack::SlimeAttack(Keyboard& keyboard, Pipeline& pipeline, Collision& collision) : keyboard(keyboard), pipeline(pipeline), collision(collision), slimeAttackEnemies(*this, collision, pipeline) {}

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
        if (keyboard.keyPressed(RGFW_space)) { slimeAttackAttackTimer += deltaTime; }
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
            if (keyboard.keyPressed(RGFW_w)) {
                slimeAttackSpeed[1] = -4.f;
                slimeAttackTouchingGround = false;
            }
        }

        if (keyboard.keyHit(RGFW_q)) { slimeAttackEnemies.spawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME); }
        if(keyboard.keyPressed(RGFW_a)) { slimeAttackSpeed[0] = -1.f; }
        else if(keyboard.keyPressed(RGFW_d)) { slimeAttackSpeed[0] = 1.f; }
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