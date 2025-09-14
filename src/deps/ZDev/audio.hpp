#pragma once

#ifdef AUDIOPLAYER_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
#endif
#include "../miniaudio.h"

class AudioPlayer {
public:
    AudioPlayer() { ma_engine_init(nullptr, &engine); }
    ~AudioPlayer() { ma_engine_uninit(&engine); }

    void play(const char* filepath) { ma_engine_play_sound(&engine, filepath, nullptr); }
private:
    ma_engine engine;
};