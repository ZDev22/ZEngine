#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "pipeline.hpp"
#include "renderSystem.hpp"
#include "global.hpp" 

#include "../games/functions/keyboard.hpp"

#include "../games/flappyBird/program.hpp"
//#include "../games/crossyRoad/program.hpp" - how this could work
//#include "../games/RPG/program.hpp"

class App {
public:
    App();
    void run();

private:
    void render();
    
    AppWindow window{720, 480};
    Push push{};
    Device device{window};
    Renderer renderer{window, device};
    Keyboard keyboard{window};
    std::unique_ptr<Pipeline> pipeline;
    std::unique_ptr<RenderSystem> renderSystem;
    Program program{keyboard, *pipeline, push};
};