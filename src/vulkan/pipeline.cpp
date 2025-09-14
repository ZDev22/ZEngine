#include "pipeline.hpp"
#include "renderer.hpp"
#include "font.hpp"
#include "../deps/ZDev/math.hpp"

#include <filesystem>
#include <fstream>

Pipeline::Pipeline(Device& device, Renderer& renderer, const std::string& shader) : device(device), renderer(renderer) { createGraphicsPipeline(shader); }
Pipeline::~Pipeline() {
    if (graphicsPipeline != VK_NULL_HANDLE) { vkDestroyPipeline(device.device(), graphicsPipeline, nullptr); }
    if (pipelineLayout != VK_NULL_HANDLE) { vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr); }
    if (descriptorSetLayout != VK_NULL_HANDLE) { vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr); }
    if (descriptorPool != VK_NULL_HANDLE) { vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr); }
}

void Pipeline::bind(VkCommandBuffer commandBuffer) { vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); }
std::vector<char> Pipeline::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { throw("failed to open file: " + filepath); }
    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void Pipeline::createGraphicsPipeline(const std::string& shader) {
    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = createShaderModule(readFile("vulkan/shaders/" + shader + ".vert.spv"));
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = createShaderModule(readFile("vulkan/shaders/" + shader + ".frag.spv"));
    shaderStages[1].pName = "main";

    VkVertexInputBindingDescription bindingDescription = Model::Vertex::getBindingDescription();
    auto attributeDescriptions = Model::Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkDescriptorSetLayoutBinding bufferBinding{};
    bufferBinding.binding = 0;
    bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bufferBinding.descriptorCount = 1;
    bufferBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding imageBinding{};
    imageBinding.binding = 1;
    imageBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageBinding.descriptorCount = MAX_TEXTURES;
    imageBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding bindings[2] = { bufferBinding, imageBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) { throw("failed to create descriptor set layout!"); }

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

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) { throw("failed to create pipeline layout!"); }

    VkDescriptorPoolSize poolSizes[2] = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = MAX_TEXTURES;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = 1;

    if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) { throw("failed to create descriptor pool!"); }

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
    pipelineInfo.renderPass = renderer.getSwapChainRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) { throw("failed to create graphics pipeline!"); }
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

std::shared_ptr<Model> Pipeline::makeModel(const std::vector<glm::vec2>& positions) {

    std::vector<Model::Vertex> vertices;
    vertices.reserve(positions.size());
    for (size_t i = 0; i < positions.size(); i++) { vertices.emplace_back(positions[i], glm::vec2(positions[i] + glm::vec2(.5f))); }

    return std::make_shared<Model>(device, vertices);
}

void Pipeline::createSprite(std::shared_ptr<Model> model, int textureIndex, glm::vec2 position, glm::vec2 scale, float rotation, glm::vec4 color) {
    if (sprites.size() >= MAX_SPRITES) { throw("Maximum number of sprites exceeded!"); }
    Sprite sprite;
    SpriteData spriteData;

    sprite.model = model;
    if (textureIndex < 0 || textureIndex >= MAX_TEXTURES) { throw("Out of bounds texture!"); }
    sprite.texture = spriteTextures[textureIndex].get();
    sprite.visible = true;

    spriteData.position = position;
    spriteData.scale = scale;
    spriteData.rotation = rotation;
    spriteData.color = color;
    spriteData.textureIndex = textureIndex;

    sprites.push_back(spriteData);
    spriteCPU.push_back(sprite);
}

void Pipeline::createText(const std::string& file, const std::string& text, glm::vec2 position, float fontSize, glm::vec4 color) {
    if (spriteTextures.size() < MAX_TEXTURES) {
        spriteTextures.push_back(createFontTexture(device, *this, file, 32.f, 512, descriptorSetLayout, descriptorPool, fontCharData));
        Texture* fontTexture = spriteTextures[spriteTextures.size() - 1].get();
        int atlasSize = fontTexture->getTexWidth();
        float pixelHeight = 32.f;
        float scaleFactor = fontSize / pixelHeight;

        for (char c : text) {
            if (c < 32 || c > 127) { continue; }
            int charIndex = c - 32;
            stbtt_bakedchar& cd = fontCharData[charIndex];
            float xoff = cd.xoff * scaleFactor;
            float yoff = cd.yoff * scaleFactor;
            float width = (cd.x1 - cd.x0) * scaleFactor;
            float height = (cd.y1 - cd.y0) * scaleFactor;
            glm::vec2 pos = glm::vec2(position.x + xoff + width / 2, position.y + yoff + height / 2);
            glm::vec2 scale = glm::vec2(width, height);
            glm::vec2 uvOffset = glm::vec2(cd.x0 / (float)atlasSize, cd.y0 / (float)atlasSize);
            glm::vec2 uvScale = glm::vec2((cd.x1 - cd.x0) / (float)atlasSize, (cd.y1 - cd.y0) / (float)atlasSize);

            //createSprite(quadModel, spriteTextures.size() - 1, pos, scale, 0.f, color, uvOffset, uvScale);
            position.x += cd.xadvance * scaleFactor;
        }
    }
    else { throw("There are too many textures to store this font!"); }
}

void Pipeline::loadSprites() {
    sprites.clear();
    spriteCPU.clear();

    squareModel = makeModel({
        {-.5f, -.5f}, // Bottom-Left
        { .5f, -.5f}, // Bottom-Right
        {-.5f,  .5f}, // Top-Right
        { .5f,  .5f}  // Top-Left
    });

    //loadFlappyBird();
    loadSlimeAttack();
    //loadTerminalCalculator();

    for (int i = 0; i < fonts.size(); i++) { createText(fonts[i], "Hello", glm::vec2(.0f), .1f, glm::vec4(1.f)); }
}

void Pipeline::loadTextures() {
    while (texturePaths.size() < MAX_TEXTURES - fonts.size()) { texturePaths.push_back("e.jpg"); }

    spriteTextures.clear();
    spriteTextures.reserve(MAX_TEXTURES);

    for (const auto& path : texturePaths) {
        spriteTextures.push_back(std::make_unique<Texture>(device, path, descriptorSetLayout, descriptorPool, *this));
    }
}

void Pipeline::loadFlappyBird() {
    texturePaths = { "flappyBird.png", "pipe.png" };
    fonts = { "Bullpen3D.ttf" };
    loadTextures();

    createSprite(squareModel, 0, glm::vec2(-.7f, -.2f), glm::vec2(.1f, .1f), 0.f, glm::vec4(1.f));

    for (float i = 1.f; i < 5.f; i++) {
        float y = randomFloat(.4f, 1.4f);
        createSprite(squareModel, 1, glm::vec2(i, y), glm::vec2(.15f, 1.5f), 0.f, glm::vec4(1.f));
        createSprite(squareModel, 1, glm::vec2(i, y - 2.f), glm::vec2(.15f, 1.5f), 180.f, glm::vec4(1.f));
    }
}

void Pipeline::loadSlimeAttack() {
    texturePaths = { "flappyBird.png", "pipe.png" };
    fonts = {};
    loadTextures();
    
    createSprite(squareModel, 1, glm::vec2(0.f), glm::vec2(.15f, .15f), 0.f, glm::vec4(1.f));
    createSprite(squareModel, 1, glm::vec2(0.f, .7f), glm::vec2(2.f, .15f), 0.f, glm::vec4(1.f));
}

void Pipeline::loadTerminalCalculator() {
    texturePaths = { "terminalCalculator.png" };
    fonts = {};
    loadTextures();
    
    createSprite(squareModel, 0, glm::vec2(0.f), glm::vec2(2.f, 1.f), 0.f, glm::vec4(1.f));
}