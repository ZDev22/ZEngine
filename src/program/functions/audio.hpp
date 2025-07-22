#pragma once

#define MA_NO_DECODING
#define MA_NO_ENCODING
#include "deps/miniaudio.h"

#include <string>

class AudioPlayer {
public:
    AudioPlayer();
    ~AudioPlayer();

    void play(const std::string& filepath);

private:
    ma_engine engine;
};