#pragma once

#include "deps/miniaudio.h"

#include "zwindow.hpp"
#include "collision.hpp"
#include "zengine.hpp"

struct FlappyBird {
public:
    FlappyBird(ZWindow& zwindow, ma_engine& audio, Collision& collision, Push& vertex);

    void tick();
private:
    ZWindow& zwindow;
    Collision& collision;
    ma_engine& audio;
    Push& vertex;
};