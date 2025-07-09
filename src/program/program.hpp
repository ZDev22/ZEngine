#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "functions/keyboard.hpp"

class Program {
public:
    Program(Keyboard& keyboard);

    void tick();
private:
    Keyboard& keyboard;
};

#endif