#pragma once

#include "../../deps/ZDev/audio.hpp"
#include "../../deps/ZDev/keyboard.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

class AudioPlayer;
class SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Push& push);

    void tick();
    void knockBack(float origin);
private:
    std::unique_ptr<SlimeAttackEnemies> slimeAttackEnemies;

    Keyboard& keyboard;
    Pipeline& pipeline;
    Push& push;

    AudioPlayer& audio;
};