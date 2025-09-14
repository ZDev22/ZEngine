#pragma once

#include "../../deps/ZDev/audio.hpp"
#include "../../deps/ZDev/keyboard.hpp"
#include "../../vulkan/pipeline.hpp"
#include "../../vulkan/global.hpp"

#include "slimeAttackEnemies.hpp"

#include <glm/glm.hpp>

class AudioPlayer;
class SlimeAttack {
public:
    SlimeAttack(Keyboard& keyboard, AudioPlayer& audio, Pipeline& pipeline, Collision& collision, Push& push);

    void tick();
    void knockback(float origin);
private:
    SlimeAttackEnemies slimeAttackEnemies;

    Keyboard& keyboard;
    AudioPlayer& audio;
    Pipeline& pipeline;
    Collision& collision;
    Push& push;
};