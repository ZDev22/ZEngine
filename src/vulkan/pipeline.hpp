#pragma once

#include "device.hpp"
#include "sprite.hpp"
#include "texture.hpp"
#include "global.hpp"

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <string>

extern std::vector<std::unique_ptr<Texture>> spriteTextures;

class RenderSystem;
class Renderer;
class Pipeline {
public:
    Pipeline(Device& device, RenderSystem& renderSystem, Renderer& renderer, const std::string& vertFilepath, const std::string& fragFilepath);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    void loadSprites();
    void bind(VkCommandBuffer commandBuffer);
    VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; } 
    VkDescriptorPool getDescriptorPool() const { return descriptorPool; }

    int switchTexture(Sprite& sprite, int textureID);
    std::shared_ptr<Model> makeModel(const std::vector<glm::vec2>& positions);
    void createSprite(std::shared_ptr<Model> model, int textureIndex, glm::vec2 position, glm::vec2 scale, float rotation, glm::vec4 color);

private:
    static std::vector<char> readFile(const std::string& filepath);
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void setTexture(int textureID);

    Device& device;
    RenderSystem& renderSystem;
    Renderer& renderer;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;

    std::vector<std::string> texturePaths = { "images/FlappyBird.png", "images/pipe.png"};
    std::shared_ptr<Model> quadModel;
};