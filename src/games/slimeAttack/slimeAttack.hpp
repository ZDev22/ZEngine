#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../functions/audio.hpp"
#include "../functions/keyboard.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

class AudioPlayer;
class SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, Pipeline& pipeline, Push& push);

    void tick();
private:
    Keyboard& keyboard;
    Pipeline& pipeline;
    Push& push;

    AudioPlayer audio;
};

#endif