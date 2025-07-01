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
<<<<<<< HEAD
    
=======

>>>>>>> 8bd45812fdf72ff958a206dc0592dced42f5c02a
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();

private:
    AppWindow window{1280, 720};
    Device device{window};
    Renderer renderer{window, device};
    Keyboard keyboard{window};
    Program program{keyboard};
    std::unique_ptr<Pipeline> pipeline;
    std::unique_ptr<Global> global; 
    std::unique_ptr<RenderSystem> renderSystem;
};