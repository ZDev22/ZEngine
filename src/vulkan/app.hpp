#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "pipeline.hpp"
#include "renderSystem.hpp"
#include "global.hpp" 

#include "../program/program.hpp"
#include "../program/functions/keyboard.hpp"

#include <memory>

class App {
public:
    App();

    void run();
    void render();

private:
    AppWindow window{720, 480};
    Push push{};
    Device device{window};
    Renderer renderer{window, device};
    Keyboard keyboard{window};
    std::unique_ptr<Pipeline> pipeline;
    std::unique_ptr<Global> global; 
    std::unique_ptr<RenderSystem> renderSystem;
    Program program{keyboard, *pipeline, push};
};