/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license

#define ZENGINE_IMPLEMENTATION - define functions INCLUDE IN MAIN.CPP ONLY
#define ZENGINE_DEPS_DEFINED - if your using zdeps.c define this (disables IMPLEMENTATION defines)
#define ZENGINE_DISABLE_VSYNC - extend beyond mortal limitations and exceed maximum fps
#define ZENGINE_SPRITE_MAPMODE_MANUAL - manually change the ZEngineSpriteRemap flag whenever you update sprite data
#define ZENGINE_SPRITE_MATRIXMODE_MANUAL - manually call sprites[0].setRotationMatrix() for every sprite you need
#define ZENGINE_DEPTHMODE_FIRST - makes it so the first created sprites get layered on top of new ones
#define ZENGINE_DEFAULT_TEXTURE "assets/img/bird.png" - change the default error texture to whatever you want

COMPILER FLAGS:
-DZENGINE_DEBUG - adds debug printing for debugging.
-DZENGINE_MAX_SPRITES 10000 - the maximum amount of sprite the engine can load at once (more sprites, more memory usage)
-DZENGINE_MAX_TEXTURES 50 - the maximum amount of texture the engine can load at once
-DZENGINE_DISABLE_AUDIO - disables audio, and dosen't include miniaudio.h or init it.
*/

#ifndef ZENGINE_H
#define ZENGINE_H

#ifdef ZENGINE_IMPLEMENTATION
/* define a few necissary macros if not already defined */
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

#if ZENGINE_MAX_TEXTURES < 25
    #undef ZENGINE_MAX_TEXTURES
    #define ZENGINE_MAX_TEXTURES 25
#endif

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
    #define ZENGINE_DEFAULT_TEXTURE "assets/img/e.png"
#endif

#if !defined(ZENGINE_DEPS_DEFINED)
    #define MINIAUDIO_IMPLEMENTATION

    #define RGFW_IMPLEMENTATION
    #define RGFW_ASSERT(x) (void)(x)

    #define STB_IMAGE_IMPLEMENTATION
    #define STBI_ASSERT
#endif

#endif // ZENGINE_IMPLEMENTATION

/* dependencies */
#define RGFW_VULKAN
#define RGFW_EXPORT

#include "deps/RGFW.h" /* window */
#include "deps/stb_image.h" /* image */

#ifdef ZENGINE_DISABLE_AUDIO
    #define ZENGINE_AUDIO unsigned char audio /* 1 byte to prevent errors */
#else
    #include "deps/miniaudio.h" /* audio */
    #define ZENGINE_AUDIO ma_engine audio
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
    Buffer vertexBuffer;
    Vertex vertices[4];
} Model;

typedef struct __attribute__((aligned(16))) Sprite {
    float position[2];
    float scale[2];
    float rotationMatrix[4];

    float depth;
    unsigned int textureIndex;
    float rotation;
    float padding;
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
    _Bool loaded;
} Texture;

/* extern vars */
extern double deltaTime;
extern struct Texture spriteTextures[ZENGINE_MAX_TEXTURES];
extern struct Sprite sprites[ZENGINE_MAX_SPRITES];
extern unsigned int spritesSize;
extern struct Model zmodel;
extern ZENGINE_AUDIO;
extern Camera camera;
extern _Bool framebufferResized;
extern RGFW_window* zwindow;
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    extern _Bool ZEngineSpriteRemap;
#endif

/* ZENGINE FORWARD-DECLARED FUNCTIONS */
void ZEngineInit();
void ZEngineRender();
void ZEngineDeinit();

/* sprite funcs */
void createSprite(float positionx, float positiony, float scalex, float scaley, float rotation, unsigned int textureIndex);
Sprite* createSpritePtr(float posx, float posy, float scalex, float scaley, float rotation, unsigned int textureIndex);
void deleteSpritePtr(Sprite* sprite);
void deleteSprite(unsigned int sprite);
void setRotationMatrix(Sprite* sprite);

/* engine funcs */
void createCommandBuffers();
VkCommandBuffer beginSingleTimeCommands();
VkShaderModule createShaderModule(const char* filepath);
void endSingleTimeCommands(VkCommandBuffer commandBuffer);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
void querySwapChainSupport(SwapChainSupportDetails* details, VkPhysicalDevice device);
unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);
void copyBufferToImage(VkBuffer buffer, VkImage image, unsigned int width, unsigned int height, unsigned int layerCount);
void createImageWithInfo(const VkImageCreateInfo* imageInfo, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);

/* struct funcs */
VkVertexInputBindingDescription getBindingDescription();
VkVertexInputAttributeDescription* getAttributeDescriptions();

/* swapchain funcs */
void createSwapChain();
void deleteSwapChain();
VkResult acquireNextImage(unsigned int* imageIndex);
void submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex);

/* texture funcs*/
void createTexture(const char* filepath, float opacity, unsigned int index);
void transitionImageLayout(unsigned int index, VkImageLayout oldLayout, VkImageLayout newLayout);

/* buffer funcs */
void createBuffer(Buffer* buffer, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags);

#ifdef ZENGINE_IMPLEMENTATION
/* PRIVATE VARS */

/* kinda just chillin ngl */
Camera camera;
ZENGINE_AUDIO;

double deltaTime = 0.0; /* deltaTime, do what you will. Example implementation in main.cpp */
_Bool ZEngineClose = 0; /* flag to show when the engine is closing */
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    _Bool ZEngineSpriteRemap = 1; /* flag to update sprite data buffer with ZENGINE_SPRITE_MAPMODE_MANUAL */
#endif

/* texture vecs */
struct Texture spriteTextures[ZENGINE_MAX_TEXTURES] = {0};
char* spriteData;

/* window vars */
_Bool framebufferResized = 0;
RGFW_window* zwindow = NULL;
VkExtent2D windowExtent;

/* sprite vars */
struct Sprite sprites[ZENGINE_MAX_SPRITES] = {0};
unsigned int spritesSize = 0;

/* device vars */
VkInstance instance;
VkCommandPool commandPool;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkPhysicalDeviceProperties properties;
VkDevice device_;
VkQueue presentQueue_;
VkSurfaceKHR surface_;
VkQueue graphicsQueue_;
_Bool uniformBuffer = 0;

/* renderer vars */
VkCommandBuffer* commandBuffers = NULL;
unsigned int currentImageIndex;

/* pipeline vars */
VkPipeline graphicsPipeline;
VkPipelineLayout pipelineLayout;
VkDescriptorSetLayout descriptorSetLayout;
VkDescriptorPool descriptorPool;
struct Model zmodel;

/* rendersystem vars */
VkDescriptorSet spriteDataDescriptorSet;
struct Buffer spriteDataBuffer;

/* swapchain vars */
VkSwapchainKHR swapChain;
VkSwapchainKHR oldSwapChain;
VkFormat swapChainImageFormat;
VkRenderPass renderPass;
VkFramebuffer* swapChainFramebuffers = NULL;
VkImage* depthImages = NULL;
VkDeviceMemory* depthImageMemorys = NULL;
VkImageView* depthImageViews = NULL;
VkImage* swapChainImages = NULL;
VkImageView* swapChainImageViews = NULL;
VkSemaphore imageAvailableSemaphores;
VkSemaphore renderFinishedSemaphores;
VkFence inFlightFences;
VkFence* imagesInFlight = NULL;
unsigned int imageCount;
unsigned int oldImageCount;

/* SWAP CHAIN FUNCTIONS */
void createSwapChain() {
    ZENGINE_PRINT("  - Creating swapchain KHR\n");
    SwapChainSupportDetails swapChainSupport;
    querySwapChainSupport(&swapChainSupport, physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = swapChainSupport.formats[0];
    for (unsigned int i = 0; i < swapChainSupport.formatsSize; i++) {
        if (swapChainSupport.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swapChainSupport.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = swapChainSupport.formats[i];
            break;
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

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (unsigned int i = 0; i < swapChainSupport.presentModeSize; i++) {
#ifdef ZENGINE_DISABLE_VSYNC
        if (swapChainSupport.presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
            presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            break;
        }
#else
        if (swapChainSupport.presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
#endif
    }
    createInfo.presentMode = presentMode;
    free(swapChainSupport.formats);
    free(swapChainSupport.presentModes);

    ZENGINE_THROW(vkCreateSwapchainKHR(device_, &createInfo, NULL, &swapChain));
    ZENGINE_THROW(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, NULL));
    swapChainImages = (VkImage*)malloc(imageCount * sizeof(VkImage));
    ZENGINE_THROW(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, swapChainImages));
    swapChainImageFormat = surfaceFormat.format;

    ZENGINE_PRINT("  - Creating image views\n");
    swapChainImageViews = (VkImageView*)malloc(imageCount * sizeof(VkImageView));

    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
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

    ZENGINE_PRINT("  - Creating renderpass\n");
    VkAttachmentDescription depthAttachment = {0};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
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

    ZENGINE_PRINT("  - Creating depth resources\n");
    depthImages = (VkImage*)malloc(imageCount * sizeof(VkImage));
    depthImageMemorys = (VkDeviceMemory*)malloc(imageCount * sizeof(VkDeviceMemory));
    depthImageViews = (VkImageView*)malloc(imageCount * sizeof(VkImageView));

    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
        VkImageCreateInfo imageInfo = {0};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = windowExtent.width;
        imageInfo.extent.height = windowExtent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_D32_SFLOAT;
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
        viewInfo.format = VK_FORMAT_D32_SFLOAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        vkCreateImageView(device_, &viewInfo, NULL, &depthImageViews[i]);
    }

    ZENGINE_PRINT("  - Creating frame buffers\n");
    imagesInFlight = (VkFence*)malloc(imageCount * sizeof(VkFence));
    swapChainFramebuffers = (VkFramebuffer*)malloc(imageCount * sizeof(VkFramebuffer));

    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
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

    vkCreateSemaphore(device_, &semaphoreInfo, NULL, &imageAvailableSemaphores);
    vkCreateSemaphore(device_, &semaphoreInfo, NULL, &renderFinishedSemaphores);
    vkCreateFence(device_, &fenceInfo, NULL, &inFlightFences);
}

void deleteSwapChain() {
    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
        vkDestroyFramebuffer(device_, swapChainFramebuffers[i], NULL);
        vkDestroyImageView(device_, depthImageViews[i], NULL);
        vkDestroyImage(device_, depthImages[i], NULL);
        vkFreeMemory(device_, depthImageMemorys[i], NULL);
        vkDestroyImageView(device_, swapChainImageViews[i], NULL);
    }

    vkDestroyRenderPass(device_, renderPass, NULL);
    if (!ZEngineClose) vkDestroySwapchainKHR(device_, swapChain, NULL);

    vkDestroySemaphore(device_, imageAvailableSemaphores, NULL);
    vkDestroySemaphore(device_, renderFinishedSemaphores, NULL);
    vkDestroyFence(device_, inFlightFences, NULL);

    oldSwapChain = VK_NULL_HANDLE;
    free(swapChainImages);
    free(swapChainFramebuffers);
    free(depthImageViews);
    free(depthImages);
    free(depthImageMemorys);
    free(swapChainImageViews);
    free(imagesInFlight);
}

VkResult acquireNextImage(unsigned int* imageIndex) { return vkAcquireNextImageKHR(device_, swapChain, 18446744073709551615ULL, imageAvailableSemaphores, VK_NULL_HANDLE, imageIndex); }

void submitCommandBuffers(const VkCommandBuffer* buffers, unsigned int* imageIndex) {
    if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE) { vkWaitForFences(device_, 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX); }
    imagesInFlight[*imageIndex] = inFlightFences;

    const VkSemaphore waitSemaphores[1] = { imageAvailableSemaphores };
    const VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    const VkSemaphore signalSemaphores[1] = { renderFinishedSemaphores };

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device_, 1, &inFlightFences);
    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFences);

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = imageIndex;

    vkQueuePresentKHR(presentQueue_, &presentInfo);
}

/* TEXTURE FUNCTIONS */
void deleteTexture(unsigned int index) {
    if (spriteTextures[index].loaded) {
        vkDeviceWaitIdle(device_);
        vkDestroySampler(device_, spriteTextures[index].sampler, NULL);
        vkDestroyImageView(device_, spriteTextures[index].view, NULL);
        vkDestroyImage(device_, spriteTextures[index].image, NULL);
        vkFreeMemory(device_, spriteTextures[index].memory, NULL);
    }
    else { spriteTextures[index].loaded = 1; }
}
void createTexture(const char* filepath, float opacity, unsigned int index) {
    deleteTexture(index);
    int width = 0; int height = 0;
    stbi_uc* pixels = stbi_load(filepath, &width, &height, NULL, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;

    if (opacity != 1.f) {
        for (unsigned int i = 3; i < (unsigned int)imageSize; i += 4) { pixels[i] *= opacity; }
    }

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCreateBuffer(device_, &bufferInfo, NULL, &spriteTextures[index].buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, spriteTextures[index].buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(device_, &allocInfo, NULL, &spriteTextures[index].bufferMemory);
    vkBindBufferMemory(device_, spriteTextures[index].buffer, spriteTextures[index].bufferMemory, 0);

    void* data;
    vkMapMemory(device_, spriteTextures[index].bufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(device_, spriteTextures[index].bufferMemory);
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

    createImageWithInfo(&imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &spriteTextures[index].image, &spriteTextures[index].memory);
    transitionImageLayout(index, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(spriteTextures[index].buffer, spriteTextures[index].image, width, height, 1);
    transitionImageLayout(index, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device_, spriteTextures[index].buffer, NULL);
    vkFreeMemory(device_, spriteTextures[index].bufferMemory, NULL);

    VkImageViewCreateInfo viewInfo = {0};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = spriteTextures[index].image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    vkCreateImageView(device_, &viewInfo, NULL, &spriteTextures[index].view);

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
    vkCreateSampler(device_, &samplerInfo, NULL, &spriteTextures[index].sampler);

    VkDescriptorImageInfo moreImageInfo = {0};
    moreImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    moreImageInfo.imageView = spriteTextures[index].view;
    moreImageInfo.sampler = spriteTextures[index].sampler;

    VkWriteDescriptorSet imageWrite = {0};
    imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWrite.dstSet = spriteDataDescriptorSet;
    imageWrite.dstBinding = 1;
    imageWrite.dstArrayElement = index;
    imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    imageWrite.descriptorCount = 1;
    imageWrite.pImageInfo = &moreImageInfo;
    vkUpdateDescriptorSets(device_, 1, &imageWrite, 0, NULL);
}

void transitionImageLayout(unsigned int index, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = spriteTextures[index].image;
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
    spriteTextures[index].layout = newLayout;
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

    return 0;
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
        ZENGINE_PRINT("Failed to create shader module!");
        exit(1);
    }

    free(buffer);
    return shaderModule;
}

void createSprite(float posx, float posy, float scalex, float scaley, float rotation, unsigned int textureIndex) {
    if (spritesSize >= ZENGINE_MAX_SPRITES) { return; }

    sprites[spritesSize].position[0] = posx;
    sprites[spritesSize].position[1] = posy;
    sprites[spritesSize].scale[0] = scalex;
    sprites[spritesSize].scale[1] = scaley;
    sprites[spritesSize].rotation = rotation;
    sprites[spritesSize].textureIndex = textureIndex;
#ifdef ZENGINE_DEPTHMODE_FIRST
    sprites[spritesSize].depth = spritesSize / ZENGINE_MAX_SPRITES;
#else
    sprites[spritesSize].depth = .999f - ((float)spritesSize * 0.00001f);
#endif
    spritesSize++;
}

Sprite* createSpritePtr(float posx, float posy, float scalex, float scaley, float rotation, unsigned int textureIndex) {
    if (spritesSize >= ZENGINE_MAX_SPRITES) { return NULL; }
    createSprite(posx, posy, scalex, scaley, rotation, textureIndex);
    return &sprites[spritesSize - 1];
}

void deleteSpritePtr(Sprite* sprite) {
    deleteSprite(sprite - sprites);
}

void deleteSprite(unsigned int sprite) {
    spritesSize--;
    sprites[sprite] = sprites[spritesSize];
    for (unsigned int i = sprite; i < spritesSize - 1; i++) {
#ifdef ZENGINE_DEPTHMODE_FIRST
        sprites[i].depth -= 1 / ZENGINE_MAX_SPRITES;
#else
        sprites[i].depth = .999f - ((float)i * 0.00001f);
#endif
    }
}

void setRotationMatrix(Sprite* sprite) {
    sprite->rotationMatrix[0] = cos(sprite->rotation * .01745329f);
    sprite->rotationMatrix[2] = sin(sprite->rotation * .01745329f);
    sprite->rotationMatrix[1] = -sprite->rotationMatrix[2];
    sprite->rotationMatrix[3] = sprite->rotationMatrix[0];
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
        "VK_MVK_macos_surface"
    };

    instanceInfo.enabledExtensionCount = 2;
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
    if  (result != 0) {
        ZENGINE_PRINT("Instance creation failed - error code: %d\n", result);
    }
#else
    vkCreateInstance(&instanceInfo, NULL, &instance);
#endif

    ZENGINE_PRINT("Creating surface...\n"); RGFW_window_createSurface_Vulkan(zwindow, instance, &surface_); //---------------------------------------------------------------------------------------------------------------

    ZENGINE_PRINT("Creating physical device...\n"); //---------------------------------------------------------------------------------------------------------------
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if (deviceCount == 0) {
        ZENGINE_PRINT("No Vulkan-compatible GPUs found!\nPlease install vulkan drivers: Intel: sudo pacman -S vulkan-intel || AMD: sudo pacman -S vulkan-radeon || If you are not on arch linux, please follow the guide in https://github.com/ZDev22/ZEngine/blob/main/README.md\n");
        exit(1);
    }
    else if (deviceCount == 1) {
        VkPhysicalDevice devices[1];
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
        physicalDevice = devices[0];
    }
    else {
        ZENGINE_PRINT("Found %u GPUs\n", deviceCount);

        VkPhysicalDevice devices[deviceCount];
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

        short highScore = -1;
        for (unsigned char i = 0; i < (unsigned char)deviceCount; ++i) {
            /* check if device is suitable, and score it */
            unsigned short newScore = 0;
            QueueFamilyIndices indices = findQueueFamilies(devices[i]);
            unsigned int extensionCount = 0;

            /* check if extensions are supported on the GPU: */
            vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, NULL);
            VkExtensionProperties availableExtensions[extensionCount];
            vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, availableExtensions);

            SwapChainSupportDetails swapChainSupport;
    #ifdef ZENGINE_DEBUG
            querySwapChainSupport(&swapChainSupport, devices[i]);
            ZENGINE_PRINT("GPU %hhu:\nSwapchain formats: %u, presentmodes: %u\n", i, swapChainSupport.formatsSize, swapChainSupport.presentModeSize);
            for (unsigned char i = 0; i < (unsigned char)swapChainSupport.presentModeSize; i++) {
                if (swapChainSupport.presentModes[i] == 0) { ZENGINE_PRINT("    - GPU supports VSync\n"); }
                else if (swapChainSupport.presentModes[i] == 2) { ZENGINE_PRINT("    - GPU can disable VSync\n"); }
            }
            free(swapChainSupport.formats);
            free(swapChainSupport.presentModes);
    #else
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[i], surface_, &swapChainSupport.capabilities);
            vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface_, &swapChainSupport.formatsSize, NULL);
            vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface_, &swapChainSupport.presentModeSize, NULL);
    #endif

            newScore += swapChainSupport.formatsSize * swapChainSupport.presentModeSize;

            VkPhysicalDeviceFeatures supportedFeatures;
            vkGetPhysicalDeviceFeatures(devices[i], &supportedFeatures);
            if (!(indices.graphicsFamilyHasValue && indices.presentFamilyHasValue && !(swapChainSupport.formatsSize == 0) && !(swapChainSupport.presentModeSize == 0))) { newScore = 0; }
            else if (newScore > highScore) {
                highScore = newScore;
                physicalDevice = devices[i];
                break;
            }
        }
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
	    uniformBuffer = 1;
    }
    ZENGINE_PRINT("Selected GPU: %s\n", properties.deviceName);

    ZENGINE_PRINT("Creating logical device...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    unsigned char queueFamilySize = 1;
    if (indices.presentFamily != indices.graphicsFamily) { queueFamilySize = 2; }

    unsigned int uniqueQueueFamilies[queueFamilySize];
    uniqueQueueFamilies[0] = indices.graphicsFamily;
    if (queueFamilySize == 2) { uniqueQueueFamilies[1] = indices.presentFamily; }

    VkDeviceQueueCreateInfo queueCreateInfos[queueFamilySize];

    for (unsigned char i = 0; i < queueFamilySize; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = NULL;
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
    ZENGINE_PRINT("Creating command buffers...\n");
    createCommandBuffers();

    ZENGINE_PRINT("Initializing shaders...\n");
    VkPipelineShaderStageCreateInfo shaderStages[2] = {0};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = createShaderModule("shaders/texture.vert.spv");
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = createShaderModule("shaders/texture.frag.spv");
    shaderStages[1].pName = "main";

    ZENGINE_PRINT("Rasterizing pipeline data...\n");
    VkVertexInputAttributeDescription* inputAttributes = getAttributeDescriptions();
    VkVertexInputBindingDescription bindingDescription = getBindingDescription();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = inputAttributes;

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

    ZENGINE_PRINT("Creating pipeline layout...\n");
    VkDescriptorSetLayoutBinding layoutBindings[2] = {0};
    layoutBindings[0].binding = 0;
    if (uniformBuffer) { layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
    else { layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }
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

    VkDescriptorPoolSize poolSizes[2] = {
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ZENGINE_MAX_TEXTURES * (ZENGINE_MAX_TEXTURES + 1)}
    };

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
    free(inputAttributes);

    ZENGINE_PRINT("Initing textures...\n");
    VkDescriptorImageInfo imageInfos[ZENGINE_MAX_TEXTURES] = {0};

    VkDescriptorSetAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    ZENGINE_THROW(vkAllocateDescriptorSets(device_, &allocInfo, &spriteDataDescriptorSet));

    createTexture(ZENGINE_DEFAULT_TEXTURE, 1.f, 0);
    imageInfos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfos[0].imageView = spriteTextures[0].view;
    imageInfos[0].sampler = spriteTextures[0].sampler;

    spriteTextures[0].loaded = 0;
    for (unsigned int i = 1; i < ZENGINE_MAX_TEXTURES; i++) {
        spriteTextures[i] = spriteTextures[0];
        imageInfos[i] = imageInfos[0];
    }
    spriteTextures[0].loaded = 1;

    ZENGINE_PRINT("Initing sprites...\n");
    camera.zoom[0]     = 1.f; camera.zoom[1]     = 1.f;
    camera.position[0] = 0.f; camera.position[1] = 0.f;
    camera.aspect      = (float)windowExtent.width / (float)windowExtent.height;

    /* load zmodel */
    zmodel.vertices[0].pos[0] = -.5f;
    zmodel.vertices[0].pos[1] = -.5f;
    zmodel.vertices[0].cord[0] = 0.f;
    zmodel.vertices[0].cord[1] = 0.f;
    zmodel.vertices[1].pos[0] = .5f;
    zmodel.vertices[1].pos[1] = -.5f;
    zmodel.vertices[1].cord[0] = 1.f;
    zmodel.vertices[1].cord[1] = 0.f;
    zmodel.vertices[2].pos[0] = -.5f;
    zmodel.vertices[2].pos[1] = .5f;
    zmodel.vertices[2].cord[0] = 0.f;
    zmodel.vertices[2].cord[1] = 1.f;
    zmodel.vertices[3].pos[0] = .5f;
    zmodel.vertices[3].pos[1] = .5f;
    zmodel.vertices[3].cord[0] = 1.f;
    zmodel.vertices[3].cord[1] = 1.f;

    createBuffer(&zmodel.vertexBuffer, sizeof(Vertex) * 4, 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    map(&zmodel.vertexBuffer);
    memcpy(zmodel.vertexBuffer.mapped, (const void*)zmodel.vertices, (unsigned int)(sizeof(Vertex) * 4));
    unmap(&zmodel.vertexBuffer);

    if (uniformBuffer) { createBuffer(&spriteDataBuffer, sizeof(Sprite) * ZENGINE_MAX_SPRITES, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); }
    else { createBuffer(&spriteDataBuffer, sizeof(Sprite) * ZENGINE_MAX_SPRITES, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); }

    map(&spriteDataBuffer);

#ifndef ZENGINE_DISABLE_AUDIO
    ma_engine_init(NULL, &audio); /* init audio */
#endif

    /* allocate info */
    VkDescriptorBufferInfo bufferInfo = {0};
    bufferInfo.buffer = spriteDataBuffer.buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(Sprite) * ZENGINE_MAX_SPRITES;

    ZENGINE_PRINT("Writing sprite descriptor sets...\n");
    VkWriteDescriptorSet bufferWrite = {0};
    bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    bufferWrite.dstSet = spriteDataDescriptorSet;
    bufferWrite.dstBinding = 0;
    bufferWrite.dstArrayElement = 0;
    if (uniformBuffer) { bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
    else { bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }
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
    vkWaitForFences(device_, 1, &inFlightFences, VK_TRUE, 0xFFFFFFFFFFFFFFFF);
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
#ifndef ZENGINE_SPRITE_MATRIXMODE_MANUAL
        for (unsigned int i = 0; i < spritesSize; i++) {
            setRotationMatrix(&sprites[i]);
        }
#endif
        memcpy(spriteDataBuffer.mapped, sprites, sizeof(Sprite) * spritesSize);
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
        ZEngineSpriteRemap = 0;
    }
#endif

    static VkBuffer buffers[1];
    buffers[0] = zmodel.vertexBuffer.buffer;
    static VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    vkCmdDraw(commandBuffer, 4, spritesSize, 0, 0);

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

    ZENGINE_PRINT("Freeing textures\n"); for (unsigned int i = 0; i < ZENGINE_MAX_TEXTURES; i++) { deleteTexture(i); }
    ZENGINE_PRINT("Unmaping sprite data buffer\n"); unmap(&spriteDataBuffer);
    ZENGINE_PRINT("Freeing sprite data buffer\n"); deleteBuffer(&spriteDataBuffer);
    ZENGINE_PRINT("Freeing swapchain\n"); deleteSwapChain();

    ZENGINE_PRINT("Freeing descriptor set layout\n"); vkDestroyDescriptorSetLayout(device_, descriptorSetLayout, NULL);
    ZENGINE_PRINT("Freeing descriptor pool\n"); vkDestroyDescriptorPool(device_, descriptorPool, NULL);
    ZENGINE_PRINT("Freeing command pool\n"); vkDestroyCommandPool(device_, commandPool, NULL);
    ZENGINE_PRINT("Destroying device\n"); vkDestroyDevice(device_, NULL);
    ZENGINE_PRINT("Freeing window surface\n"); vkDestroySurfaceKHR(instance, surface_, NULL);
    ZENGINE_PRINT("Destroying instance\n"); vkDestroyInstance(instance, NULL);
#ifndef ZENGINE_DISABLE_AUDIO
    ZENGINE_PRINT("Deiniting audio\n"); ma_engine_uninit(&audio);
#endif
}

#undef ZENGINE_IMPLEMENTATION
#endif // ZENGINE_IMPLEMENTATION
#endif // ZENGINE_H
