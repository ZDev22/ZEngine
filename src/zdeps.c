/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

This file is designed to take the weight of compiling all dependencies away from main.cpp
To use along-side zengine.hpp, #define ZENGINE_DEPS_DEFINED */

/* RGFW */
#define RGFW_IMPLEMENTATION
#define RGFW_VULKAN
#define RGFW_EXPORT
#define RGFW_ASSERT(x) (void)(x)

/* MINIAUDIO */
#define MINIAUDIO_IMPLEMENTATION
/* supported audio backends */
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#if defined(__linux__)
    #define MA_ENABLE_ALSA
    #define MA_ENABLE_PULSEAUDIO
    #define MA_ENABLE_JACK
#elif defined(_APPLE_)
    #define MA_ENABLE_COREAUDIO
#elif defined(_WIN32_)
    #define MA_ENABLE_WINMM
    #define MA_ENABLE_WASAPI
#else
    #define MA_NULL
#endif
/* disabled decoders */
#define MA_NO_FLAC

/* STB_IMAGE */
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT

/* STB_TRUETYPE */
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_ASSERT

/* DEPENDENCIES */
#include "deps/RGFW.h"
#include "deps/miniaudio.h"
#include "deps/stb_image.h"
#include "deps/stb_truetype.h"