#include "pipeline.hpp"
#include "global.hpp"
#include "../program/functions/math.hpp"

#include <filesystem>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cassert>
#include <array>

Pipeline::Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, VkRenderPass renderPass) : device{ device } { createGraphicsPipeline(vertFilepath, fragFilepath, renderPass); }
Pipeline::~Pipeline() {
    vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
    vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
    vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
    std::cout << "Destroying Pipeline" << std::endl;
}

void Pipeline::bind(VkCommandBuffer commandBuffer) { vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); }
std::vector<char> Pipeline::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { throw std::runtime_error("failed to open file: " + filepath); }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void Pipeline::createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, VkRenderPass renderPass) {
    std::cout << "[Pipeline] Reading vertex shader: " << vertFilepath << std::endl;
    vertShaderModule = createShaderModule(readFile(vertFilepath));
    std::cout << "[Pipeline] Vertex shader loaded and module created" << std::endl;

    std::cout << "[Pipeline] Reading fragment shader: " << fragFilepath << std::endl;
    fragShaderModule = createShaderModule(readFile(fragFilepath));
    std::cout << "[Pipeline] Fragment shader loaded and module created" << std::endl;

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";
    std::cout << "[Pipeline] Shader stages created" << std::endl;

    VkVertexInputBindingDescription bindingDescription = Model::Vertex::getBindingDescription();
    auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    std::cout << "[Pipeline] Vertex input state created" << std::endl;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    std::cout << "[Pipeline] Input assembly state created" << std::endl;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;
    std::cout << "[Pipeline] Viewport state created" << std::endl;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    std::cout << "[Pipeline] Rasterization state created" << std::endl;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    std::cout << "[Pipeline] Multisample state created" << std::endl;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    std::cout << "[Pipeline] Color blend state created" << std::endl;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    std::cout << "[Pipeline] Depth-stencil state created" << std::endl;

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();
    std::cout << "[Pipeline] Dynamic state created" << std::endl;

    if (texturePaths.empty()) {
        std::cerr << "[Pipeline][ERROR] texturePaths is empty! Cannot create descriptor set layout." << std::endl;
        throw std::runtime_error("No textures provided for pipeline!");
    }
    VkDescriptorSetLayoutBinding bufferBinding{};
    bufferBinding.binding = 0;
    bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bufferBinding.descriptorCount = 1;
    bufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding imageBinding{};
    imageBinding.binding = 1;
    imageBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageBinding.descriptorCount = texturePaths.size();
    imageBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { bufferBinding, imageBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult res = vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &descriptorSetLayout);
    std::cout << "[Pipeline] vkCreateDescriptorSetLayout returned: " << res << std::endl;
    if (res != VK_SUCCESS) {
        std::cerr << "[Pipeline][ERROR] Failed to create descriptor set layout! VkResult: " << res << std::endl;
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    std::cout << "[Pipeline] Descriptor set layout created: " << descriptorSetLayout << std::endl;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Push);
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    res = vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
    std::cout << "[Pipeline] vkCreatePipelineLayout returned: " << res << std::endl;
    if (res != VK_SUCCESS) {
        std::cerr << "[Pipeline][ERROR] Failed to create pipeline layout! VkResult: " << res << std::endl;
        throw std::runtime_error("failed to create pipeline layout!");
    }
    std::cout << "[Pipeline] Pipeline layout created with descriptor set layout: " << descriptorSetLayout << std::endl;

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = texturePaths.size();

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    res = vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &descriptorPool);
    std::cout << "[Pipeline] vkCreateDescriptorPool returned: " << res << std::endl;
    if (res != VK_SUCCESS) {
        std::cerr << "[Pipeline][ERROR] Failed to create descriptor pool! VkResult: " << res << std::endl;
        throw std::runtime_error("failed to create descriptor pool!");
    }
    std::cout << "[Pipeline] Descriptor pool created: " << descriptorPool << std::endl;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    res = vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
    std::cout << "[Pipeline] vkCreateGraphicsPipelines returned: " << res << std::endl;
    if (res != VK_SUCCESS) {
        std::cerr << "[Pipeline][ERROR] Failed to create graphics pipeline! VkResult: " << res << std::endl;
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    std::cout << "[Pipeline] Graphics pipeline created successfully." << std::endl;
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    vkCreateShaderModule(device.device(), &createInfo, nullptr, &shaderModule);
    return shaderModule;
}

void Pipeline::setTexture(int textureID) { spriteTextures[textureID] = std::make_unique<Texture>(device, texturePaths, descriptorSetLayout, descriptorPool, *this); }
std::shared_ptr<Model> Pipeline::makeModel(const std::vector<glm::vec2>& positions) {
    glm::vec2 baseMin = {-0.5f, -0.5f};
    glm::vec2 baseSize = {1.0f, 1.0f};

    std::vector<Model::Vertex> vertices;
    for (const auto& pos : positions) {
        glm::vec2 uv = (pos - baseMin) / baseSize;
        vertices.push_back({ pos, uv });
    }

    std::vector<uint32_t> indices(positions.size());
    for (uint32_t i = 0; i < positions.size(); ++i) { indices[i] = i; }
    return std::make_shared<Model>(device, vertices, indices);
}

void Pipeline::loadSprites() {
    std::cout << "Starting sprite loading...\n";
    for (size_t f = 0; f < texturePaths.size(); f++) { texturePaths[f] = "images/" + texturePaths[f]; }

    sprites.clear();
    spriteCPU.clear();

    auto quadModel = makeModel({
        {-0.5f, -0.5f}, // Bottom-Left  (Vertex 0)
        { 0.5f, -0.5f}, // Bottom-Right (Vertex 1)
        {-0.5f,  0.5f}, // Top-Right    (Vertex 2)
        { 0.5f,  0.5f}  // Top-Left     (Vertex 3)
    });
    auto texture = std::make_unique<Texture>(device, texturePaths[0], descriptorSetLayout, descriptorPool, *this);

    Sprite sprite;
    SpriteData spriteData;

    sprite.model = quadModel;
    sprite.texture = texture.get();
    sprite.textureIndex = 0;

    spriteData.translation = glm::vec2(-.7f, -.2f);
    spriteData.scale = glm::vec2(.1f, .1f);
    spriteData.rotation = 0.0f;
    spriteData.color = glm::vec4(1.0f);
    spriteData.textureIndex = 0;

    sprites.push_back(spriteData);
    spriteCPU.push_back(sprite);
    spriteTextures.push_back(std::move(texture));

    //Pipes
    texture = std::make_unique<Texture>(device, texturePaths[1], descriptorSetLayout, descriptorPool, *this);
    sprite.texture = texture.get();
    sprite.textureIndex = 1;

    spriteData.textureIndex = 1;
    spriteData.scale = glm::vec2(.15f, 1.5f);

    for (int i = 1; i < 4; i++) {
        spriteData.translation = glm::vec2(1.f + (i / 2), randomFloat(.8f, .2f));

        sprites.push_back(spriteData);
        spriteCPU.push_back(sprite);
        spriteTextures.push_back(std::move(texture));
    }

    std::cout << "Sprites created: " << sprites.size() << std::endl;
}