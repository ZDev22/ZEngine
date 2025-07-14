#pragma once

#include "window.hpp"

#include <glm/glm.hpp>

extern float deltaTime;
extern uint16_t imageCount;

struct Push { glm::mat4 projection; };
class Global {
public:
    Global(AppWindow& window) : window(window), aspectRatio(1.0f) { setAspectRatio();  }

    void setAspectRatio() {
        extent = window.getExtent();
        aspectRatio = static_cast<float>(extent.width) / extent.height;
    }

    float getAspectRatio() const { return aspectRatio; }

private:
    VkExtent2D extent;
    float aspectRatio;
    AppWindow& window;
};