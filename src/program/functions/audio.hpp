#pragma once

#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <string>
#include <filesystem>
#include <iostream>

#ifdef AUDIOPLAYER_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "deps/miniaudio.h"

class AudioPlayer {
public:
    inline AudioPlayer() { ma_engine_init(nullptr, &engine); }
    inline ~AudioPlayer() { ma_engine_uninit(&engine); }

    inline void play(const std::string& filepath) { ma_result result = ma_engine_play_sound(&engine, ("assets/sounds/" + filepath).c_str(), nullptr); }
private:
    ma_engine engine;
};

#endif