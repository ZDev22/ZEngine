#pragma once

#include "../../deps/ZDev/audio.hpp"
#include "../../deps/ZDev/keyboard.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

#include "slimeAttackEnemies.hpp"

class AudioPlayer;
class SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Push& push);

    void tick();
private:
    SlimeAttackEnemies slimeAttackEnemies{*this};
    Keyboard& keyboard;
    Pipeline& pipeline;
    Push& push;

    AudioPlayer& audio;
};