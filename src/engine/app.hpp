#pragma once

#include "renderer.hpp"
#include "renderSystem.hpp"

struct App {
public:
    App();
    void run();

private:
    void render();

    AppWindow window{720, 480};
    Push vertex{};
    Device device{window};
    Renderer renderer{window, device};
    Keyboard keyboard{window.getWindow()};
    Pipeline pipeline;
    std::unique_ptr<RenderSystem> renderSystem;
};

void Terminal();