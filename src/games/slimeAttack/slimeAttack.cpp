#include "slimeAttack.hpp"
#include "slimeAttackEnemies.hpp"
#include "../../deps/ZDev/math.hpp"
#include "../../deps/ZDev/collision.hpp"

#include <glm/glm.hpp>

SlimeAttack::SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Push& push) : keyboard(keyboard), audio(audio), pipeline(pipeline), push(push) {
    slimeattack::slimeAttackEnemyInit();
}

glm::vec2 speed = glm::vec2(0.f);
bool touchingGround = false;

void SlimeAttack::tick() {

    slimeattack::slimeAttackSimulateEnemies();
    if (slimeattack::slimeAttackIsTouchingEnemies()) {
        // Damage player
    }

    if(touchingGround) { 
        if (keyboard.keyPressed(GLFW_KEY_W)) { 
            speed.y = -.00002f;
            touchingGround = false; 
        }
    }

    if (keyboard.keyHit(GLFW_KEY_Q)) { slimeattack::slimeAttackSpawnEnemy(SLIMEATTACK_ENEMY_TYPE_SLIME, pipeline); }
    if(keyboard.keyPressed(GLFW_KEY_A)) { speed.x -= 5.f * deltaTime; }
    if(keyboard.keyPressed(GLFW_KEY_D)) { speed.x += 5.f * deltaTime; }

    speed.x *= .1f;
    speed.y += .0001f * deltaTime;
    sprites[0].position += speed;

    if (checkSquareCollision(spriteCPU[0], sprites[0], spriteCPU[1], sprites[1])) { 
        touchingGround = true;
        sprites[0].position.y -= speed.y;
        speed.y = 0;
    }
    else { touchingGround = false; }
    
    //Reset stuff for next frame
    keyboard.resetKeys();
}