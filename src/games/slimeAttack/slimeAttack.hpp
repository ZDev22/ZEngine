#pragma once

#include "../../deps/ZDev/keyboard.hpp"
#include "../../deps/ZDev/collision.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

#include "slimeAttackEnemies.hpp"

class AudioPlayer;
class SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, Pipeline& pipeline, Collision& collision);

    void tick();
    void knockback(float origin);
private:
    SlimeAttackEnemies slimeAttackEnemies;
    Collision& collision;
    Keyboard& keyboard;
    Pipeline& pipeline;
};