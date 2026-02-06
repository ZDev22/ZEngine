/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

#define ZENGINE_IMPLEMENTATION - define functions INCLUDE IN MAIN.CPP ONLY
#define ZENGINE_DEPS_DEFINED - if your using zdeps.c define this
#define ZENGINE_DISABLE_VSYNC - extend beyond mortal limitations and exceed maximum fps
#define ZENGINE_DEBUG 2 - adds debug printing, the higher the number the more debug info
#define ZENGINE_MAX_FRAMES_IN_FLIGHT 2 - max amount of frames being processed at once
#define ZENGINE_MAX_SPRITES 100000 - the maximum amount of sprite the engine can load at once (more sprites, more memory usage)
#define ZENGINE_MAX_TEXTURES 50 - the maximum amount of texture the engine can load at once

CREATE A SPRITE:  createSprite(model, textureIndex, x, y, scalex, scaley, rotation);
CREATE A TEXTURE: sprites[0].setTexture(std::make_unique<Texture>("texture.png")); - automatically assumes assets/images/texture.png
CREATE A MODEL:   std::shared_ptr<Model> model = make_shared<Model>(vector_of_verticy_positions); - see ZEngineInit for a real-world example
*/

#ifndef ZENGINE_HPP
#define ZENGINE_HPP

/* define a few necissary macros if not already defined */
#ifndef ZENGINE_MAX_FRAMES_IN_FLIGHT
    #define ZENGINE_MAX_FRAMES_IN_FLIGHT 2
#endif
#ifndef ZENGINE_MAX_SPRITES
    #define ZENGINE_MAX_SPRITES 100000
#endif
#ifndef ZENGINE_MAX_TEXTURES
    #define ZENGINE_MAX_TEXTURES 50
#endif

/* debugging printing */
#define ZENGINE_PRINT1(x)
#define ZENGINE_PRINT2(x)
#define ZENGINE_PRINT3(x)
#define ZENGINE_THROW1(x) (x)
#define ZENGINE_THROW2(x) (x)
#define ZENGINE_THROW3(x) (x)

#ifdef ZENGINE_DEBUG
    #include <iostream>
    #if ZENGINE_DEBUG > 0
        #define ZENGINE_PRINT1(...) std::cout << __VA_ARGS__
        #define ZENGINE_THROW1(x) if((x) != VK_SUCCESS) throw;
    #endif
    #if ZENGINE_DEBUG > 1
        #define ZENGINE_PRINT2(...) std::cout << __VA_ARGS__
        #define ZENGINE_THROW2(x) if((x) != VK_SUCCESS) throw;
    #endif
    #if ZENGINE_DEBUG > 2
        #define ZENGINE_PRINT3(...) std::cout << __VA_ARGS__
        #define ZENGINE_THROW3(x) if((x) != VK_SUCCESS) throw;
    #endif
#endif

#if defined(ZENGINE_IMPLEMENTATION) && !defined(ZENGINE_DEPS_DEFINED)
    #define MINIAUDIO_IMPLEMENTATION

    #define RGFW_IMPLEMENTATION
    #define RGFW_ASSERT(x) (void)(x)

    #define STB_IMAGE_IMPLEMENTATION
    #define STBI_ASSERT
#endif
#define RGFW_VULKAN
#define RGFW_EXPORT

/* dependencies */
#include "deps/RGFW.h" /* window */
#include "deps/miniaudio.h" /* audio */
#include "deps/stb_image.h" /* image */

/* undefine these so they don't get used later */
#undef MINIAUDIO_IMPLEMENTATION
#undef RGFW_IMPLEMENTATION
#undef RGFW_EXPORT
#undef RGFW_ASSERT
#undef STB_IMAGE_IMPLEMENTATION
#undef STBI_ASSERT

/* vulkan */
#if defined(__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
#elif defined(__APPLE__)
    #define VK_USE_PLATFORM_MACOS_MVK
#elif defined(_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

/* std libs (the less the better) */
#include <string> // c-strings are better
#include <memory>
#include <vector>
#include <fstream>
#include <filesystem>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* forward declaration of a bunch of structs lol */
struct SpriteData;
struct Sprite;
struct Buffer;
struct Model;
struct Texture;
struct SwapChain;
struct ma_engine;

/* declare a few structs before creating variables about them */
struct Vertex {
    float position[2] = {0};
    float texCoord[2] = {0};

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static VkVertexInputAttributeDescription* getAttributeDescriptions() {
        VkVertexInputAttributeDescription* attributeDescriptions = new VkVertexInputAttributeDescription[2];
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
        return attributeDescriptions;
    }
};

struct Camera {
    float position[2];
    float zoom[2];
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    unsigned int graphicsFamily;
    unsigned int presentFamily;
    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    inline bool isComplete() const { return graphicsFamilyHasValue && presentFamilyHasValue; }
};

/* extern vars */
extern float deltaTime;
extern bool ZEngineClose;
extern std::unique_ptr<Texture> spriteTextures[ZENGINE_MAX_TEXTURES];
extern std::shared_ptr<Model> squareModel;
extern VkDevice device_;
extern ma_engine audio;

#ifdef ZENGINE_IMPLEMENTATION

/* kinda just chillin ngl */
Camera camera;
ma_engine audio;

float deltaTime = 0.f; /* deltaTime, do what you will. Example implementation in main.cpp */
bool ZEngineClose = false; /* flag to show when the engine is closing */

/* sprite vecs */
std::vector<SpriteData> sprites;
std::vector<Sprite> spriteCPU;
unsigned int spriteID = 0;

/* texture vecs */
std::unique_ptr<Texture> spriteTextures[ZENGINE_MAX_TEXTURES];

/* window vars */
bool framebufferResized = false;
RGFW_window* windowdata;
VkExtent2D windowExtent;

/* device vars */
VkInstance instance;
VkCommandPool commandPool;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkPhysicalDeviceProperties properties;
VkDevice device_;
VkQueue presentQueue_;
VkSurfaceKHR surface_;
VkQueue graphicsQueue_;

/* renderer vars */
std::vector<VkCommandBuffer> commandBuffers;
unsigned int currentImageIndex;

/* pipeline vars */
VkPipeline graphicsPipeline;
VkPipelineLayout pipelineLayout;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;
std::shared_ptr<Model> squareModel;

/* rendersystem vars */
VkDescriptorSet spriteDataDescriptorSet;
std::unique_ptr<Buffer> spriteDataBuffer;

/* swapchain vars */
std::unique_ptr<SwapChain> swapChain;
VkSwapchainKHR oldSwapChain;

#endif // ZENGINE_IMPLEMENTATION (variables)

/* ZENGINE FORWARD-DECLARED FUNCTIONS */
void ZEngineInit();
void ZEngineRender();
void ZEngineDeinit();

void createSprite(std::shared_ptr<Model> model, unsigned int textureIndex, float positionx, float positiony, float scalex, float scaley, float rotation);
void updateTexture(unsigned char index);
inline const Vertex* getVertices(const std::shared_ptr<Model>& model);
inline unsigned int getVerticySize(const std::shared_ptr<Model>& model);

void createCommandBuffers();
VkCommandBuffer beginSingleTimeCommands();
VkShaderModule createShaderModule(const char* filepath);
void endSingleTimeCommands(VkCommandBuffer commandBuffer);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);
void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layerCount);
VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

/* ZENGINE STRUCTS */
struct alignas(16) SpriteData {
    float position[2];
    float scale[2];
    float rotationMatrix[4];

    unsigned int textureIndex;
    unsigned int ID;
    float rotation;

    constexpr void setRotationMatrix() {
        rotationMatrix[0] = cos(rotation * .01745329f);
        rotationMatrix[2] = sin(rotation * .01745329f);
        rotationMatrix[1] = -rotationMatrix[2];
        rotationMatrix[3] = rotationMatrix[0];
    }
    inline void setTexture(std::unique_ptr<Texture> texture) {
        spriteTextures[textureIndex] = std::move(texture);
        updateTexture(textureIndex);
    }
};

struct Sprite {
    std::shared_ptr<Model> model;
    Texture* texture;
    bool visible;
};

extern std::vector<SpriteData> sprites;
extern std::vector<Sprite> spriteCPU;

struct SwapChain {
public:
    SwapChain() {
        /* create swapchain KHR */
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat; {
            surfaceFormat = swapChainSupport.formats[0];
            for (unsigned int i = 0; i < swapChainSupport.formats.size(); i++) {
                if (swapChainSupport.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swapChainSupport.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    surfaceFormat = swapChainSupport.formats[i];
                    break;
                }
            }
        }

        windowExtent = swapChainSupport.capabilities.currentExtent;
        unsigned int imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) { imageCount = swapChainSupport.capabilities.maxImageCount; }

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface_;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = windowExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        if (indices.graphicsFamily != indices.presentFamily) {
            const unsigned int queueFamilyIndices[2] = { indices.graphicsFamily, indices.presentFamily };
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.clipped = VK_TRUE;
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.oldSwapchain = oldSwapChain == nullptr ? VK_NULL_HANDLE : oldSwapChain;
        #ifdef ZENGINE_DISABLE_VSYNC
            createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
        #else
            createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
        #endif

        ZENGINE_THROW3(vkCreateSwapchainKHR(device_, &createInfo, nullptr, &swapChain));
        ZENGINE_THROW3(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, nullptr));
        swapChainImages.resize(imageCount);
        ZENGINE_THROW3(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, swapChainImages.data()));
        swapChainImageFormat = surfaceFormat.format;

        /* create image views*/
        swapChainImageViews.resize(swapChainImages.size());

        VkImageViewCreateInfo viewInfo{};
        for (unsigned int i = 0; i < swapChainImages.size(); i++) {
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = swapChainImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapChainImageFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            vkCreateImageView(device_, &viewInfo, nullptr, &swapChainImageViews[i]);
        }

        /* create renderpass */
        swapChainDepthFormat = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = swapChainDepthFormat;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.dstSubpass = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcAccessMask = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

        VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        ZENGINE_THROW2(vkCreateRenderPass(device_, &renderPassInfo, nullptr, &renderPass));

        /* create depth resources */
        depthImages.resize(swapChainImages.size());
        depthImageMemorys.resize(swapChainImages.size());
        depthImageViews.resize(swapChainImages.size());

        for (unsigned int i = 0; i < depthImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = windowExtent.width;
            imageInfo.extent.height = windowExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = swapChainDepthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;
            createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = depthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = swapChainDepthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;
            vkCreateImageView(device_, &viewInfo, nullptr, &depthImageViews[i]);
        }

        /* create frame buffers*/
        swapChainFramebuffers.resize(swapChainImages.size());

        for (unsigned int i = 0; i < swapChainImages.size(); i++) {
            const VkImageView attachments[2] = { swapChainImageViews[i], depthImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 2;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = windowExtent.width;
            framebufferInfo.height = windowExtent.height;
            framebufferInfo.layers = 1;
            vkCreateFramebuffer(device_, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
        }

        /* create semaphores */
        imageAvailableSemaphores.resize(ZENGINE_MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(ZENGINE_MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(ZENGINE_MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (unsigned int i = 0; i < ZENGINE_MAX_FRAMES_IN_FLIGHT; i++) {
            vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
            vkCreateSemaphore(device_, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
            vkCreateFence(device_, &fenceInfo, nullptr, &inFlightFences[i]);
        }
    }
    ~SwapChain() {
        ZENGINE_PRINT3(" - Destroying framebuffers\n"); for (VkFramebuffer framebuffer : swapChainFramebuffers) { vkDestroyFramebuffer(device_, framebuffer, nullptr); }
        ZENGINE_PRINT3(" - Destroying depth data\n");

        for (unsigned char i = 0; i < depthImages.size(); i++) {
            vkDestroyImageView(device_, depthImageViews[i], nullptr);
            vkDestroyImage(device_, depthImages[i], nullptr);
            vkFreeMemory(device_, depthImageMemorys[i], nullptr);
        }

        ZENGINE_PRINT3(" - Destroying image views\n"); for (VkImageView imageView : swapChainImageViews) { vkDestroyImageView(device_, imageView, nullptr); }
        ZENGINE_PRINT3(" - Destroying render pass\n"); vkDestroyRenderPass(device_, renderPass, nullptr);
        if (!ZEngineClose) { ZENGINE_PRINT3(" - Destroying swapchain KHR\n"); vkDestroySwapchainKHR(device_, swapChain, nullptr); }
        ZENGINE_PRINT3(" - Destroying semaphores\n");
        for (unsigned char i = 0; i < ZENGINE_MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device_, imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(device_, renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device_, inFlightFences[i], nullptr);
        }
    }

    inline VkResult acquireNextImage(unsigned int* imageIndex) { return vkAcquireNextImageKHR(device_, swapChain, 18446744073709551615ULL, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, imageIndex); }
    
    VkResult submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex) {
        if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) { vkWaitForFences(device_, 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX); }
        imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

        const VkSemaphore waitSemaphores[1] = { imageAvailableSemaphores[currentFrame] };
        const VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        const VkSemaphore signalSemaphores[1] = { renderFinishedSemaphores[currentFrame] };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(device_, 1, &inFlightFences[currentFrame]);
        vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFences[currentFrame]);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = imageIndex;

        currentFrame = (currentFrame + 1) % ZENGINE_MAX_FRAMES_IN_FLIGHT;
        return vkQueuePresentKHR(presentQueue_, &presentInfo);
    }
 
    inline VkFramebuffer getFrameBuffer(unsigned long index) const { return swapChainFramebuffers[index]; }
    inline VkImageView getImageView(int index) const { return swapChainImageViews[index]; }
    inline VkRenderPass getRenderPass() const { return renderPass; }
    inline unsigned int getSwapChainImageSize() const { return swapChainImages.size(); }
    inline VkSwapchainKHR getSwapChain() const { return swapChain; }

private:
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
    VkFormat swapChainDepthFormat;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkImage> depthImages;
    std::vector<VkDeviceMemory> depthImageMemorys;
    std::vector<VkImageView> depthImageViews;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    unsigned int currentFrame = 0;
};

struct Texture {
public:
    void createTextureSampler() {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        vkCreateSampler(device_, &samplerInfo, nullptr, &sampler);
    }

    Texture(const std::string& filepath) : imageLayout(VK_IMAGE_LAYOUT_UNDEFINED), image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE), sampler(VK_NULL_HANDLE) {
        stbi_uc* pixels = stbi_load(("assets/images/" + filepath).c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device_, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, imageSize);
        vkUnmapMemory(device_, stagingBufferMemory);
        stbi_image_free(pixels);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = texWidth;
        imageInfo.extent.height = texHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);
        transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, image, texWidth, texHeight, 1);
        transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(device_, stagingBuffer, nullptr);
        vkFreeMemory(device_, stagingBufferMemory, nullptr);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        vkCreateImageView(device_, &viewInfo, nullptr, &imageView);
        createTextureSampler();
    }

    Texture(const unsigned char* pixelData, const unsigned int size) : imageLayout(VK_IMAGE_LAYOUT_UNDEFINED), image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE), sampler(VK_NULL_HANDLE), texChannels(1) {
        texWidth = size;
        texHeight = size;
        VkDeviceSize imageSize = size * size * 4;

        unsigned char* rgbaPixels = (unsigned char*)malloc(imageSize);
        memset(rgbaPixels, 0xFF, imageSize);
        for (unsigned int i = 0; i < size * size; ++i) { rgbaPixels[i * 4 + 3] = pixelData[i]; }

        createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device_, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, rgbaPixels, imageSize);
        vkUnmapMemory(device_, stagingBufferMemory);

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = size;
        imageInfo.extent.height = size;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        createImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory);
        transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(stagingBuffer, image, size, size, 1);
        transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(device_, stagingBuffer, nullptr);
        vkFreeMemory(device_, stagingBufferMemory, nullptr);

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        free(rgbaPixels);
        vkCreateImageView(device_, &viewInfo, nullptr, &imageView);
        createTextureSampler();
    }

    ~Texture() {
        if (!ZEngineClose) { vkDeviceWaitIdle(device_); }
        vkDestroySampler(device_, sampler, nullptr);
        vkDestroyImageView(device_, imageView, nullptr);
        vkDestroyImage(device_, image, nullptr);
        vkFreeMemory(device_, imageMemory, nullptr);
    }

    void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        endSingleTimeCommands(commandBuffer);
        imageLayout = newLayout;
    }

    inline VkImageView getImageView() const { return imageView; }
    inline VkSampler getSampler() const { return sampler; }

private:
    VkImageLayout imageLayout;
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;
    VkSampler sampler;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    int texWidth, texHeight, texChannels;
};

struct Buffer {
public:
    Buffer(VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags) : bufferSize(instanceSize * instanceCount) {

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

        vkAllocateMemory(device_, &allocInfo, nullptr, &memory);
        vkBindBufferMemory(device_, buffer, memory, 0);
    }
    ~Buffer() {
        vkDestroyBuffer(device_, buffer, nullptr);
        vkFreeMemory(device_, memory, nullptr);
        buffer = VK_NULL_HANDLE;
        memory = VK_NULL_HANDLE;
    }

    void map() {
        void* temp = nullptr;
        vkMapMemory(device_, memory, 0, bufferSize, 0, &temp);
        mapped = (char*)temp;
    }
    void unmap() {
        if (mapped) {
            vkUnmapMemory(device_, memory);
            mapped = nullptr;
        }
    }

    inline void writeToBuffer(const void* data, unsigned int size) { memcpy(mapped, data, size); }
    inline VkBuffer getBuffer() const { return buffer; }

private:
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize;
    void* mapped = nullptr;
};

struct Model {
public:
    Model(float* positions, const unsigned int verticySize) : verticySize(verticySize) { /* verticies are X and Y positions, verticy size is the size of the vector / 2 */
        vertices = new Vertex[verticySize];

        unsigned int index = 0;
        for (unsigned int i = 0; i < verticySize; ++i) {
            Vertex v{};
            index = i << 1; /* << 1 is * 2 lol*/
            v.position[0] = positions[index];
            v.position[1] = positions[index + 1];
            v.texCoord[0] = positions[index] + 0.5f;
            v.texCoord[1] = positions[index + 1] + 0.5f;
            vertices[i] = v;
        }

        vertexBuffer = std::make_unique<Buffer>(sizeof(Vertex) * verticySize, 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vertexBuffer->map();
        vertexBuffer->writeToBuffer((const void*)vertices, (unsigned int)(sizeof(Vertex) * verticySize));
        vertexBuffer->unmap();
    }
    ~Model() { delete[] vertices; }

    inline void bind(VkCommandBuffer commandBuffer) {
        static VkBuffer buffers[] = { vertexBuffer->getBuffer() };
        static VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    /* this fixes copying memory leak ig */
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = default;
    Model& operator=(Model&&) = default;

    inline void draw(VkCommandBuffer commandBuffer, unsigned int instanceCount, unsigned int firstInstance) { vkCmdDraw(commandBuffer, (unsigned int)verticySize, instanceCount, 0, firstInstance); }
    inline const Vertex* getVertices() const { return vertices; }
    inline unsigned int size() const { return verticySize; }

private:
    std::unique_ptr<Buffer> vertexBuffer;
    const unsigned int verticySize;
    Vertex* vertices;
};

#ifdef ZENGINE_IMPLEMENTATION

/* ZENIGNE HELPER FUNCTIONS */
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (unsigned int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = true;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
        if (queueFamilies[i].queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = true;
        }
        if (indices.isComplete()) { break; }
        i++;
    }
    return indices;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    unsigned int formatCount = 0;
    unsigned int presentModeCount = 0;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
    }
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
    }
    return details;
}

VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    VkFormatProperties props;
    for (VkFormat format : candidates) {
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) { return format; }
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) { return format; }
    }
    throw("failed to find supported format");
}

unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (unsigned int i = 0; i < memProperties.memoryTypeCount; i++) { if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) { return i; }}
    throw("Failed to find suitable memory type");
}

VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);
    vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory);
    vkBindBufferMemory(device_, buffer, bufferMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommands(commandBuffer);
}

void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layerCount) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    endSingleTimeCommands(commandBuffer);
}

void createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    vkCreateImage(device_, &imageInfo, nullptr, &image);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device_, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory);
    vkBindImageMemory(device_, image, imageMemory, 0);
}

/* ZENGINE HELPER FUNCTIONS */
inline const Vertex* getVertices(const std::shared_ptr<Model>& model) { return model->getVertices(); }
inline unsigned int getVerticySize(const std::shared_ptr<Model>& model) { return model->size(); }

void updateTexture(unsigned char index) {
    Texture* texture = spriteTextures[index].get();
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = texture->getImageView();
    imageInfo.sampler = texture->getSampler();

    VkWriteDescriptorSet imageWrite{};
    imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWrite.dstSet = spriteDataDescriptorSet;
    imageWrite.dstBinding = 1;
    imageWrite.dstArrayElement = index;
    imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageWrite.descriptorCount = 1;
    imageWrite.pImageInfo = &imageInfo;
    vkUpdateDescriptorSets(device_, 1, &imageWrite, 0, nullptr);
}

void createCommandBuffers() {
    commandBuffers.resize(swapChain->getSwapChainImageSize());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = (unsigned int)commandBuffers.size();
    vkAllocateCommandBuffers(device_, &allocInfo, commandBuffers.data());
}

VkShaderModule createShaderModule(const char* filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (!file.is_open()) { throw ("Failed to open shader!"); }

    size_t fileSize = file.tellg();
    file.seekg(0);

    unsigned int buffer[fileSize / 4];
    file.read(reinterpret_cast<char*>(buffer), fileSize);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = buffer;

    VkShaderModule shaderModule;
    ZENGINE_THROW1(vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule));

    return shaderModule;
}

void createSprite(std::shared_ptr<Model> model, unsigned int textureIndex, float positionx, float positiony, float scalex, float scaley, float rotation) {
    if (sprites.size() >= ZENGINE_MAX_SPRITES) { return; }
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
    spriteData.textureIndex = textureIndex;
    spriteData.ID = spriteID++;

    sprites.emplace_back(spriteData);
    spriteCPU.emplace_back(sprite);
}

/* ZENGINE */
void ZEngineInit() { /* YOU MUST CREATE THE RGFW WINDOW BEFORE INITING ZENGINE */
#ifdef ZENGINE_DEBUG
    std::ios::sync_with_stdio(false);
#endif
    ZENGINE_PRINT1("Compiling shaders\n"); //---------------------------------------------------------------------------------------------------------------
    const char* shaders[4] = { ".vert", ".frag", ".comp", ".geom" };
    const char* stages[4]     = { "vert",  "frag",  "comp",  "geom" };

    for (const auto& file : std::filesystem::directory_iterator("shaders")) {
        if (!file.is_regular_file()) { continue; }
        for (int i = 0; i < 4; ++i) {
            if (file.path().extension().string() == shaders[i]) {
                std::string inputFile = file.path().string().c_str();
                std::string outputFile = (file.path().string() + ".spv").c_str();

                if (!(std::filesystem::exists(outputFile)  && std::filesystem::last_write_time(outputFile) >= std::filesystem::last_write_time(inputFile))) {
                    int result = std::system(("glslc -fshader-stage=" + std::string(stages[i]) + " " + inputFile + " -o " + outputFile).c_str());
                    if (result != 0) { ZENGINE_PRINT1("Failed to compile " << inputFile << " (error: " << result << ")\n"); }
                }
                break;
            }
        }
    }

    ZENGINE_PRINT1("Creating instance...\n"); //---------------------------------------------------------------------------------------------------------------
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ZDev";
    appInfo.pEngineName = "ZEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 11, 0);
#ifdef __APPLE__
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 3, 0);
#else
    appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);
#endif

    size_t rgfWExtensionCount = 0;
    const char** rgfWExtensions = RGFW_getRequiredInstanceExtensions_Vulkan(&rgfWExtensionCount);
    if (!rgfWExtensions || rgfWExtensionCount == 0) { throw("Failed to get Vulkan extensions"); }

    std::vector<const char*> extensions(rgfWExtensions, rgfWExtensions + rgfWExtensionCount);
    #ifdef __APPLE__
        extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif

    ZENGINE_PRINT1("Enabling extensions:\n");
    for (unsigned int i = 0; i < extensions.size(); i++) { ZENGINE_PRINT2("     - " << extensions[i] << std::endl); }

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    #ifdef __APPLE__
        instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = (unsigned int)extensions.size();
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    #if ZENGINE_DEBUG > 1
        VkResult result = vkCreateInstance(&instanceInfo, nullptr, &instance);
        ZENGINE_PRINT2("Instance: " << result << std::endl; );
    #else
        vkCreateInstance(&instanceInfo, nullptr, &instance);
    #endif

    ZENGINE_PRINT1("Creating surface...\n"); RGFW_window_createSurface_Vulkan(windowdata, instance, &surface_); //---------------------------------------------------------------------------------------------------------------

    ZENGINE_PRINT1("Creating physical device...\n"); //---------------------------------------------------------------------------------------------------------------
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) { throw("No Vulkan-compatible GPUs found"); }
    else if (deviceCount == 1) { ZENGINE_PRINT2("Found 1 GPU\n"); }
    else { ZENGINE_PRINT2("Found " << deviceCount << " GPUs\n"); }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    short highScore = 0;
    for (unsigned int i = 0; i < deviceCount; ++i) {
        /* check if device is suitable, and score it */
        short newScore = 0;
        QueueFamilyIndices indices = findQueueFamilies(devices[i]);
        unsigned int extensionCount = 0;
        bool swapChainAdequate = false;

        /* check if extensions are supported on the GPU: */
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
        VkExtensionProperties availableExtensions[extensionCount];
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, availableExtensions);

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(devices[i]);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        ZENGINE_PRINT1("SwapChain formats: " << swapChainSupport.formats.size() << ", present modes: " << swapChainSupport.presentModes.size() << std::endl);
        for (unsigned char i = 0; i < swapChainSupport.presentModes.size(); i++) {
            if (swapChainSupport.presentModes[i] == 0) { ZENGINE_PRINT2("    - GPU supports VSync\n"); }
            else if (swapChainSupport.presentModes[i] == 2) { ZENGINE_PRINT2("    - GPU can disable VSync\n"); }
        }
        newScore += (swapChainSupport.formats.size() * swapChainSupport.presentModes.size());

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(devices[i], &supportedFeatures);
        if (!(indices.isComplete() && swapChainAdequate)) { newScore = 0; }
        if (newScore > highScore) {
            highScore = newScore;
            physicalDevice = devices[i];
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) { physicalDevice = devices[0]; ZENGINE_PRINT1("Selected GPU is unsupported! Expect bugs!\n"); }
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    ZENGINE_PRINT1("Selected GPU: " << properties.deviceName << std::endl);

    ZENGINE_PRINT1("Creating logical device...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;

    std::vector<unsigned int> uniqueQueueFamilies;
    uniqueQueueFamilies.emplace_back(indices.graphicsFamily);
    if (indices.presentFamily != indices.graphicsFamily) { uniqueQueueFamilies.emplace_back(indices.presentFamily); }

    for (unsigned int queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    #ifdef __APPLE__
        VkPhysicalDeviceVulkan12Features VKFeatures{};
        VKFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        VKFeatures.bufferDeviceAddress = VK_FALSE;
        VKFeatures.bufferDeviceAddressCaptureReplay = VK_FALSE;
        VKFeatures.bufferDeviceAddressMultiDevice = VK_FALSE;
    #endif

    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceInfo.queueCreateInfoCount = (unsigned int)queueCreateInfos.size();
    #ifdef __APPLE__
        deviceInfo.pNext = &VKFeatures;
    #endif

    const char* extension[1] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = extension;
    deviceInfo.enabledLayerCount = 0;

    ZENGINE_THROW1(vkCreateDevice(physicalDevice, &deviceInfo, nullptr, &device_));

    vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
    vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);

    ZENGINE_PRINT1("Creating command pool...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
    VkCommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    ZENGINE_THROW1(vkCreateCommandPool(device_, &commandPoolInfo, nullptr, &commandPool));

    ZENGINE_PRINT1("Getting project ready...\n"); //---------------------------------------------------------------------------------------------------------------
    swapChain = std::make_unique<SwapChain>();
    createCommandBuffers();

    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = createShaderModule("shaders/texture.vert.spv");
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = createShaderModule("shaders/texture.frag.spv");
    shaderStages[1].pName = "main";

    VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = Vertex::getAttributeDescriptions(); 

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
    layoutBindings[1].descriptorCount = ZENGINE_MAX_TEXTURES;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = layoutBindings;
    ZENGINE_THROW1(vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr, &descriptorSetLayout));

    VkDescriptorPoolSize poolSizes[] = {{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ZENGINE_MAX_TEXTURES * (ZENGINE_MAX_TEXTURES + 1)}};

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = 2;
    descriptorPoolInfo.pPoolSizes = poolSizes;
    descriptorPoolInfo.maxSets = ZENGINE_MAX_TEXTURES + 1;
    ZENGINE_THROW2(vkCreateDescriptorPool(device_, &descriptorPoolInfo, nullptr, &descriptorPool));

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    ZENGINE_THROW1(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout));

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
    pipelineInfo.renderPass = swapChain->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    ZENGINE_THROW2(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline));

    vkDestroyShaderModule(device_, shaderStages[0].module, nullptr);
    vkDestroyShaderModule(device_, shaderStages[1].module, nullptr);

    /* create the pipeline layout */
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Camera);

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    ZENGINE_THROW2(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelineLayout));

    /* create texture array descriptor set */
    ZENGINE_PRINT2("Initing sprites...\n");

    VkDescriptorImageInfo imageInfos[ZENGINE_MAX_TEXTURES];
    std::unique_ptr<Texture> texture = std::make_unique<Texture>("e.jpg");
    VkImageView textureImage = texture->getImageView();
    VkSampler textureSampler = texture->getSampler();
    for (unsigned int i = 0; i < ZENGINE_MAX_TEXTURES; i++) {
        spriteTextures[i] = std::move(texture);
        VkDescriptorImageInfo& imageInfo = imageInfos[i];
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImage;;
        imageInfo.sampler = textureSampler;
    }

    /* init sprites */
    camera.zoom[0]     = 1.f; camera.zoom[1]     = 1.f;
    camera.position[0] = 0.f; camera.position[1] = 0.f;
    float* positions = new float[8] {
        -.5f, -.5f, // Bottom-Left
        .5f, -.5f,  // Bottom-Right
        -.5f, .5f,  // Top-Right
        .5f, .5f    // Top-Left
    };
    squareModel = std::make_shared<Model>(positions, 8);
    createSprite(squareModel, 0, 0.f, 0.f, .1f, .1f, 0.f);
    delete[] positions;

    VkDeviceSize bufferSize = sizeof(SpriteData) * ZENGINE_MAX_SPRITES;
    spriteDataBuffer = std::make_unique<Buffer>(bufferSize, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    spriteDataBuffer->map();
    spriteDataBuffer->writeToBuffer(sprites.data(), sizeof(SpriteData) * sprites.size());

    ma_engine_init(nullptr, &audio); /* init audio */

    /* allocate info */
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    ZENGINE_THROW3(vkAllocateDescriptorSets(device_, &allocInfo, &spriteDataDescriptorSet));

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = spriteDataBuffer->getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(SpriteData) * ZENGINE_MAX_SPRITES;

    ZENGINE_PRINT2("Writing sprite descriptor sets\n");
    VkWriteDescriptorSet bufferWrite{};
    bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    bufferWrite.dstSet = spriteDataDescriptorSet;
    bufferWrite.dstBinding = 0;
    bufferWrite.dstArrayElement = 0;
    bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bufferWrite.descriptorCount = 1;
    bufferWrite.pBufferInfo = &bufferInfo;

    VkWriteDescriptorSet imageWrite{};
    imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWrite.dstSet = spriteDataDescriptorSet;
    imageWrite.dstBinding = 1;
    imageWrite.dstArrayElement = 0;
    imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageWrite.descriptorCount = ZENGINE_MAX_TEXTURES;
    imageWrite.pImageInfo = imageInfos;

    VkWriteDescriptorSet descriptorWrites[2] = { bufferWrite, imageWrite };
    vkUpdateDescriptorSets(device_, 2, descriptorWrites, 0, nullptr);
}

void ZEngineRender() {
    spriteDataBuffer->writeToBuffer(sprites.data(), sizeof(SpriteData) * sprites.size()); /* update sprite position, rotation & other */

    /* resize window */
    if (swapChain->acquireNextImage(&currentImageIndex) == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
        /* recreate swapchain */
        vkDeviceWaitIdle(device_);
        oldSwapChain = swapChain->getSwapChain();
        swapChain = std::make_unique<SwapChain>();
        if (swapChain->getSwapChainImageSize() != commandBuffers.size()) {
            vkFreeCommandBuffers(device_, commandPool, (unsigned int)commandBuffers.size(), commandBuffers.data());
            commandBuffers.clear();
            createCommandBuffers();
        }

        framebufferResized = false;
        return;
    }

    /* create command buffer */
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBuffer commandBuffer = commandBuffers[currentImageIndex];
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    /* begine render pass */
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(currentImageIndex);
    renderPassInfo.renderArea.extent = windowExtent;

    VkClearValue clearValues[2] = {};
    clearValues[0].color = VkClearColorValue{0.f, 0.f, 0.f, 0.f};
    clearValues[1].depthStencil = VkClearDepthStencilValue{ 1.f, 0 };
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkRect2D scissor{{0, 0}, windowExtent};
    VkViewport viewport{0.f, 0.f, (float)windowExtent.width, (float)windowExtent.height, 0.f, 1.f};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    /* render sprites */
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &spriteDataDescriptorSet, 0, nullptr);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Camera), &camera);

    std::shared_ptr<Model> lastModel = squareModel;
    unsigned int instance = 0;
    unsigned int instanceCount = 0;

    for (unsigned int i = 0; i < spriteCPU.size(); ++i) {
        if (spriteCPU[i].visible) {
            sprites[i].setRotationMatrix();
            if (spriteCPU[i].model == lastModel) { instanceCount++; } /* model is the same, add it to the instance */
            else { /* model changed, draw the batch and count again */
                lastModel = spriteCPU[i].model;
                if (instanceCount > 0) {
                    lastModel->bind(commandBuffer);
                    lastModel->draw(commandBuffer, instanceCount, instance);
                }
                instance = i;
                instanceCount = 1;
            }
        }
        else { /* this is the best way to not draw invisible sprites */
            lastModel->bind(commandBuffer);
            lastModel->draw(commandBuffer, instanceCount, instance);
            instanceCount = 0;
        }
    }
    if (instanceCount > 0) { /* draw the last few sprites not accounted for by visibility or model switching */
        lastModel->bind(commandBuffer);
        lastModel->draw(commandBuffer, instanceCount, instance);
    }

    /* end frame */
    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);
    swapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
}

void ZEngineDeinit() {
    ZEngineClose = true;
    ZENGINE_PRINT3("Waiting for GPU...\n"); vkDeviceWaitIdle(device_);

    ZENGINE_PRINT2("Freeing command buffers\n"); vkFreeCommandBuffers(device_, commandPool, (unsigned int)commandBuffers.size(), commandBuffers.data());
    commandBuffers.clear();

    ZENGINE_PRINT3("Freeing graphics pipeline\n"); vkDestroyPipeline(device_, graphicsPipeline, nullptr);
    ZENGINE_PRINT3("Freeing pipeline layout\n");   vkDestroyPipelineLayout(device_, pipelineLayout, nullptr);
    ZENGINE_PRINT3("Freeing discriptor pool\n");   vkFreeDescriptorSets(device_, descriptorPool, 1, &spriteDataDescriptorSet);

    ZENGINE_PRINT2("Freeing textures\n"); for (unsigned int i = 0; i < ZENGINE_MAX_TEXTURES; i++) { spriteTextures[i].reset(); }
    ZENGINE_PRINT2("Unmaping sprite data buffer\n"); spriteDataBuffer->unmap();
    ZENGINE_PRINT3("Freeing sprite data buffer\n"); spriteDataBuffer.reset();
    ZENGINE_PRINT2("Freeing sprites\n"); sprites.clear();
    ZENGINE_PRINT3("Freeing sprite models\n"); spriteCPU.clear();
    ZENGINE_PRINT3("Freeing square model\n"); squareModel.reset();
    ZENGINE_PRINT3("Freein swapchain\n"); swapChain.reset();

    ZENGINE_PRINT3("Freeing descriptor set layout\n"); vkDestroyDescriptorSetLayout(device_, descriptorSetLayout, nullptr);
    ZENGINE_PRINT3("Freeing descriptor pool\n"); vkDestroyDescriptorPool(device_, descriptorPool, nullptr);
    ZENGINE_PRINT3("Freeing command pool\n"); vkDestroyCommandPool(device_, commandPool, nullptr);
    ZENGINE_PRINT3("Destroying device\n"); vkDestroyDevice(device_, nullptr);
    ZENGINE_PRINT3("Freeing window surface\n"); vkDestroySurfaceKHR(instance, surface_, nullptr);
    ZENGINE_PRINT1("Destroying instance\n"); vkDestroyInstance(instance, nullptr);
    ZENGINE_PRINT3("Deiniting audio"); ma_engine_uninit(&audio);
}

#endif // ZENGINE_IMPLEMENTATION
#undef ZENGINE_IMPLEMENTATION
#endif // ZENGINE_HPP
