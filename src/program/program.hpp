#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "functions/keyboard.hpp"
#include "../vulkan/pipeline.hpp"
#include "../vulkan/global.hpp"

class Program {
public:
    Program(Keyboard& keyboard, Pipeline& pipeline, Push& push);

    void tick();
private:
    Keyboard& keyboard;
    Pipeline& pipeline;
    Push& push;
};

#endif