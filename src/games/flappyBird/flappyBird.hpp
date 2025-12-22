#pragma once

#include "deps/miniaudio.h"
#include "deps/ZDeps/keyboard.hpp"
#include "deps/ZDeps/collision.hpp"
#include "engine/pipeline.hpp"
#include "engine/global.hpp"

#define FPS_CAP_SET
#define FPS_CAP 60.f
#define USE_MOUSE

struct FlappyBird {
public:
    FlappyBird(Keyboard& keyboard, ma_engine& audio, Pipeline& pipeline, Collision& collision, Push& vertex);

    void tick();
private:
    Keyboard& keyboard;
    Collision& collision;
    Pipeline& pipeline;
    ma_engine& audio;
    Push& vertex;
};