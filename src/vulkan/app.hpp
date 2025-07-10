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
    
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    void run();
    void render();

private:
    AppWindow window{1280, 720};
    Device device{window};
    Renderer renderer{window, device};
    Keyboard keyboard{window};
    std::unique_ptr<Pipeline> pipeline;
    std::unique_ptr<Global> global; 
    std::unique_ptr<RenderSystem> renderSystem;
};