#ifndef FLAPPYBIRD_HPP
#define FLAPPYBIRD_HPP_HPP

#define AUDIOPLAYER_IMPLEMENTATION
#include "../../deps/ZDev/audio.hpp"

#include "../../deps/ZDev/keyboard.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

class AudioPlayer;
class FlappyBird {
public:
    FlappyBird(Keyboard& keyboard, Pipeline& pipeline, Push& push);

    void tick();
private:
    Keyboard& keyboard;
    Pipeline& pipeline;
    Push& push;

    AudioPlayer audio;
};

#endif