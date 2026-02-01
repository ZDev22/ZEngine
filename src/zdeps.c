/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

This file is designed to take the weight of compiling all dependencies away from main.cpp */

#define RGFW_IMPLEMENTATION
#define RGFW_VULKAN
#define RGFW_EXPORT
#define RGFW_ASSERT(x) (void)(x)

#define MINIAUDIO_IMPLEMENTATION

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ASSERT

#include "deps/RGFW.h"
#include "deps/miniaudio.h"
#include "deps/stb_image.h"