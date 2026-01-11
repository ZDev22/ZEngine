#pragma once

#include "deps/ZDeps/keyboard.hpp"
#include "deps/ZDeps/collision.hpp"
#include "zengine.hpp"

#include "slimeAttackEnemies.hpp"

struct SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, Collision& collision);

    void tick();
    void knockback(float origin);
private:
    SlimeAttackEnemies slimeAttackEnemies;
    Collision& collision;
    Keyboard& keyboard;
};