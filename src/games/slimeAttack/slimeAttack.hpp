#ifndef SLIMEATTACK_HPP
#define SLIMEATTACK_HPP

#include "../../deps/ZDev/audio.hpp"
#include "../../deps/ZDev/keyboard.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

class AudioPlayer;
class SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, Pipeline& pipeline, Push& push);

    void init();
    void tick();
private:
    Keyboard& keyboard;
    Pipeline& pipeline;
    Push& push;

    AudioPlayer audio;
};

#endif