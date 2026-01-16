#pragma once

#include "zwindow.hpp"
#include "zengine.hpp"

#include "slimeAttackEnemies.hpp"

struct SlimeAttack {
public:
    SlimeAttack(ZWindow& zwindow);

    void tick();
    void knockback(float origin);
private:
    SlimeAttackEnemies slimeAttackEnemies;
    ZWindow& zwindow;
};