#include "pipeline.hpp"
#include "global.hpp"

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cassert>
#include <array>

Pipeline::Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, VkRenderPass renderPass)
    : device{ device } {
    createGraphicsPipeline(vertFilepath, fragFilepath, renderPass);
}

Pipeline::~Pipeline() {
    vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
    vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
    vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
    std::cout << "Destroying Pipeline" << std::endl;
}

void Pipeline::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
}