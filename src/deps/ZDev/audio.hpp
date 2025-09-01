#ifndef AUDIO_HPP
#define AUDIO_HPP

#ifdef AUDIOPLAYER_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
#endif

#include "../miniaudio.h"

#include <string>

class AudioPlayer {
public:
    AudioPlayer() { ma_engine_init(nullptr, &engine); }
    ~AudioPlayer() { ma_engine_uninit(&engine); }

    void play(const std::string& filepath) { ma_engine_play_sound(&engine, ("assets/sounds/" + filepath).c_str(), nullptr); }
private:
    ma_engine engine;
};

#endif