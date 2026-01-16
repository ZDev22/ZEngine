#pragma once

#include "deps/miniaudio.h"

#include "zwindow.hpp"
#include "zengine.hpp"

struct FlappyBird {
public:
    FlappyBird(ZWindow& zwindow, ma_engine& audio, Push& vertex);

    void tick();
private:
    ZWindow& zwindow;
    ma_engine& audio;
    Push& vertex;
};