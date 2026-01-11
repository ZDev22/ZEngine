#pragma once

#include "deps/miniaudio.h"
#include "deps/ZDeps/keyboard.hpp"
#include "deps/ZDeps/collision.hpp"
#include "zengine.hpp"

struct FlappyBird {
public:
    FlappyBird(Keyboard& keyboard, ma_engine& audio, Collision& collision, Push& vertex);

    void tick();
private:
    Keyboard& keyboard;
    Collision& collision;
    ma_engine& audio;
    Push& vertex;
};