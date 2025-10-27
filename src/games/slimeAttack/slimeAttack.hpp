#pragma once

#include "../../deps/ZDev/keyboard.hpp"
#include "../../deps/ZDev/collision.hpp"
#include "../../engine/pipeline.hpp"
#include "../../engine/global.hpp"

#include "slimeAttackEnemies.hpp"

#define NO_DECOR false // if your computer can't render window titles, enable this
#define NO_FPS false // if you don't want to display FPS, endable this
#define FPS_CAP_SET true // set if the program will use the FPS cap
#define FPS_CAP .5f / 60.f // set the framerate
#define USE_MOUSE false // if your program uses the mouse, set this to true

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