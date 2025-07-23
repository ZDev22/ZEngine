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
    AudioPlayer() { ma_engine_init(nullptr, &engine); }
    ~AudioPlayer() { ma_engine_uninit(&engine); }

    void play(const std::string& filepath) { ma_engine_play_sound(&engine, ("assets/sounds/" + filepath).c_str(), nullptr); }
private:
    ma_engine engine;
};

#endif