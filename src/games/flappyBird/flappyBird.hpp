#pragma once

#include "../../deps/miniaudio.h"
#include "../../deps/ZDev/keyboard.hpp"
#include "../../deps/ZDev/collision.hpp"
#include "../../engine/pipeline.hpp"
#include "../../engine/global.hpp"

#define NO_DECOR true // if your computer can't render window titles, enable this
#define NO_FPS false // if you don't want to display FPS, endable this
#define FPS_CAP_SET false // set if the program will use the FPS cap
#define FPS_CAP .5f / 60.f // set the framerate
#define USE_MOUSE false // if your program uses the mouse, set this to true

class AudioPlayer;
class FlappyBird {
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