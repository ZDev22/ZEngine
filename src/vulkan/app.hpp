#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "renderSystem.hpp"

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
    Keyboard keyboard{window.getWindow()};
    Pipeline pipeline;
    std::unique_ptr<RenderSystem> renderSystem;
};