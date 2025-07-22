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
    inline AudioPlayer() {
        ma_result result = ma_engine_init(nullptr, &engine);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to initialize audio engine: " << result << std::endl;
        }
    }

    inline ~AudioPlayer() {
        ma_engine_uninit(&engine);
    }

    inline void play(const std::string& filepath) {
        std::string full_path = "assets/sounds/" + filepath;
        if (!std::filesystem::exists(full_path)) {
            std::cerr << "Audio file not found: " << full_path << std::endl;
            return;
        }
        ma_result result = ma_engine_play_sound(&engine, full_path.c_str(), nullptr);
        if (result != MA_SUCCESS) {
            std::cerr << "Failed to play sound: " << result << std::endl;
        }
    }

private:
    ma_engine engine;
};

#endif // AUDIO_HPP