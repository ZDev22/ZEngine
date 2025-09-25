#pragma once

#include "../../deps/miniaudio.h"
#include "../../deps/ZDev/keyboard.hpp"
#include "../../deps/ZDev/collision.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

class AudioPlayer;
class FlappyBird {
public:
    FlappyBird(Keyboard& keyboard, ma_engine& audio, Pipeline& pipeline, Collision& collision);

    void tick();
private:
    Keyboard& keyboard;
    Collision& collision;
    Pipeline& pipeline;
    ma_engine& audio;
};