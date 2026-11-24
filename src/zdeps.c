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
#define RGFW_NO_X11_CURSOR_PRELOAD
#define RGFW_NO_X11_EXT_PRELOAD
#define RGFW_NO_X11_XI_PRELOAD
#define RGFW_NO_UNIX_CLOCK
#define RGFW_NO_ALLOCATE_MONITORS
#define RGFW_VULKAN
#define RGFW_EXPORT
#define RGFW_ASSERT(x) (void)(x)

/* AUDIO */
#define SOKOL_IMPL

/* STB_TRUETYPE */
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_ASSERT(x)

/* ZDEPS */
#define ZUTILS_IMPLEMENTATION
#define ZAUDIO_IMPLEMENTATION
#define ZCOLLIDE_IMPLEMENTATION

/* VULKAN */
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/* DEPENDENCIES */
#include "deps/RGFW.h"
#include "deps/sokol_audio.h"

#undef RGFW_IMPLEMENTATION
#undef SOKOL_AUDIO_IMPL

#include "zutils.h"
#include "zaudio.h"
#include "zcollide.h"
