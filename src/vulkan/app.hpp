#pragma once

#include "window.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "pipeline.hpp"
#include "renderSystem.hpp"
#include "global.hpp" 

#include "../deps/ZDev/keyboard.hpp"

//#include "../games/flappyBird/flappyBird.hpp"
#include "../games/slimeAttack/slimeAttack.hpp"

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
    Pipeline pipeline;
    std::unique_ptr<RenderSystem> renderSystem;

    //FlappyBird flappyBird{keyboard, pipeline, push};
    SlimeAttack slimeAttack{keyboard, pipeline, push};
};