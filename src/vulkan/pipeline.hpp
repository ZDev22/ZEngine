#pragma once

#include "device.hpp"
#include "sprite.hpp"
#include "global.hpp"
#include "../deps/stb_truetype.h"

class Renderer;
class Pipeline {
public:
    Pipeline(Device& device, Renderer& renderer, const std::string& shader);
    ~Pipeline();

    void loadSprites();
    void bind(VkCommandBuffer commandBuffer);
    VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
    VkDescriptorSetLayout& getDescriptorSetLayout() { return descriptorSetLayout; }
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

    int switchTexture(Sprite& sprite, int textureID);
    std::shared_ptr<Model> makeModel(const std::vector<float>& positions);
    void createSprite(std::shared_ptr<Model> model, int textureIndex, float positionx, float positiony, float scalex, float scaley, float rotation, float r, float g, float b, float a);
    void createText(const std::string& file, const std::string& text, float positionx, float positiony, float fontSize, float r, float g, float b, float a);
    std::shared_ptr<Model> getSquareModel() { return squareModel; }

private:
    static std::vector<char> readFile(const std::string& filepath);
    void createGraphicsPipeline(const std::string& shader);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    void loadTextures();
    void loadFlappyBird();
    void loadSlimeAttack();
    void loadTerminalCalculator();

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