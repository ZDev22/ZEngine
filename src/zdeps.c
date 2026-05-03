/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

This file is designed to take the weight of compiling all dependencies away from main.c
To use along-side zengine.h, use #define ZENGINE_DEPS_DEFINED */

/* RGFW */
#define RGFW_IMPLEMENTATION
#define RGFW_VULKAN
#define RGFW_EXPORT
#define RGFW_ASSERT(x) (void)(x)

/* MINIAUDIO */
#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#if defined(__linux__)
    #define MA_ENABLE_ALSA
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
#define MA_NO_WAV
#define MA_NO_VORBIS
#define MA_NO_OPUS

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

#undef RGFW_IMPLEMENTATION
#undef MINIAUDIO_IMPLEMENTATION
#undef ZCOLLIDE_IMPLEMENTATION
#undef ZMATH_IMPLEMENTATION
#undef ZTEXT_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION

/* ZDEPS */
#define ZCOLLIDE_IMPLEMENTATION
#define ZMATH_IMPLEMENTATION
#define ZTEXT_IMPLEMENTATION
#include "zcollide.h"
#include "zmath.h"
#include "ztext.h"

