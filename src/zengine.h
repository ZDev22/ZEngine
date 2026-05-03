/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

#define ZENGINE_IMPLEMENTATION - define functions INCLUDE IN MAIN.CPP ONLY
#define ZENGINE_DEPS_DEFINED - if your using zdeps.c define this (disables IMPLEMENTATION defines)
#define ZENGINE_DISABLE_VSYNC - extend beyond mortal limitations and exceed maximum fps
#define ZENGINE_DISABLE_AUDIO - disables audio, and dosen't include miniaudio.h or init it.
#define ZENGINE_SPRITE_MAPMODE_MANUAL - manually change the ZEngineSpriteRemap flag whenever you update sprite data
#define ZENGINE_SPRITE_MATRIXMODE_MANUAL - manually call sprites[0].setRotationMatrix() for every sprite you need
#define ZENGINE_DEFAULT_TEXTURE "bird.png" - change the default texture from "e.png" to whatever you want

#define ZENGINE_DEBUG - adds debug printing for debugging
#define ZENGINE_MAX_FRAMES_IN_FLIGHT 2 - max amount of frames being processed at once
#define ZENGINE_MAX_SPRITES 100000 - the maximum amount of sprite the engine can load at once (more sprites, more memory usage)
#define ZENGINE_MAX_TEXTURES 50 - the maximum amount of texture the engine can load at once
*/

#ifndef ZENGINE_H
#define ZENGINE_H

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

#if ZENGINE_MAX_SPRITES > 4096 && defined(__APPLE__)
    #undef ZENGINE_MAX_SPRITES
    #define ZENGINE_MAX_SPRITES 4096
#endif

#define SIZEOF_SPRITE_DATA 48 /* the bytes of the Sprite struct sent to the gpu, stays constant */

/* debugging */
#define ZENGINE_PRINT(x, ...)
#define ZENGINE_THROW(x) (x)

#ifdef ZENGINE_DEBUG
    #include <stdio.h>
    #undef ZENGINE_THROW
    #define ZENGINE_THROW(x) if((x) != VK_SUCCESS) exit(1);

    #undef ZENGINE_PRINT
    #define ZENGINE_PRINT(x, ...) printf(x, ##__VA_ARGS__)
#endif

#ifndef ZENGINE_DEFAULT_TEXTURE
    #define ZENGINE_DEFAULT_TEXTURE "e.png"
#endif

/* dependencies */
#if defined(ZENGINE_IMPLEMENTATION) && !defined(ZENGINE_DEPS_DEFINED)
    #define MINIAUDIO_IMPLEMENTATION

    #define RGFW_IMPLEMENTATION
    #define RGFW_ASSERT(x) (void)(x)

    #define STB_IMAGE_IMPLEMENTATION
    #define STBI_ASSERT
#endif
#define RGFW_VULKAN
#define RGFW_EXPORT

#include "deps/RGFW.h" /* window */
#include "deps/stb_image.h" /* image */

#ifndef ZENGINE_DISABLE_AUDIO
    #include "deps/miniaudio.h" /* audio */
    #define ZENGINE_AUDIO ma_engine audio
#else
    #define ZENGINE_AUDIO unsigned char audio /* 1 byte to prevent errors */
#endif

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
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct ma_engine;

typedef struct Vertex {
    float pos[2];
    float cord[2];
} Vertex;

typedef struct Buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize bufferSize;
    void* mapped;
} Buffer;

typedef struct Model {
    Buffer* vertexBuffer; 
    Vertex* vertices;
    unsigned int verticySize;
} Model;

typedef struct __attribute__((aligned(16))) Sprite {
    float position[2];
    float scale[2];
    float rotationMatrix[4];

    float depth;
    unsigned int textureIndex;
    float rotation;

    /* CPU-side only */
    Model* model;
    unsigned char* data;
} Sprite;

typedef struct Camera {
    float position[2];
    float zoom[2];
    float aspect;
} Camera;

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    unsigned int formatsSize;
    VkPresentModeKHR* presentModes;
    unsigned int presentModeSize;
} SwapChainSupportDetails;

typedef struct QueueFamilyIndices {
    unsigned int graphicsFamily;
    unsigned int presentFamily;
    _Bool graphicsFamilyHasValue;
    _Bool presentFamilyHasValue;
} QueueFamilyIndices;

typedef struct Texture {
    VkImageLayout layout;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    int channels;
} Texture;

/* extern vars */
extern double deltaTime;
extern struct Texture* spriteTextures;
extern struct Sprite* sprites;
extern unsigned int spritesSize;
extern struct Model* squareModel;
extern ZENGINE_AUDIO;
extern Camera camera;
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    extern _Bool ZEngineSpriteRemap;
#endif

#ifdef ZENGINE_IMPLEMENTATION

/* kinda just chillin ngl */
Camera camera;
ZENGINE_AUDIO;

double deltaTime = 0.0; /* deltaTime, do what you will. Example implementation in main.cpp */
_Bool ZEngineClose = 0; /* flag to show when the engine is closing */
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    _Bool ZEngineSpriteRemap = 1; /* flag to update sprite data buffer with ZENGINE_SPRITE_MAPMODE_MANUAL */
#endif

/* texture vecs */
struct Texture* spriteTextures = NULL;
char* spriteData;

/* window vars */
_Bool framebufferResized = 0;
RGFW_window* zwindow = NULL;
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
VkCommandBuffer* commandBuffers = NULL;
unsigned int currentImageIndex;

/* pipeline vars */
VkPipeline graphicsPipeline;
VkPipelineLayout pipelineLayout;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;
struct Model* squareModel = NULL;

/* rendersystem vars */
VkDescriptorSet spriteDataDescriptorSet;
struct Buffer* spriteDataBuffer = NULL;

/* swapchain vars */
VkSwapchainKHR swapChain;
VkSwapchainKHR oldSwapChain;
VkFormat swapChainImageFormat;
VkFormat swapChainDepthFormat;
VkRenderPass renderPass;
VkFramebuffer* swapChainFramebuffers = NULL;
VkImage* depthImages = NULL;
VkDeviceMemory* depthImageMemorys = NULL;
VkImageView* depthImageViews = NULL;
VkImage* swapChainImages = NULL;
VkImageView* swapChainImageViews = NULL;
VkSemaphore imageAvailableSemaphores[ZENGINE_MAX_FRAMES_IN_FLIGHT];
VkSemaphore renderFinishedSemaphores[ZENGINE_MAX_FRAMES_IN_FLIGHT];
VkFence inFlightFences[ZENGINE_MAX_FRAMES_IN_FLIGHT];
VkFence* imagesInFlight = NULL;
unsigned int currentFrame;
unsigned int imageCount;
unsigned int oldImageCount;

/* sprite vars */
Sprite* sprites = NULL;
unsigned int spritesSize = 0;

#endif // ZENGINE_IMPLEMENTATION (variables)

/* ZENGINE FORWARD-DECLARED FUNCTIONS */
void ZEngineInit();
void ZEngineRender();
void ZEngineDeinit();

/* sprite funcs */
void createSprite(Model* model, unsigned int textureIndex, float positionx, float positiony, float scalex, float scaley, float rotation);
Sprite* createSpritePtr();
void initSprite(Sprite* sprite);
void deleteSpritePointer(Sprite* sprite);
void deleteSprite(unsigned int sprite);
void setRotationMatrix(Sprite* sprite);

/* texture funcs*/
void updateTexture(unsigned int index, Texture* texture);

/* model funcs*/
const Vertex* getVertices(const Model* model);
unsigned int getVerticySize(const Model* model);

/* engine funcs */
void createCommandBuffers();
VkCommandBuffer beginSingleTimeCommands();
VkShaderModule createShaderModule(const char* filepath);
void endSingleTimeCommands(VkCommandBuffer commandBuffer);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
void querySwapChainSupport(SwapChainSupportDetails* details, VkPhysicalDevice device);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);
void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layerCount);
VkFormat findSupportedFormat(const VkFormat candidate1, const VkFormat candidate2, const VkFormat canditate3, VkImageTiling tiling, VkFormatFeatureFlags features);
void createImageWithInfo(const VkImageCreateInfo* imageInfo, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);
void createImageBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

/* struct funcs */
VkVertexInputBindingDescription getBindingDescription();
VkVertexInputAttributeDescription* getAttributeDescriptions();

void createModel(Model* model, float* positions, const unsigned int verticySize);
void deleteModel(Model* model);
void bindModel(Model* model, VkCommandBuffer commandBuffer);
void drawModel(Model* model, VkCommandBuffer commandBuffer, unsigned int instanceCount, unsigned int firstInstance);

void createSwapChain();
void deleteSwapChain();
VkResult acquireNextImage(unsigned int* imageIndex);    
void submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex);

void createTextureSampler(Texture* texture);
void createTexture(Texture* texture, const char* filepath);
void createTextureFromData(Texture* texture, const unsigned char* pixelData, const unsigned short width, const unsigned short height);
void deleteTexture(Texture* texture);
void transitionImageLayout(Texture* texture, VkImageLayout oldLayout, VkImageLayout newLayout);

void createBuffer(Buffer* buffer, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags);

#ifdef ZENGINE_IMPLEMENTATION
/* SWAP CHAIN FUNCTIONS */
void createSwapChain() {
    /* create swapchain KHR */
    SwapChainSupportDetails swapChainSupport;
    querySwapChainSupport(&swapChainSupport, physicalDevice);

    VkSurfaceFormatKHR surfaceFormat; {
        surfaceFormat = swapChainSupport.formats[0];
        for (unsigned int i = 0; i < swapChainSupport.formatsSize; i++) {
            if (swapChainSupport.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swapChainSupport.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = swapChainSupport.formats[i];
                break;
            }
        }
    }

    windowExtent = swapChainSupport.capabilities.currentExtent;
    imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) { imageCount = swapChainSupport.capabilities.maxImageCount; }

    VkSwapchainCreateInfoKHR createInfo = {0};
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
        createInfo.pQueueFamilyIndices = NULL;
    }

    createInfo.clipped = VK_TRUE;
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.oldSwapchain = oldSwapChain == NULL ? VK_NULL_HANDLE : oldSwapChain;
#ifdef ZENGINE_DISABLE_VSYNC
    createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
#else
    createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
#endif

    ZENGINE_THROW(vkCreateSwapchainKHR(device_, &createInfo, NULL, &swapChain));
    ZENGINE_THROW(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, NULL));
    swapChainImages = (VkImage*)malloc(imageCount * sizeof(VkImage));
    ZENGINE_THROW(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, swapChainImages));
    swapChainImageFormat = surfaceFormat.format;

    /* create image views*/
    swapChainImageViews = (VkImageView*)malloc(imageCount * sizeof(VkImageView));

    for (unsigned char i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {0};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapChainImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapChainImageFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(device_, &viewInfo, NULL, &swapChainImageViews[i]);
    }

    /* create renderpass */
    swapChainDepthFormat = findSupportedFormat(VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

    VkAttachmentDescription depthAttachment = {0};
    depthAttachment.format = swapChainDepthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {0};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {0};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {0};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {0};
    dependency.dstSubpass = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    renderPassInfo.pNext = NULL;
    ZENGINE_THROW(vkCreateRenderPass(device_, &renderPassInfo, NULL, &renderPass));

    /* create depth resources */
    depthImages = (VkImage*)malloc(imageCount * sizeof(VkImage));
    depthImageMemorys = (VkDeviceMemory*)malloc(imageCount * sizeof(VkDeviceMemory));
    depthImageViews = (VkImageView*)malloc(imageCount * sizeof(VkImageView));

    for (unsigned char i = 0; i < imageCount; i++) {
        VkImageCreateInfo imageInfo = {0};
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
        createImageWithInfo(&imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthImages[i], &depthImageMemorys[i]);

        VkImageViewCreateInfo viewInfo = {0};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = depthImages[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapChainDepthFormat;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(device_, &viewInfo, NULL, &depthImageViews[i]);
    }

    /* create frame buffers*/
    imagesInFlight = (VkFence*)malloc(imageCount * sizeof(VkFence));
    swapChainFramebuffers = (VkFramebuffer*)malloc(imageCount * sizeof(VkFramebuffer));

    for (unsigned char i = 0; i < imageCount; i++) {
        imagesInFlight[i] = VK_NULL_HANDLE;
        const VkImageView attachments[2] = { swapChainImageViews[i], depthImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo = {0};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = windowExtent.width;
        framebufferInfo.height = windowExtent.height;
        framebufferInfo.layers = 1;
        vkCreateFramebuffer(device_, &framebufferInfo, NULL, &swapChainFramebuffers[i]);
    }

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (unsigned char i = 0; i < ZENGINE_MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(device_, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]);
        vkCreateSemaphore(device_, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]);
        vkCreateFence(device_, &fenceInfo, NULL, &inFlightFences[i]);
    }
}

void deleteSwapChain() {
    for (unsigned char i = 0; i < imageCount; i++) { 
        vkDestroyFramebuffer(device_, swapChainFramebuffers[i], NULL);
        vkDestroyImageView(device_, depthImageViews[i], NULL);
        vkDestroyImage(device_, depthImages[i], NULL);
        vkFreeMemory(device_, depthImageMemorys[i], NULL);
        vkDestroyImageView(device_, swapChainImageViews[i], NULL);
    }

    vkDestroyRenderPass(device_, renderPass, NULL);
    if (!ZEngineClose) vkDestroySwapchainKHR(device_, swapChain, NULL);

    for (unsigned char i = 0; i < ZENGINE_MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device_, imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(device_, renderFinishedSemaphores[i], NULL);
        vkDestroyFence(device_, inFlightFences[i], NULL);
    }

    oldSwapChain = VK_NULL_HANDLE;
    free(swapChainImages);
    free(swapChainFramebuffers);
    free(depthImageViews);
    free(depthImages);
    free(depthImageMemorys);
    free(swapChainImageViews);
    free(imagesInFlight);
}

VkResult acquireNextImage(unsigned int* imageIndex) { return vkAcquireNextImageKHR(device_, swapChain, 18446744073709551615ULL, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, imageIndex); }
    
void submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex) {
    if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) { vkWaitForFences(device_, 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX); }
    imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

    const VkSemaphore waitSemaphores[1] = { imageAvailableSemaphores[currentFrame] };
    const VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    const VkSemaphore signalSemaphores[1] = { renderFinishedSemaphores[currentFrame] };

    VkSubmitInfo submitInfo = {0};
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

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = imageIndex;

    currentFrame = (currentFrame + 1) % ZENGINE_MAX_FRAMES_IN_FLIGHT;
    vkQueuePresentKHR(presentQueue_, &presentInfo);
}

/* TEXTURE FUNCTIONS */
void createTextureSampler(Texture* texture) {
    VkSamplerCreateInfo samplerInfo = {0};
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
    vkCreateSampler(device_, &samplerInfo, NULL, &texture->sampler);
}

void createTexture(Texture* texture, const char* filepath) {
    int width = 0; int height = 0;
    char name[64];
    snprintf(name, 64, "assets/images/%s", filepath);
    stbi_uc* pixels = stbi_load(name, &width, &height, &texture->channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;

    createImageBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &texture->buffer, &texture->bufferMemory);

    void* data;
    vkMapMemory(device_, texture->bufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device_, texture->bufferMemory);
    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    createImageWithInfo(&imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->image, &texture->memory);
    transitionImageLayout(texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(texture->buffer, texture->image, width, height, 1);
    transitionImageLayout(texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device_, texture->buffer, NULL);
    vkFreeMemory(device_, texture->bufferMemory, NULL);

    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = texture->image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(device_, &viewInfo, NULL, &texture->view);
    createTextureSampler(texture);
}

void createTextureFromData(Texture* texture, const unsigned char* pixelData, const unsigned short width, const unsigned short height) {
    VkDeviceSize imageSize = width * height * 4;

    unsigned char* pixels = (unsigned char*)malloc(imageSize);
    memset(pixels, 0xFF, imageSize);
    for (unsigned int i = 0; i < width * height; i++) { pixels[i * 4 + 3] = pixelData[i]; }

    createImageBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &texture->buffer, &texture->bufferMemory);

    void* data;
    vkMapMemory(device_, texture->bufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device_, texture->bufferMemory);

    VkImageCreateInfo imageInfo = {0};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    createImageWithInfo(&imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texture->image, &texture->memory);
    transitionImageLayout(texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(texture->buffer, texture->image, width, height, 1);
    transitionImageLayout(texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device_, texture->buffer, NULL);
    vkFreeMemory(device_, texture->bufferMemory, NULL);

    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = texture->image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    free(pixels);
    vkCreateImageView(device_, &viewInfo, NULL, &texture->view);
    createTextureSampler(texture);
}

void deleteTexture(Texture* texture) {
    if (!ZEngineClose) { vkDeviceWaitIdle(device_); }
    vkDestroySampler(device_, texture->sampler, NULL);
    vkDestroyImageView(device_, texture->view, NULL);
    vkDestroyImage(device_, texture->image, NULL);
    vkFreeMemory(device_, texture->memory, NULL);
}

void transitionImageLayout(Texture* texture, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = texture->image;
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

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier);
    endSingleTimeCommands(commandBuffer);
    texture->layout = newLayout;
}

/* BUFFER FUNCS */
void createBuffer(Buffer* buffer, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags) {
    /* HOW TO INIT YOUR BUFFER:
        buffer = (Buffer*)calloc(1, sizeof(Buffer));
    */

    buffer->bufferSize = instanceSize * instanceCount;
    buffer->mapped = NULL;

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = buffer->bufferSize;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ZENGINE_THROW(vkCreateBuffer(device_, &bufferInfo, NULL, &buffer->buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, buffer->buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

    ZENGINE_THROW(vkAllocateMemory(device_, &allocInfo, NULL, &buffer->memory));
    ZENGINE_THROW(vkBindBufferMemory(device_, buffer->buffer, buffer->memory, 0));
}

void deleteBuffer(Buffer* buffer) {
    vkDestroyBuffer(device_, buffer->buffer, NULL);
    vkFreeMemory(device_, buffer->memory, NULL);
    buffer->buffer = VK_NULL_HANDLE;
    buffer->memory = VK_NULL_HANDLE;
}

void map(Buffer* buffer) {
    if (buffer->mapped == NULL) {
        ZENGINE_THROW(vkMapMemory(device_, buffer->memory, 0, buffer->bufferSize, 0, &buffer->mapped));
    }
}
void unmap(Buffer* buffer) {
    if (buffer->mapped != NULL) {
        vkUnmapMemory(device_, buffer->memory);
        buffer->mapped = NULL;
    }
}

void write(Buffer* buffer, const void* data, unsigned int size) { memcpy(buffer->mapped, data, size); }

/* ZENIGNE HELPER FUNCTIONS */
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (unsigned int i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = 1;
        }

        VkBool32 presentSupport = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
        if (queueFamilies[i].queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = 1;
        }
        if (indices.graphicsFamilyHasValue && indices.presentFamilyHasValue) { break; }
        i++;
    }
    return indices;
}

void querySwapChainSupport(SwapChainSupportDetails* details, VkPhysicalDevice device) {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details->capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &details->formatsSize, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &details->presentModeSize, NULL);

    if (details->formatsSize != 0) {
        details->formats = (VkSurfaceFormatKHR*)malloc(details->formatsSize * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &details->formatsSize, details->formats);
    }
    if (details->presentModeSize != 0) {
        details->presentModes = (VkPresentModeKHR*)malloc(details->presentModeSize * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &details->presentModeSize, details->presentModes);
    }
}

VkFormat findSupportedFormat(const VkFormat candidate1, const VkFormat candidate2, const VkFormat candidate3, VkImageTiling tiling, VkFormatFeatureFlags features) {
    VkFormatProperties props;
    VkFormat format;
    for (unsigned char i = 0; i < 3; i++) {
        switch(i) {
        case 0: format = candidate1; break;
        case 1: format = candidate2; break;
        case 2: format = candidate3; break;
        default: break;
        }

        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) { return format; }
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) { return format; }
    }
    printf("failed to find supported format");
    exit(1);
}

VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {0};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

VkVertexInputAttributeDescription* getAttributeDescriptions() {
    VkVertexInputAttributeDescription* attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(2 * sizeof(VkVertexInputAttributeDescription));
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = 0;
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = 8;
    return attributeDescriptions;
}

unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (unsigned int i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) { return i; }
    }

    printf("failed to find suitable memory type\n");
    exit(1);
}

VkCommandBuffer beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer = {0};
    vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);
    vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
}

void createImageBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {
    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCreateBuffer(device_, &bufferInfo, NULL, buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device_, &allocInfo, NULL, bufferMemory);
    vkBindBufferMemory(device_, *buffer, *bufferMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkBufferCopy copyRegion = {0};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommands(commandBuffer);
}

void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layerCount) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkBufferImageCopy region = {0};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = layerCount;
    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    endSingleTimeCommands(commandBuffer);
}

void createImageWithInfo(const VkImageCreateInfo* imageInfo, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory) {
    vkCreateImage(device_, imageInfo, NULL, image);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device_, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device_, &allocInfo, NULL, imageMemory);
    vkBindImageMemory(device_, *image, *imageMemory, 0);
}

/* ZENGINE HELPER FUNCTIONS */
void createModel(Model* model, float* positions, const unsigned int verticySize) { /* verticies are X and Y positions, verticy size is the size of the array / 2 */
    /* INIT YOUR MODEL:
        model = (Model*)malloc(sizeof(Model));
    */

    model->vertexBuffer = (Buffer*)calloc(1, sizeof(Buffer));
    model->vertices = (Vertex*)calloc(1, verticySize * sizeof(Vertex));
    model->verticySize = verticySize;

    unsigned int index = 0;
    for (unsigned int i = 0; i < verticySize; ++i) {
        Vertex v;
        index = i << 1; /* << 1 is * 2 lol*/
        v.pos[0] = positions[index];
        v.pos[1] = positions[index + 1];
        v.cord[0] = positions[index] + 0.5f;
        v.cord[1] = positions[index + 1] + 0.5f;
        model->vertices[i] = v;
    }

    createBuffer(model->vertexBuffer, sizeof(Vertex) * verticySize, 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    map(model->vertexBuffer);
    write(model->vertexBuffer, (const void*)model->vertices, (unsigned int)(sizeof(Vertex) * verticySize));
    unmap(model->vertexBuffer);
}

void deleteModel(Model* model) {
    free(model->vertices);
}

void bindModel(Model* model, VkCommandBuffer commandBuffer) {
    static VkBuffer buffers[1] = {0};
    buffers[0] = model->vertexBuffer->buffer;
    static VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void drawModel(Model* model, VkCommandBuffer commandBuffer, unsigned int instanceCount, unsigned int firstInstance) { vkCmdDraw(commandBuffer, (unsigned int)model->verticySize, instanceCount, 0, firstInstance); }

void createCommandBuffers() {
    oldImageCount = imageCount;
    commandBuffers = (VkCommandBuffer*)malloc(imageCount * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = imageCount;
    vkAllocateCommandBuffers(device_, &allocInfo, commandBuffers);
}

VkShaderModule createShaderModule(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Failed to open shader!");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    void* buffer = (void*)malloc(fileSize);
    if (!buffer) {
        fclose(file);
        printf("Failed to create shader buffer!");
        exit(1);
    }

    fread(buffer, 1, fileSize, file);
    fclose(file);

    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = (const unsigned int*)buffer;

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device_, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        free(buffer);
        printf("Failed to create shader module!");
        exit(1);
    }

    free(buffer);
    return shaderModule;
}

void createSprite(Model* model, unsigned int textureIndex, float posx, float posy, float scalex, float scaley, float rotation) {
    if (spritesSize >= ZENGINE_MAX_SPRITES) { return; }

    sprites[spritesSize].position[0] = posx;
    sprites[spritesSize].position[1] = posy;
    sprites[spritesSize].scale[0] = scalex;
    sprites[spritesSize].scale[1] = scaley;
    sprites[spritesSize].rotation = rotation;
    sprites[spritesSize].textureIndex = textureIndex;
    sprites[spritesSize].depth = spritesSize / ZENGINE_MAX_SPRITES;
    sprites[spritesSize].model = model;
    sprites[spritesSize].data = NULL;

    spritesSize++;
}

Sprite* createSpritePtr() {
    if (spritesSize >= ZENGINE_MAX_SPRITES) { return NULL; }
    createSprite(squareModel, 0, 0.f, 0.f, .1f, .1f, 0);
    return &sprites[spritesSize - 1];
}

void initSprite(Sprite* sprite) {
    sprite->position[0] = 0.f;
    sprite->position[1] = 0.f;
    sprite->scale[0] = .1f;
    sprite->scale[1] = .1f;
    sprite->rotation = 0.f;
    sprite->textureIndex = 0;
    sprite->depth = spritesSize - 1;
    sprite->model = squareModel;
    sprite->data = NULL;
}

void deleteSpritePointer(Sprite* sprite) {
    for (unsigned int i = (unsigned int)(sprite->depth * ZENGINE_MAX_SPRITES); i < spritesSize - 1; i++) {
        sprites[i] = sprites[i + 1];
        sprites[i].depth -= 1 / ZENGINE_MAX_SPRITES;
    }
    spritesSize--;
    deleteModel(sprites[spritesSize].model);
    sprites[spritesSize].data = NULL;
}

void deleteSprite(unsigned int sprite) {
    for (unsigned int i = sprite; i < spritesSize; i++) {
        sprites[i] = sprites[i + 1];
        sprites[i].depth -= 1 / ZENGINE_MAX_SPRITES;
    }

    spritesSize--;
    deleteModel(sprites[spritesSize].model);
    sprites[spritesSize].data = NULL;
}

void setRotationMatrix(Sprite* sprite) {
    sprite->rotationMatrix[0] = cos(sprite->rotation * .01745329f);
    sprite->rotationMatrix[2] = sin(sprite->rotation * .01745329f);
    sprite->rotationMatrix[1] = -sprite->rotationMatrix[2];
    sprite->rotationMatrix[3] = sprite->rotationMatrix[0];
}

void updateTexture(unsigned int index, Texture* texture) {
    spriteTextures[index] = *texture;

    VkDescriptorImageInfo imageInfo = {0};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = spriteTextures[index].view;
    imageInfo.sampler = spriteTextures[index].sampler;

    VkWriteDescriptorSet imageWrite = {0};
    imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWrite.dstSet = spriteDataDescriptorSet;
    imageWrite.dstBinding = 1;
    imageWrite.dstArrayElement = index;
    imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageWrite.descriptorCount = 1;
    imageWrite.pImageInfo = &imageInfo;
    vkUpdateDescriptorSets(device_, 1, &imageWrite, 0, NULL);
}

/* ZENGINE */
void ZEngineInit() {
    ZENGINE_PRINT("Creating instance...\n"); //---------------------------------------------------------------------------------------------------------------
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ZDev";
    appInfo.pEngineName = "ZEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 11, 0);
    appInfo.apiVersion =  VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceInfo = {0};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

#ifdef __APPLE__
    const char* instanceExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_MVK_macos_surface",
        VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
    };

    instanceInfo.enabledExtensionCount = 3;
    instanceInfo.ppEnabledExtensionNames = instanceExtensions;
    instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
    size_t rgfWExtensionCount = 0;
    const char** rgfWExtensions = RGFW_getRequiredInstanceExtensions_Vulkan(&rgfWExtensionCount);
    if (!rgfWExtensions || rgfWExtensionCount == 0) {
        printf("Failed to get Vulkan extensions");
        exit(1);
    }

    ZENGINE_PRINT("Enabling extensions:\n");
    for (size_t i = 0; i < rgfWExtensionCount; i++) { ZENGINE_PRINT("     - %s\n", rgfWExtensions[i]); }

    instanceInfo.enabledExtensionCount = rgfWExtensionCount;
    instanceInfo.ppEnabledExtensionNames = rgfWExtensions;
#endif

#ifdef ZENGINE_DEBUG
    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);
    ZENGINE_PRINT("Instance: %d\n", result );
#else
    vkCreateInstance(&instanceInfo, NULL, &instance);
#endif

    ZENGINE_PRINT("Creating surface...\n"); RGFW_window_createSurface_Vulkan(zwindow, instance, &surface_); //---------------------------------------------------------------------------------------------------------------

    ZENGINE_PRINT("Creating physical device...\n"); //---------------------------------------------------------------------------------------------------------------
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (deviceCount == 0) { 
        printf("No Vulkan-compatible GPUs found");
        exit(1);
    }
    else if (deviceCount == 1) { ZENGINE_PRINT("Found 1 GPU\n"); }
    else { ZENGINE_PRINT("Found %u GPUs\n", deviceCount); }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    short highScore = 0;
    for (unsigned char i = 0; i < deviceCount; ++i) {
        /* check if device is suitable, and score it */
        unsigned short newScore = 0;
        QueueFamilyIndices indices = findQueueFamilies(devices[i]);
        unsigned int extensionCount = 0;
        _Bool swapChainAdequate = 0;

        /* check if extensions are supported on the GPU: */
        vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, NULL);
        VkExtensionProperties availableExtensions[extensionCount];
        vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, availableExtensions);

        SwapChainSupportDetails swapChainSupport;
        querySwapChainSupport(&swapChainSupport, devices[i]);
        swapChainAdequate = !swapChainSupport.formatsSize == 0 && !swapChainSupport.presentModeSize == 0;
        ZENGINE_PRINT("SwapChain formats: %u, presentmodes: %u\n", swapChainSupport.formatsSize, swapChainSupport.presentModeSize);
        for (unsigned char i = 0; i < swapChainSupport.presentModeSize; i++) {
            if (swapChainSupport.presentModes[i] == 0) { ZENGINE_PRINT("    - GPU supports VSync\n"); }
            else if (swapChainSupport.presentModes[i] == 2) { ZENGINE_PRINT("    - GPU can disable VSync\n"); }
        }
        newScore += swapChainSupport.formatsSize * swapChainSupport.presentModeSize;

        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(devices[i], &supportedFeatures);
        if (!(indices.graphicsFamilyHasValue && indices.presentFamilyHasValue && swapChainAdequate)) { newScore = 0; }
        else if (newScore > highScore) {
            highScore = newScore;
            physicalDevice = devices[i];
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) { physicalDevice = devices[0]; ZENGINE_PRINT("Selected GPU is unsupported! Expect bugs!\n"); }
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    ZENGINE_PRINT("Selected GPU: %s\n", properties.deviceName);

    ZENGINE_PRINT("Creating logical device...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    float queuePriority = 1.0f;
 
    unsigned char queueFamilySize = 1;
    if (indices.presentFamily != indices.graphicsFamily) { queueFamilySize = 2; }

    unsigned int* uniqueQueueFamilies = (unsigned int*)malloc(queueFamilySize * 4);
    uniqueQueueFamilies[0] = indices.graphicsFamily;
    if (queueFamilySize == 2) { uniqueQueueFamilies[1] = indices.presentFamily; }

    VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc(queueFamilySize * sizeof(VkDeviceQueueCreateInfo));
 
    for (unsigned char i = 0; i < queueFamilySize; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkDeviceCreateInfo deviceInfo = {0};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = queueCreateInfos;
    deviceInfo.queueCreateInfoCount = queueFamilySize;

#ifdef __APPLE__
    const char* extension[2] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        "VK_KHR_portability_subset"
    };
    deviceInfo.enabledExtensionCount = 2;
#else
    const char* extension[1] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    deviceInfo.enabledExtensionCount = 1;
#endif
    deviceInfo.ppEnabledExtensionNames = extension;
    deviceInfo.enabledLayerCount = 0;
    ZENGINE_THROW(vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device_));

    vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
    vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);

    ZENGINE_PRINT("Creating command pool...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
    VkCommandPoolCreateInfo commandPoolInfo = {0};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    ZENGINE_THROW(vkCreateCommandPool(device_, &commandPoolInfo, NULL, &commandPool));

    ZENGINE_PRINT("Creating swapChain...\n"); //---------------------------------------------------------------------------------------------------------------
    createSwapChain();
    createCommandBuffers();

    VkPipelineShaderStageCreateInfo shaderStages[2] = {0};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = createShaderModule("shaders/texture.vert.spv");
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = createShaderModule("shaders/texture.frag.spv");
    shaderStages[1].pName = "main";

    VkVertexInputBindingDescription bindingDescription = getBindingDescription();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = getAttributeDescriptions();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkDescriptorSetLayoutBinding layoutBindings[2] = {0};
    layoutBindings[0].binding = 0;
#ifdef __APPLE__
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
#else
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
#endif
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[1].descriptorCount = ZENGINE_MAX_TEXTURES;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = layoutBindings;
    ZENGINE_THROW(vkCreateDescriptorSetLayout(device_, &layoutInfo, NULL, &descriptorSetLayout));

    VkDescriptorPoolSize poolSizes[2] = {{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ZENGINE_MAX_TEXTURES * (ZENGINE_MAX_TEXTURES + 1)}};

    VkDescriptorPoolCreateInfo descriptorPoolInfo = {0};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = 2;
    descriptorPoolInfo.pPoolSizes = poolSizes;
    descriptorPoolInfo.maxSets = ZENGINE_MAX_TEXTURES + 1;
    ZENGINE_THROW(vkCreateDescriptorPool(device_, &descriptorPoolInfo, NULL, &descriptorPool));

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = NULL;
    ZENGINE_THROW(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, NULL, &pipelineLayout));

    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
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
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    ZENGINE_THROW(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline));

    vkDestroyShaderModule(device_, shaderStages[0].module, NULL);
    vkDestroyShaderModule(device_, shaderStages[1].module, NULL);

    /* create the pipeline layout */
    VkPushConstantRange pushConstantRange = {0};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(Camera);

    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    ZENGINE_THROW(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, NULL, &pipelineLayout));

    ZENGINE_PRINT("Initing textures...\n");
    spriteTextures = (Texture*)malloc(ZENGINE_MAX_TEXTURES * sizeof(Texture));
    VkDescriptorImageInfo imageInfos[ZENGINE_MAX_TEXTURES] = {0};
    for (unsigned int i = 0; i < ZENGINE_MAX_TEXTURES; i++) {
        createTexture(&spriteTextures[i], ZENGINE_DEFAULT_TEXTURE);
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView = spriteTextures[i].view;
        imageInfos[i].sampler = spriteTextures[i].sampler;
    }

    ZENGINE_PRINT("Initing sprites...\n");
    sprites = (Sprite*)malloc(ZENGINE_MAX_SPRITES * sizeof(Sprite));
    spriteData = (char*)malloc(SIZEOF_SPRITE_DATA * ZENGINE_MAX_SPRITES);
    spriteDataBuffer = (Buffer*)calloc(1, sizeof(Buffer));

    camera.zoom[0]     = 1.f; camera.zoom[1]     = 1.f;
    camera.position[0] = 0.f; camera.position[1] = 0.f;
    camera.aspect      = (float)windowExtent.width / (float)windowExtent.height;

    float* positions = (float*)malloc(8 * 4);
    positions[0] = -.5f; positions[1] = -.5f; // Bottom left
    positions[2] =  .5f; positions[3] = -.5f; // Bottom right
    positions[4] = -.5f; positions[5] =  .5f; // Top right
    positions[6] =  .5f; positions[7] =  .5f; // Top left
 
    squareModel = (Model*)malloc(sizeof(Model));
    createModel(squareModel, positions, 8);
    free(positions);

#ifdef __APPLE__
    createBuffer(spriteDataBuffer, SIZEOF_SPRITE_DATA * ZENGINE_MAX_SPRITES, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
#else
    createBuffer(spriteDataBuffer, SIZEOF_SPRITE_DATA * ZENGINE_MAX_SPRITES, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
#endif

    map(spriteDataBuffer);

#ifndef ZENGINE_DISABLE_AUDIO
    ma_engine_init(NULL, &audio); /* init audio */
#endif

    /* allocate info */
    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    ZENGINE_THROW(vkAllocateDescriptorSets(device_, &allocInfo, &spriteDataDescriptorSet));

    VkDescriptorBufferInfo bufferInfo = {0};
    bufferInfo.buffer = spriteDataBuffer->buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = SIZEOF_SPRITE_DATA * ZENGINE_MAX_SPRITES;

    ZENGINE_PRINT("Writing sprite descriptor sets...\n");
    VkWriteDescriptorSet bufferWrite = {0};
    bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    bufferWrite.dstSet = spriteDataDescriptorSet;
    bufferWrite.dstBinding = 0;
    bufferWrite.dstArrayElement = 0;
#ifdef __APPLE__
    bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
#else
    bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
#endif
    bufferWrite.descriptorCount = 1;
    bufferWrite.pBufferInfo = &bufferInfo;

    VkWriteDescriptorSet imageWrite = {0};
    imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWrite.dstSet = spriteDataDescriptorSet;
    imageWrite.dstBinding = 1;
    imageWrite.dstArrayElement = 0;
    imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageWrite.descriptorCount = ZENGINE_MAX_TEXTURES;
    imageWrite.pImageInfo = imageInfos;

    VkWriteDescriptorSet descriptorWrites[2] = { bufferWrite, imageWrite };
    vkUpdateDescriptorSets(device_, 2, descriptorWrites, 0, NULL);

    ZENGINE_PRINT("ZEngine successfuly initialized!\n");
}

void ZEngineRender() {
    /* resize window */
    if (acquireNextImage(&currentImageIndex) == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
        /* recreate swapchain */
        vkDeviceWaitIdle(device_);
        oldSwapChain = swapChain;
        deleteSwapChain();
        createSwapChain();
        if (imageCount != oldImageCount) {
            vkFreeCommandBuffers(device_, commandPool, imageCount, commandBuffers);
            free(commandBuffers);
            createCommandBuffers();
        }

        framebufferResized = 0;
        camera.aspect = (float)windowExtent.width / (float)windowExtent.height;
        return;
    }

    /* create command buffer */
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBuffer commandBuffer = commandBuffers[currentImageIndex];
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    /* begine render pass */
    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[currentImageIndex];
    renderPassInfo.renderArea.extent = windowExtent;

    VkClearValue clearValues[2] = {0};

    clearValues[1].depthStencil = (VkClearDepthStencilValue){ 1.f, 0 };
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = windowExtent
    };

    VkViewport viewport = (VkViewport){0.f, 0.f, (float)windowExtent.width, (float)windowExtent.height, 0.f, 1.f};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    /* render sprites */
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &spriteDataDescriptorSet, 0, NULL);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Camera), &camera);

#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    if (ZEngineSpriteRemap) {
#endif
        for (unsigned int i = 0; i < spritesSize; i++) {
#ifndef ZENGINE_SPRITE_MATRIXMODE_MANUAL
            setRotationMatrix(&sprites[i]);
#endif
            memcpy(spriteData + i * SIZEOF_SPRITE_DATA, &sprites[i], SIZEOF_SPRITE_DATA);
        }
        memcpy(spriteDataBuffer->mapped, spriteData, SIZEOF_SPRITE_DATA * spritesSize); 
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
        ZEngineSpriteRemap = 0;
    }
#endif

    bindModel(squareModel, commandBuffer);
    drawModel(squareModel, commandBuffer, spritesSize, 0);

    /* end frame */
    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);
    submitCommandBuffers(&commandBuffer, &currentImageIndex);
}

void ZEngineDeinit() {
    ZEngineClose = 1;
    ZENGINE_PRINT("Waiting for GPU...\n"); vkDeviceWaitIdle(device_);

    ZENGINE_PRINT("Freeing command buffers\n"); vkFreeCommandBuffers(device_, commandPool, imageCount, commandBuffers);
    ZENGINE_PRINT("Clearing command buffers\n"); free(commandBuffers);

    ZENGINE_PRINT("Freeing graphics pipeline\n"); vkDestroyPipeline(device_, graphicsPipeline, NULL);
    ZENGINE_PRINT("Freeing pipeline layout\n");   vkDestroyPipelineLayout(device_, pipelineLayout, NULL);
    ZENGINE_PRINT("Freeing discriptor pool\n");   vkFreeDescriptorSets(device_, descriptorPool, 1, &spriteDataDescriptorSet);

    ZENGINE_PRINT("Freeing textures\n"); for (unsigned int i = 0; i < ZENGINE_MAX_TEXTURES; i++) { deleteTexture(&spriteTextures[i]); }
    ZENGINE_PRINT("Unmaping sprite data buffer\n"); unmap(spriteDataBuffer);
    ZENGINE_PRINT("Freeing sprite data buffer\n"); deleteBuffer(spriteDataBuffer);
    ZENGINE_PRINT("Freeing sprite gpu buffer\n"); free(spriteData);
    ZENGINE_PRINT("Freeing models\n"); deleteModel(squareModel); 
    ZENGINE_PRINT("Freeing sprites\n"); free(sprites);
    ZENGINE_PRINT("Freeing swapchain\n"); deleteSwapChain();

    ZENGINE_PRINT("Freeing descriptor set layout\n"); vkDestroyDescriptorSetLayout(device_, descriptorSetLayout, NULL);
    ZENGINE_PRINT("Freeing descriptor pool\n"); vkDestroyDescriptorPool(device_, descriptorPool, NULL);
    ZENGINE_PRINT("Freeing command pool\n"); vkDestroyCommandPool(device_, commandPool, NULL);
    ZENGINE_PRINT("Destroying device\n"); vkDestroyDevice(device_, NULL);
    ZENGINE_PRINT("Freeing window surface\n"); vkDestroySurfaceKHR(instance, surface_, NULL);
    ZENGINE_PRINT("Destroying instance\n"); vkDestroyInstance(instance, NULL);
#ifndef ZENGINE_DISABLE_AUDIO
    ZENGINE_PRINT("Deiniting audio"); ma_engine_uninit(&audio);
#endif
}

#endif // ZENGINE_IMPLEMENTATION
#undef ZENGINE_IMPLEMENTATION
#endif // ZENGINE_H

