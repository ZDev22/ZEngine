#include "../functions/audio.hpp"

#include "slimeAttack.hpp"
#include "../functions/math.hpp"
#include "../functions/collision.hpp"

#include <glm/glm.hpp>

SlimeAttack::SlimeAttack(Keyboard& keyboard, Pipeline& pipeline, Push& push) : keyboard(keyboard), pipeline(pipeline), push(push) {}

glm::vec2 speed = glm::vec2(0.f);

void SlimeAttack::tick() {
    //speed.y += .05f * deltaTime;

    if(keyboard.keyHit(GLFW_KEY_W)) { if (speed.y = 0) { speed.y = .3f; }}
    if(keyboard.keyPressed(GLFW_KEY_A)) { speed.x -= 5.f * deltaTime; }
    if(keyboard.keyPressed(GLFW_KEY_D)) { speed.x += 5.f * deltaTime; }
    speed.x *= .1f;

    sprites[0].position += speed;
    
    //Reset stuff for next frame
    keyboard.resetKeys();
}