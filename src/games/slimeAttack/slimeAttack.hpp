#pragma once

#include "deps/ZDeps/keyboard.hpp"
#include "deps/ZDeps/collision.hpp"
#include "engine/pipeline.hpp"
#include "engine/global.hpp"

#include "slimeAttackEnemies.hpp"

#define FPS_CAP_SET
#define FPS_CAP 60.f
#define USE_MOUSE
#define USE_MULTITHREADING

struct SlimeAttack {
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