/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

This file is designed to take the weight of compiling all dependencies away from main.c
To use along-side zengine.h, use #define ZENGINE_DEPS_DEFINED */

/* vulkan */
#if defined(__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(__APPLE__)
    #define VK_USE_PLATFORM_MACOS_MVK
#elif defined(_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

/* RGFW */
#define RGFW_IMPLEMENTATION
#define RGFW_USE_INT
#define RGFW_NO_X11_CURSOR_PRELOAD
#define RGFW_NO_X11_EXT_PRELOAD
#define RGFW_NO_X11_XI_PRELOAD
#define RGFW_NO_UNIX_CLOCK
#define RGFW_NO_ALLOCATE_MONITORS
#define RGFW_VULKAN
#define RGFW_EXPORT
#define RGFW_ASSERT(x) (void)(x)

/* MINIAUDIO */
#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_RUNTIME_LINKING
#define MA_64BIT
/* disable decoders */
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#if defined(__linux__)
    #define MA_ENABLE_PULSEAUDIO
#elif defined(_APPLE_)
    #define MA_ENABLE_COREAUDIO
#elif defined(_WIN32_)
    #define MA_ENABLE_WINMM
    #define MA_ENABLE_WASAPI
#else
    #define MA_NULL
#endif
#define MA_NO_FLAC
#define MA_NO_WAV
#define MA_NO_VORBIS
#define MA_NO_OPUS

/* STB_IMAGE */
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT(x)
/* disable a bunch of random image types (supported image types are png and jpg) */
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_LINEAR

/* STB_TRUETYPE */
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_ASSERT(x)

/* DEPENDENCIES */
#include "deps/RGFW.h"
#include "deps/miniaudio.h"
#include "deps/stb_image.h"
#include "deps/stb_truetype.h"

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#undef RGFW_IMPLEMENTATION
#undef MINIAUDIO_IMPLEMENTATION
#undef ZCOLLIDE_IMPLEMENTATION
#undef ZMATH_IMPLEMENTATION
#undef ZTEXT_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION

/* zdeps */
#define ZCOLLIDE_IMPLEMENTATION
#define ZMATH_IMPLEMENTATION
#define ZTEXT_IMPLEMENTATION

#include "zcollide.h"
#include "zmath.h"
#include "ztext.h"
