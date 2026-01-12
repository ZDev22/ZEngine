#pragma once

#include "zwindow.hpp"
#include "collision.hpp"
#include "zengine.hpp"

#include "slimeAttackEnemies.hpp"

struct SlimeAttack {
public:
    SlimeAttack(ZWindow& zwindow, Collision& collision);

    void tick();
    void knockback(float origin);
private:
    SlimeAttackEnemies slimeAttackEnemies;
    Collision& collision;
    ZWindow& zwindow;
};