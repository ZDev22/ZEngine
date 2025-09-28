#pragma once

#include "../deps/stb_truetype.h"
#include "device.hpp"
#include "texture.hpp"

#include <memory>

class Pipeline;

unsigned char* loadTTF(const std::string& filepath);