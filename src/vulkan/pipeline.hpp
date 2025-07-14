#pragma once

#include "device.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "global.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <string>

class Pipeline {
public:
    Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, VkRenderPass renderPass);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void loadSprites();
    void bind(VkCommandBuffer commandBuffer);
    VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; } 
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

    int switchTexture(Sprite& sprite, int textureID);
    void createSprite(std::shared_ptr<Model> model, Texture* texture, glm::vec2 position, glm::vec2 scale, float rotation, glm::vec4 color, int textureIndex);

private:
    static std::vector<char> readFile(const std::string& filepath);
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, VkRenderPass renderPass);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    std::shared_ptr<Model> makeModel(const std::vector<glm::vec2>& positions);
    void setTexture(int textureID);

    Device& device;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;

    std::vector<std::string> texturePaths = { "images/FlappyBird.png", "images/pipe.png"};
    std::vector<std::unique_ptr<Texture>> spriteTextures;
    std::shared_ptr<Model> quadModel;
};