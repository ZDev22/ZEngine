#define MINIAUDIO_IMPLEMENTATION
#include "deps/miniaudio.h"

#include "audio.hpp"

#include <iostream>
#include <filesystem>

AudioPlayer::AudioPlayer() { ma_engine_init(nullptr, &engine); }
AudioPlayer::~AudioPlayer() { ma_engine_uninit(&engine); }
void AudioPlayer::play(const std::string& filepath) {
    ma_engine_play_sound(&engine, ("sounds/" + filepath).c_str(), nullptr);
}