#include "pipeline.hpp"
#include "renderer.hpp"
#include "sprite.hpp"
#include "font.hpp"
#include "../deps/ZDev/math.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../deps/stb_truetype.h"

#include <filesystem>
#include <fstream>

#ifdef _WIN32
vector<vector<stbtt_bakedchar>> fontCharDatas;
#endif

Pipeline::Pipeline(Device& device, Renderer& renderer, const std::string& shader) : device(device), renderer(renderer) { createGraphicsPipeline(shader); }

Pipeline::~Pipeline() {
    vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
}

void Pipeline::bind(VkCommandBuffer commandBuffer) { vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline); }

std::vector<char> Pipeline::readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { throw("failed to open file: " + filepath); }
    unsigned int fileSize = static_cast<unsigned int>(file.tellg());
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
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<unsigned int>(attributeDescriptions.size());
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

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkDescriptorSetLayoutBinding layoutBindings[2] = {};
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[1].descriptorCount = MAX_TEXTURES;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = layoutBindings;

    if (vkCreateDescriptorSetLayout(device.device(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) { throw std::runtime_error("failed to create descriptor set layout!"); }

    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_TEXTURES * (MAX_TEXTURES + 1) }
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = MAX_TEXTURES + 1;

    if (vkCreateDescriptorPool(device.device(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) { throw std::runtime_error("failed to create descriptor pool!"); }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {  throw std::runtime_error("failed to create pipeline layout!"); }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderer.getSwapChainRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) { throw std::runtime_error("failed to create graphics pipeline!"); }

    vkDestroyShaderModule(device.device(), shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device.device(), shaderStages[1].module, nullptr);
}

VkShaderModule Pipeline::createShaderModule(const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device.device(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) { throw std::runtime_error("failed to create shader module!"); }
    return shaderModule;
}

std::shared_ptr<Model> Pipeline::makeModel(const std::vector<float>& positions) {
    if (positions.size() % 2 != 0) { throw std::runtime_error("Positions size must be a multiple of 2"); }

    std::vector<Model::Vertex> vertices;
    vertices.reserve(positions.size() / 2);

    for (unsigned int i = 0; i < positions.size(); i += 2) {
        Model::Vertex v{};
        v.position[0] = positions[i];
        v.position[1] = positions[i + 1];
        v.texCoord[0] = positions[i] + .5f;
        v.texCoord[1] = positions[i + 1] + .5f;
        vertices.push_back(v);
    }

    return std::make_shared<Model>(device, vertices);
}

void Pipeline::createSprite(std::shared_ptr<Model> model, unsigned int textureIndex, float positionx, float positiony, float scalex, float scaley, float rotation, float r, float g, float b, float a) {
    if (sprites.size() >= MAX_SPRITES) { throw("Maximum number of sprites exceeded!"); }
    Sprite sprite;
    SpriteData spriteData;

    sprite.model = model;
    sprite.texture = spriteTextures[textureIndex].get();
    sprite.visible = true;

    spriteData.position[0] = positionx;
    spriteData.position[1] = positiony;
    spriteData.scale[0] = scalex;
    spriteData.scale[1] = scaley;
    spriteData.rotation = rotation;
    spriteData.color[0] = r;
    spriteData.color[1] = g;
    spriteData.color[2] = b;
    spriteData.color[3] = a;
    spriteData.textureIndex = textureIndex;
    spriteData.ID = sprites.size();

    sprites.push_back(spriteData);
    spriteCPU.push_back(sprite);
}

void Pipeline::createText(unsigned int font, const std::string& text, float fontSize, unsigned int textureIndex) {
#ifdef _WIN32
    if (textureIndex < lastTexts.size() && lastTexts[textureIndex] == text) { return; }
    lastTexts[textureIndex] = text.c_str();

    if (font >= fontAtlases.size() || fontSizes[font] != fontSize) {
        fontAtlases.resize(font + 1);
        fontCharDatas.resize(font + 1);
        fontSizes.resize(font + 1);

        unsigned int atlasSize = static_cast<unsigned int>(fontSize * 8.0f);
        fontAtlases[font].resize(atlasSize * atlasSize);
        fontCharDatas[font].resize(96);

        const unsigned char* fontData = loadTTF(fonts[font]);
        int result = stbtt_BakeFontBitmap(fontData, 0, fontSize, fontAtlases[font].data(), atlasSize, atlasSize, 32, 96, fontCharDatas[font].data());
        if (result <= 0) { throw("Failed to bake font"); }
        fontSizes[font] = fontSize;
    }

    auto& grayscale = fontAtlases[font];
    auto& charData = fontCharDatas[font];
    unsigned int atlasSize = static_cast<unsigned int>(std::sqrt(grayscale.size()));

    float min_y = 0.0f;
    float max_y = 0.0f;
    float current_x = 0.0f;
    for (char c : text) {
        if (c < 32 || c > 127) continue;
        stbtt_bakedchar cd = charData[c - 32];
        min_y = min_y < cd.yoff ? min_y : cd.yoff;
        max_y = max_y > cd.yoff + (cd.y1 - cd.y0) ? max_y : cd.yoff + (cd.y1 - cd.y0);
        current_x += cd.xadvance;
    }

    int texsize = (static_cast<int>(current_x) + 1 > static_cast<int>(max_y - min_y) + 1 ? static_cast<int>(current_x) : static_cast<int>(max_y - min_y)) + 1;
    std::vector<unsigned char> text_grayscale(texsize * texsize, 0);

    current_x = 0.0f;
    for (char c : text) {
        if (c < 32 || c > 127) continue;
        stbtt_bakedchar cd = charData[c - 32];

        int dst_x = static_cast<int>(std::round(current_x + cd.xoff));
        int dst_y = static_cast<int>(std::round(cd.yoff - min_y));

        int width = cd.x1 - cd.x0;
        int height = cd.y1 - cd.y0;
        for (int py = 0; py < height; ++py) {
            for (int px = 0; px < width; ++px) {
                int src_idx = (cd.y0 + py) * atlasSize + (cd.x0 + px);
                int dst_idx = (dst_y + py) * texsize + (dst_x + px);
                if (dst_idx >= 0 && dst_idx < texsize * texsize) {
                    text_grayscale[dst_idx] = grayscale[src_idx];
                }
            }
        }
        current_x += cd.xadvance;
    }

    spriteTextures[textureIndex] = std::make_unique<Texture>(device, text_grayscale.data(), texsize, descriptorSetLayout, descriptorPool, *this);

    if (updateTextureIndex != -1) { updateTextures = true; }
    else { updateTextureIndex = textureIndex; }
#else
    unsigned int atlasSize = fontSize * 16.f;
    vector<unsigned char> grayscale(atlasSize * atlasSize);
    vector<stbtt_bakedchar> charData(96);

    int result = stbtt_BakeFontBitmap(loadTTF(fonts[font]), 0, fontSize, grayscale.data(), atlasSize, atlasSize, 32, 96, charData.data());

    float min_y = 0.0f;
    float max_y = 0.0f;
    float current_x = 0.0f;
    for (char c : text) {
        if (c < 32 || c > 127) continue;
        stbtt_bakedchar cd = charData[c - 32];
        min_y = min_y < cd.yoff ? min_y : cd.yoff;
        max_y = max_y > cd.yoff + (cd.y1 - cd.y0) ? max_y : cd.yoff + (cd.y1 - cd.y0);
        current_x += cd.xadvance;
    }

    int texsize = (static_cast<int>(current_x) + 1 > static_cast<int>(max_y - min_y) + 1 ? static_cast<int>(current_x) : static_cast<int>(max_y - min_y)) + 1;
    std::vector<unsigned char> text_grayscale(texsize * texsize, 0);

    current_x = 0.0f;
    for (char c : text) {
        if (c < 32 || c > 127) continue;
        stbtt_bakedchar cd = charData[c - 32];

        int dst_x = static_cast<int>(std::round(current_x + cd.xoff));
        int dst_y = static_cast<int>(std::round(cd.yoff - min_y));

        for (int py = 0; py < cd.y1 - cd.y0; ++py) {
            for (int px = 0; px < cd.x1 - cd.x0; ++px) {
                int src_x = cd.x0 + px;
                int src_y = cd.y0 + py;
                int src_idx = src_y * atlasSize + src_x;

                int dst_idx = (dst_y + py) * texsize + (dst_x + px);
                if (dst_idx >= 0 && dst_idx < texsize * texsize) {
                    text_grayscale[dst_idx] = grayscale[src_idx];
                }
            }
        }

        current_x += cd.xadvance;
    }

    spriteTextures[textureIndex] = std::make_unique<Texture>(device, text_grayscale.data(), texsize, descriptorSetLayout, descriptorPool, *this);

    if (updateTextureIndex != -1) { updateTextures = true; }
    else { updateTextureIndex = textureIndex; }
#endif
}

void Pipeline::loadSprites() {
    spriteTextures.reserve(MAX_TEXTURES);
#ifdef _WIN32
    lastTexts.resize(MAX_TEXTURES, "");
    fontAtlases.resize(fonts.size());
    fontCharDatas.resize(fonts.size());
    fontSizes.resize(fonts.size(), 0.0f);
#endif
    squareModel = makeModel({
        -.5f, -.5f, // Bottom-Left
        .5f, -.5f,  // Bottom-Right
        -.5f, .5f,  // Top-Right
        .5f, .5f    // Top-Left
    });

    while (texturePaths.size() < MAX_TEXTURES) { texturePaths.push_back("e.jpg"); }
    for (unsigned char i = 0; i < texturePaths.size(); i++) { spriteTextures.push_back(std::make_unique<Texture>(device, texturePaths[i], descriptorSetLayout, descriptorPool, *this)); }
}

void Pipeline::loadFlappyBird() {
    texturePaths = { "flappyBird.png", "pipe.png" };
    fonts = { "assets/fonts/Bullpen3D.ttf" };
    loadSprites();

    createSprite(squareModel, 0, -.7f, -.2f, .1f, .1f, 0.f, 1.f, 1.f, 1.f, 1.f);

    for (float i = 1.f; i < 5.f; i += 1.f) {
        float y = Random(.4f, 1.4f);
        createSprite(squareModel, 1, i, y, .15f, 1.5f, 0.f, 1.f, 1.f, 1.f, 1.f);
        createSprite(squareModel, 1, i, y - 2.f, .15f, 1.5f, 180.f, 1.f, 1.f, 1.f, 1.f);
    }
}

void Pipeline::loadSlimeAttack() {
    texturePaths = { "flappyBird.png", "pipe.png" };
    fonts = {};
    loadSprites();

    createSprite(squareModel, 1, 0.f, 0.f, .15f, .15f, 0.f, 1.f, 1.f, 1.f, 1.f);
    createSprite(squareModel, 1, 0.f, .7f, 2.f, .15f, 0.f, 1.f, 1.f, 1.f, 1.f);
}

void Pipeline::loadTerminalCalculator() {
    texturePaths = { "terminalCalculator.png" };
    fonts = {};
    loadSprites();

    createSprite(squareModel, 0, 0.f, 0.f, 2.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f);
}