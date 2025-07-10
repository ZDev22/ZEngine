#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "functions/keyboard.hpp"
#include "../vulkan/pipeline.hpp"

class Program {
public:
    Program(Keyboard& keyboard, Pipeline& pipeline);

    void tick();
private:
    Keyboard& keyboard;
    Pipeline& pipeline;
};

#endif