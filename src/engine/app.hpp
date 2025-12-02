#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "renderSystem.hpp"

#define USE_MULTITHREADING

struct App {
public:
    App();
    void run();

private:
    #ifdef USE_MULTITHREADING
        void render();
    #endif

    AppWindow window{720, 480};
    Push vertex{};
    Device device{window};
    Renderer renderer{window, device};
    Pipeline pipeline;
    std::unique_ptr<RenderSystem> renderSystem;
};

void Terminal();