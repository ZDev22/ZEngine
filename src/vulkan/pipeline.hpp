#pragma once

#include "device.hpp"
#include "global.hpp"
#include "model.hpp"
#include "../deps/stb_truetype.h"

class Renderer;
class Sprite;
class Pipeline {
public:
    Pipeline(Device& device, Renderer& renderer, const std::string& shader);
    ~Pipeline();

    void bind(VkCommandBuffer commandBuffer);
    VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
    VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

    int switchTexture(Sprite& sprite, int textureID);
    std::shared_ptr<Model> makeModel(const std::vector<float>& positions);
    void createSprite(std::shared_ptr<Model> model, unsigned int textureIndex, float positionx, float positiony, float scalex, float scaley, float rotation, float r, float g, float b, float a);
    void createText(unsigned int file, const std::string& text, float fontSize, unsigned int textureIndex);
    std::shared_ptr<Model> getSquareModel() { return squareModel; }

    void loadFlappyBird();
    void loadSlimeAttack();
    void loadTerminalCalculator();

private:
    static std::vector<char> readFile(const std::string& filepath);
    void createGraphicsPipeline(const std::string& shader);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void loadSprites();

    Device& device;
    Renderer& renderer;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;

    std::vector<std::string> texturePaths;
    std::vector<std::string> fonts;
    std::shared_ptr<Model> squareModel;
    std::vector<stbtt_bakedchar> fontCharData;
};