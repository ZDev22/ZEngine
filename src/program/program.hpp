#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "functions/keyboard.hpp"

#include <vulkan/vulkan.h>

class Program {
public:
    Program(Keyboard& keyboard, Device& device, Pipeline& pipeline, VkDescriptorSetLayout& descriptorSetLayout, VkDescriptorPool& descriptorPool);

    void tick();
private:
    Keyboard& keyboard;
    Device& device;
    Pipeline& pipeline;
    Texture& texture;
    VkDescriptorSetLayout& descriptorSetLayout;
    VkDescriptorPool& descriptorPool;
};

#endif