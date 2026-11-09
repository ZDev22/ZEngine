/* licensed under GPL v3.0 see https://github.com/ZDev22/ZEngine/blob/main/LICENSE for current license
 * RGFW and stb_image MUST be initialized. See zdeps.c for an example implementation, or just use it.

#define ZENGINE_IMPLEMENTATION - define functions INCLUDE IN MAIN.C ONLY
#define ZENGINE_DISABLE_VSYNC - extend beyond mortal limitations and exceed maximum fps
#define ZENGINE_SPRITE_MAPMODE_MANUAL - manually change the ZEngineSpriteRemap flag whenever you update sprite data

COMPILER FLAGS:
-DZENGINE_DEBUG - adds debug printing for debugging.
-DZENGINE_MAX_SPRITES=10000 - the maximum amount of sprite the engine can load at once (more sprites, more memory usage)
-DZENGINE_MAX_TEXTURES=50 - the maximum amount of texture the engine can load at once
-DZENGINE_ENABLE_VALIDATION_LAYERS - enable vulkan validation layers, validation messages print to stderr
-DZENGINE_MAX_FRAMES_IN_FLIGHT=2 - change the maximum frames in flight (default 2)
*/

#ifndef ZENGINE_H
#define ZENGINE_H

#ifdef __cplusplus
    extern "C" {
#else
    #include <stdbool.h>
#endif

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

/* misc definitions */
#ifndef MAX_FRAMES_IN_FLIGHT
    #define MAX_FRAMES_IN_FLIGHT 2
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

#endif // ZENGINE_IMPLEMENTATION

/* dependencies */
#define RGFW_VULKAN
#define RGFW_EXPORT

#include "deps/RGFW.h" /* window */
#include "deps/stb_image.h" /* image */

/* vulkan */
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
    bool graphicsFamilyHasValue;
    bool presentFamilyHasValue;
} QueueFamilyIndices;

typedef struct Texture {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    bool loaded;
} Texture;

/* extern vars */
extern double deltaTime;
extern struct Texture spriteTextures[ZENGINE_MAX_TEXTURES];
extern struct Sprite sprites[ZENGINE_MAX_SPRITES];
extern unsigned int spritesSize;
extern unsigned int availibleSpriteSize;
extern struct Model zmodel;
extern Camera camera;
extern bool framebufferResized;
extern RGFW_window* zwindow;
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    extern bool ZEngineSpriteRemap;
#endif

/* ZENGINE FORWARD-DECLARED FUNCTIONS */
void ZEngineInit();
void ZEngineRender();
void ZEngineDeinit();

/* sprite API */
Sprite* createSprite(float positionx, float positiony, float scalex, float scaley, float rotation, unsigned int textureIndex);
void deleteSprite(Sprite* sprite);
void deleteAllSprites(void);
void rotateSprite(Sprite* sprite, float rotation);

/* engine funcs */
VkShaderModule createShaderModule(const char* filepath);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties);
void createImageWithInfo(const VkImageCreateInfo* imageInfo, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory);

/* swapchain funcs */
void createSwapChain();
void deleteSwapChain();

/* texture funcs*/
void createTexture(const char* filepath, float opacity, unsigned int index);
void createTextureExt(const unsigned char* data, unsigned int index, unsigned int width, unsigned int height, VkDeviceSize imageSize, VkFormat format);

/* buffer funcs */
void createBuffer(Buffer* buffer, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags);

#ifdef ZENGINE_IMPLEMENTATION
/* PRIVATE VARS/EXTERN DEFINITIONS */
Camera camera;

double deltaTime = 0.0; /* deltaTime, do what you will. Example implementation in main.cpp */
bool ZEngineClose = 0; /* flag to show when the engine is closing */
#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    bool ZEngineSpriteRemap = 1; /* flag to update sprite data buffer with ZENGINE_SPRITE_MAPMODE_MANUAL */
#endif

/* texture vecs */
struct Texture spriteTextures[ZENGINE_MAX_TEXTURES] = {0};
VkCommandBuffer textureCommandBuffer;
VkCommandBufferAllocateInfo textureAllocInfo = {(VkStructureType)0};
VkMemoryAllocateInfo textureMoreAllocInfo = {(VkStructureType)0};
VkCommandBufferBeginInfo textureBeginInfo = {(VkStructureType)0};
VkImageCreateInfo textureImageInfo = {(VkStructureType)0};
VkBufferImageCopy textureRegion = {(VkStructureType)0};
VkSubmitInfo textureSubmitInfo = {(VkStructureType)0};
VkBufferCreateInfo textureBufferInfo = {(VkStructureType)0};
VkImageViewCreateInfo textureViewInfo = {(VkStructureType)0};
VkDescriptorImageInfo textureMoreImageInfo = {(VkSampler)0};
VkWriteDescriptorSet textureImageWrite = {(VkStructureType)0};

/* window vars */
bool framebufferResized = 0;
RGFW_window* zwindow = NULL;
VkExtent2D windowExtent;

/* sprite vars */
struct Sprite sprites[ZENGINE_MAX_SPRITES] = {0};
unsigned int availibleSprites[ZENGINE_MAX_SPRITES];
unsigned int spritesSize = 0;
unsigned int availibleSpriteSize = 0;

/* device vars */
VkInstance instance;
VkCommandPool commandPool;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkPhysicalDeviceProperties properties;
VkPhysicalDeviceMemoryProperties memProperties;
VkDevice device_;
VkQueue presentQueue_;
VkSurfaceKHR surface_;
VkQueue graphicsQueue_;
bool uniformBuffer = 0;
#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
VkDebugUtilsMessengerEXT debugMessenger;
#endif

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
SwapChainSupportDetails swapChainSupport;
VkFormat swapChainImageFormat;
VkRenderPass renderPass;
VkFramebuffer* swapChainFramebuffers = NULL;
VkImage* depthImages = NULL;
VkDeviceMemory* depthImageMemorys = NULL;
VkImageView* depthImageViews = NULL;
VkImage* swapChainImages = NULL;
VkImageView* swapChainImageViews = NULL;
VkSemaphore* renderFinishedSemaphores = NULL;
VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
VkFence* imagesInFlight = NULL;
unsigned int imageCount;
unsigned int currentFrame = 0;

/* SWAP CHAIN FUNCTIONS */
void createSwapChain() {
    ZENGINE_PRINT("  - Creating swapchain KHR\n");
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface_, &swapChainSupport.capabilities);
    VkSurfaceFormatKHR surfaceFormat = swapChainSupport.formats[0];
    for (unsigned int i = 1; i < swapChainSupport.formatsSize; i++) {
        if (swapChainSupport.formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swapChainSupport.formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = swapChainSupport.formats[i];
            break;
        }
    }

    windowExtent = swapChainSupport.capabilities.currentExtent;
    imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) { imageCount = swapChainSupport.capabilities.maxImageCount; }

    VkSwapchainCreateInfoKHR createInfo = {(VkStructureType)0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface_;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = windowExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;
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

    ZENGINE_THROW(vkCreateSwapchainKHR(device_, &createInfo, NULL, &swapChain));
    ZENGINE_THROW(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, NULL));
    swapChainImages = (VkImage*)malloc(imageCount * sizeof(VkImage));
    ZENGINE_THROW(vkGetSwapchainImagesKHR(device_, swapChain, &imageCount, swapChainImages));
    swapChainImageFormat = surfaceFormat.format;

    ZENGINE_PRINT("  - Creating image views\n");
    swapChainImageViews = (VkImageView*)malloc(imageCount * sizeof(VkImageView));

    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {(VkStructureType)0};
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
    VkRenderPassCreateInfo renderPassInfo = {(VkStructureType)0};
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
        VkImageCreateInfo imageInfo = {(VkStructureType)0};
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

        VkImageViewCreateInfo viewInfo = {(VkStructureType)0};
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

        VkFramebufferCreateInfo framebufferInfo = {(VkStructureType)0};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = windowExtent.width;
        framebufferInfo.height = windowExtent.height;
        framebufferInfo.layers = 1;
        vkCreateFramebuffer(device_, &framebufferInfo, NULL, &swapChainFramebuffers[i]);
    }

    VkSemaphoreCreateInfo semaphoreInfo = {(VkStructureType)0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {(VkStructureType)0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (unsigned char i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkCreateSemaphore(device_, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]);
        vkCreateFence(device_, &fenceInfo, NULL, &inFlightFences[i]);
    }
    renderFinishedSemaphores = (VkSemaphore*)malloc(imageCount * sizeof(VkSemaphore));
    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
        vkCreateSemaphore(device_, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]);
    }
}

void deleteSwapChain() {
    for (unsigned char i = 0; i < (unsigned char)imageCount; i++) {
        vkDestroyFramebuffer(device_, swapChainFramebuffers[i], NULL);
        vkDestroyImageView(device_, depthImageViews[i], NULL);
        vkDestroyImage(device_, depthImages[i], NULL);
        vkFreeMemory(device_, depthImageMemorys[i], NULL);
        vkDestroyImageView(device_, swapChainImageViews[i], NULL);
        vkDestroySemaphore(device_, renderFinishedSemaphores[i], NULL);
    }
    for (unsigned char i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device_, imageAvailableSemaphores[i], NULL);
        vkDestroyFence(device_, inFlightFences[i], NULL);
    }

    vkDestroyRenderPass(device_, renderPass, NULL);
    if (!ZEngineClose) vkDestroySwapchainKHR(device_, swapChain, NULL);

    oldSwapChain = VK_NULL_HANDLE;
    free(swapChainImages);
    free(swapChainFramebuffers);
    free(depthImageViews);
    free(depthImages);
    free(depthImageMemorys);
    free(swapChainImageViews);
    free(imagesInFlight);
    free(renderFinishedSemaphores);
}

/* TEXTURE FUNCTIONS */
void deleteTexture(unsigned int index) {
    if (spriteTextures[index].loaded) {
        vkDeviceWaitIdle(device_);
        vkDestroyImageView(device_, spriteTextures[index].view, NULL);
        vkDestroyImage(device_, spriteTextures[index].image, NULL);
        vkFreeMemory(device_, spriteTextures[index].memory, NULL);
    }
}

void createTexture(const char* filepath, float opacity, unsigned int index) {
    int width = 0; int height = 0;
    unsigned char* pixels = stbi_load(filepath, &width, &height, NULL, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * 4;

    if (opacity != 1.f) {
        for (unsigned int i = 3; i < (unsigned int)imageSize; i += 4) { pixels[i] *= opacity; }
    }

    createTextureExt(pixels, index, width, height, imageSize, VK_FORMAT_R8G8B8A8_SRGB);
    stbi_image_free(pixels);
}

void createTextureExt(const unsigned char* data, unsigned int index, unsigned int width, unsigned int height, VkDeviceSize imageSize, VkFormat format) {
#ifdef ZENGINE_DEBUG
    if (index > ZENGINE_MAX_TEXTURES - 1) {
        ZENGINE_PRINT("The texture you are trying to load exceeds the texture limit! Please make sure -DZENGINE_MAX_TEXTURES is higher than the loaded index!");
        exit(1);
    }
#endif

    deleteTexture(index);
    spriteTextures[index].loaded = 1;

    textureBufferInfo.size = imageSize;
    vkCreateBuffer(device_, &textureBufferInfo, NULL, &spriteTextures[index].buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, spriteTextures[index].buffer, &memRequirements);

    textureMoreAllocInfo.allocationSize = memRequirements.size;
    textureMoreAllocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    vkAllocateMemory(device_, &textureMoreAllocInfo, NULL, &spriteTextures[index].bufferMemory);
    vkBindBufferMemory(device_, spriteTextures[index].buffer, spriteTextures[index].bufferMemory, 0);

    void* mem;
    vkMapMemory(device_, spriteTextures[index].bufferMemory, 0, imageSize, 0, &mem);
    memcpy(mem, data, imageSize);
    vkUnmapMemory(device_, spriteTextures[index].bufferMemory);

    textureImageInfo.extent.width = width;
    textureImageInfo.extent.height = height;
    textureImageInfo.format = format;

    textureRegion.imageExtent.width = width;
    textureRegion.imageExtent.height = height;

    createImageWithInfo(&textureImageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &spriteTextures[index].image, &spriteTextures[index].memory);

    vkBeginCommandBuffer(textureCommandBuffer, &textureBeginInfo);

    VkImageMemoryBarrier barrierToTransfer = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, NULL, 0, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, spriteTextures[index].image, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
    VkImageMemoryBarrier barrierToShaderRead = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, NULL, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, spriteTextures[index].image, {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};
    vkCmdPipelineBarrier( textureCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrierToTransfer );
    vkCmdCopyBufferToImage(textureCommandBuffer, spriteTextures[index].buffer, spriteTextures[index].image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &textureRegion);

    vkCmdPipelineBarrier( textureCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrierToShaderRead );
    vkEndCommandBuffer(textureCommandBuffer);
    vkQueueSubmit(graphicsQueue_, 1, &textureSubmitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);
    vkDestroyBuffer(device_, spriteTextures[index].buffer, NULL);
    vkFreeMemory(device_, spriteTextures[index].bufferMemory, NULL);

    textureViewInfo.image = spriteTextures[index].image;
    textureViewInfo.format = format;
    vkCreateImageView(device_, &textureViewInfo, NULL, &spriteTextures[index].view);

    textureMoreImageInfo.imageView = spriteTextures[index].view;
    textureImageWrite.dstArrayElement = index;
    vkUpdateDescriptorSets(device_, 1, &textureImageWrite, 0, NULL);
}

/* ZENIGNE HELPER FUNCTIONS */
void createBuffer(Buffer* buffer, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags) {
    buffer->bufferSize = instanceSize * instanceCount;
    buffer->mapped = NULL;

    VkBufferCreateInfo bufferInfo = {(VkStructureType)0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = buffer->bufferSize;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ZENGINE_THROW(vkCreateBuffer(device_, &bufferInfo, NULL, &buffer->buffer));

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, buffer->buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {(VkStructureType)0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

    ZENGINE_THROW(vkAllocateMemory(device_, &allocInfo, NULL, &buffer->memory));
    ZENGINE_THROW(vkBindBufferMemory(device_, buffer->buffer, buffer->memory, 0));
}

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

unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) {
    for (unsigned int i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) { return i; }
    }

    return 0;
}

void createImageWithInfo(const VkImageCreateInfo* imageInfo, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* imageMemory) {
    vkCreateImage(device_, imageInfo, NULL, image);

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device_, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {(VkStructureType)0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    vkAllocateMemory(device_, &allocInfo, NULL, imageMemory);
    vkBindImageMemory(device_, *image, *imageMemory, 0);
}

VkShaderModule createShaderModule(const char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        ZENGINE_PRINT("Failed to open shader: %s\n", filepath);
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    void* buffer = (void*)malloc(fileSize);
    if (!buffer) {
        fclose(file);
        ZENGINE_PRINT("Failed to create shader buffer!\n");
        exit(1);
    }

    fread(buffer, 1, fileSize, file);
    fclose(file);

    VkShaderModuleCreateInfo createInfo = {(VkStructureType)0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = (const unsigned int*)buffer;

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device_, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        free(buffer);
        ZENGINE_PRINT("Failed to create shader module!\n");
        exit(1);
    }

    free(buffer);
    return shaderModule;
}

Sprite* createSprite(float positionx, float positiony, float scalex, float scaley, float rotation, unsigned int textureIndex) {
    if (availibleSpriteSize == 0) {
        if (spritesSize == ZENGINE_MAX_SPRITES) { return NULL; }

        sprites[spritesSize].position[0] = positionx;
        sprites[spritesSize].position[1] = positiony;
        sprites[spritesSize].scale[0] = scalex;
        sprites[spritesSize].scale[1] = scaley;
        sprites[spritesSize].textureIndex = textureIndex;
        sprites[spritesSize].depth = .999f - ((float)spritesSize * 0.00001f);
        rotateSprite(&sprites[spritesSize], rotation);
        spritesSize++;

        return &sprites[spritesSize - 1];
    }
    else {
        availibleSpriteSize--;
        sprites[availibleSprites[availibleSpriteSize]].position[0] = positionx;
        sprites[availibleSprites[availibleSpriteSize]].position[1] = positiony;
        sprites[availibleSprites[availibleSpriteSize]].scale[0] = scalex;
        sprites[availibleSprites[availibleSpriteSize]].scale[1] = scaley;
        sprites[availibleSprites[availibleSpriteSize]].textureIndex = textureIndex;
        rotateSprite(&sprites[availibleSprites[availibleSpriteSize]], rotation);
        return &sprites[availibleSprites[availibleSpriteSize]];
    }
}

void deleteSprite(Sprite* sprite) {
    availibleSprites[availibleSpriteSize] = (unsigned int)(sprite - sprites);
    availibleSpriteSize++;
    sprite->scale[0] = 0.f;
    sprite->scale[1] = 0.f;
}

void deleteAllSprites(void) {
    spritesSize = 0;
    availibleSpriteSize = 0;
}

void rotateSprite(Sprite* sprite, float rotation) {
    sprite->rotation = rotation;
    sprite->rotationMatrix[0] = cos(rotation * .01745329f);
    sprite->rotationMatrix[2] = sin(rotation * .01745329f);
    sprite->rotationMatrix[1] = -sprite->rotationMatrix[2];
    sprite->rotationMatrix[3] = sprite->rotationMatrix[0];
}

/* vulkan validation layers */
#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
static const char* validationLayers[] = {"VK_LAYER_KHRONOS_validation"};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData) {
    (void)messageSeverity;
    (void)messageType;
    (void)userData;

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) { fprintf(stderr, "Vulkan validation error: %s\n", callbackData->pMessage); }
    else { fprintf(stderr, "Vulkan validation: %s\n", callbackData->pMessage); }

    return VK_FALSE;
}
#endif

/* ZENGINE */
void ZEngineInit() {
    ZENGINE_PRINT("Creating instance...\n"); //---------------------------------------------------------------------------------------------------------------
    VkApplicationInfo appInfo = {(VkStructureType)0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ZDev";
    appInfo.pEngineName = "ZEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.engineVersion = VK_MAKE_VERSION(1, 11, 0);
    appInfo.apiVersion =  VK_API_VERSION_1_2;

    VkInstanceCreateInfo instanceInfo = {(VkStructureType)0};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

#ifdef __APPLE__
    const char* instanceExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_MVK_macos_surface"
#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
        , VK_EXT_DEBUG_UTILS_EXTENSION_NAME /* extra extension for vulkan validation output */
#endif
    };

    instanceInfo.enabledExtensionCount = sizeof(instanceExtensions) / sizeof(instanceExtensions[0]);
    instanceInfo.ppEnabledExtensionNames = instanceExtensions;
    instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
    size_t rgfWExtensionCount = 0;
    const char** rgfWExtensions = RGFW_getRequiredInstanceExtensions_Vulkan(&rgfWExtensionCount);
    if (!rgfWExtensions || rgfWExtensionCount == 0) {
        ZENGINE_PRINT("Failed to get Vulkan extensions\n");
        exit(1);
    }

    ZENGINE_PRINT("Enabling extensions:\n");
    for (size_t i = 0; i < rgfWExtensionCount; i++) { ZENGINE_PRINT("     - %s\n", rgfWExtensions[i]); }

#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
    /* add an extra extension for vulkan validation output */
    size_t instanceExtensionCount = rgfWExtensionCount + 1;
    const char* instanceExtensions[instanceExtensionCount];
    for (size_t i = 0; i < rgfWExtensionCount; i++) { instanceExtensions[i] = rgfWExtensions[i]; }
    instanceExtensions[rgfWExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    instanceInfo.enabledExtensionCount = instanceExtensionCount;
    instanceInfo.ppEnabledExtensionNames = instanceExtensions;
#else
    instanceInfo.enabledExtensionCount = rgfWExtensionCount;
    instanceInfo.ppEnabledExtensionNames = rgfWExtensions;
#endif
#endif

#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
    /* enable the vulkan validation layers */
    instanceInfo.enabledLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);
    instanceInfo.ppEnabledLayerNames = validationLayers;

    /* attach a debug messenger to the instance creation info, to also validate instance creation/destruction */
    VkDebugUtilsMessengerCreateInfoEXT debugInfo = {(VkStructureType)0};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallback;
    instanceInfo.pNext = &debugInfo;
#endif

#ifdef ZENGINE_DEBUG
    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);
    if  (result != 0) {
        ZENGINE_PRINT("Instance creation failed - error code: %d\n", result);
    }
#else
    vkCreateInstance(&instanceInfo, NULL, &instance);
#endif

#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
    /* create a debug messenger to validate the engine during its entire lifetime */
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (vkCreateDebugUtilsMessenger == NULL) {
        fprintf(stderr, "Failed to load vkCreateDebugUtilsMessengerEXT..\n");
        exit(1);
    }

    ZENGINE_THROW(vkCreateDebugUtilsMessenger(instance, &debugInfo, NULL, &debugMessenger));
#endif

    ZENGINE_PRINT("Creating surface...\n"); RGFW_window_createSurface_Vulkan(zwindow, instance, &surface_); //---------------------------------------------------------------------------------------------------------------

    ZENGINE_PRINT("Creating physical device...\n"); //---------------------------------------------------------------------------------------------------------------
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    if (deviceCount == 0) {
        ZENGINE_PRINT("No Vulkan-compatible GPUs found!\nPlease install vulkan drivers: Intel: sudo pacman -S vulkan-intel || AMD: sudo pacman -S vulkan-radeon || If you are not on arch linux, please follow the guide in https://github.com/ZDev22/ZEngine/blob/main/README.md\n");
        exit(1);
    }
    else if (deviceCount == 1) { physicalDevice = devices[0]; }
    else {
        ZENGINE_PRINT("Found %u GPUs\n", deviceCount);
        short highScore = -1;
        for (unsigned char i = 0; i < (unsigned char)deviceCount; ++i) {
            /* check if device is suitable, and score it */
            QueueFamilyIndices indices = findQueueFamilies(devices[i]);
            unsigned int extensionCount = 0;

            /* check if extensions are supported on the GPU: */
            vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, NULL);
            VkExtensionProperties availableExtensions[extensionCount];
            vkEnumerateDeviceExtensionProperties(devices[i], NULL, &extensionCount, availableExtensions);

            SwapChainSupportDetails swapChainSupport;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[i], surface_, &swapChainSupport.capabilities);
            vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface_, &swapChainSupport.formatsSize, NULL);
            vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface_, &swapChainSupport.presentModeSize, NULL);

            unsigned short newScore = swapChainSupport.formatsSize * swapChainSupport.presentModeSize;
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
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface_, &swapChainSupport.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface_, &swapChainSupport.formatsSize, NULL);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface_, &swapChainSupport.presentModeSize, NULL);
    if (swapChainSupport.formatsSize != 0) {
        swapChainSupport.formats = (VkSurfaceFormatKHR*)malloc(swapChainSupport.formatsSize * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface_, &swapChainSupport.formatsSize, swapChainSupport.formats);
    }
    if (swapChainSupport.presentModeSize != 0) {
        swapChainSupport.presentModes = (VkPresentModeKHR*)malloc(swapChainSupport.presentModeSize * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface_, &swapChainSupport.presentModeSize, swapChainSupport.presentModes);
    }

    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
	    uniformBuffer = 1;
    }

#ifdef ZENGINE_DEBUG
    ZENGINE_PRINT("Selected GPU: %s\n", properties.deviceName);
    ZENGINE_PRINT("  - Swapchain formats: %u, presentmodes: %u\n", swapChainSupport.formatsSize, swapChainSupport.presentModeSize);
    for (unsigned char i = 0; i < (unsigned char)swapChainSupport.presentModeSize; i++) {
        if (swapChainSupport.presentModes[i] == 0) { ZENGINE_PRINT("  - GPU supports VSync\n"); }
        else if (swapChainSupport.presentModes[i] == 2) { ZENGINE_PRINT("  - GPU can disable VSync\n"); }
    }
#endif

    ZENGINE_PRINT("Creating logical device...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    unsigned char queueFamilySize = 1;
    if (indices.presentFamily != indices.graphicsFamily) { queueFamilySize = 2; }

    unsigned int uniqueQueueFamilies[queueFamilySize];
    uniqueQueueFamilies[0] = indices.graphicsFamily;
    if (queueFamilySize == 2) { uniqueQueueFamilies[1] = indices.presentFamily; }

    VkDeviceQueueCreateInfo queueCreateInfos[queueFamilySize];
    float queuePriorities = 1.f;
    for (unsigned char i = 0; i < queueFamilySize; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {(VkStructureType)0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriorities;
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkPhysicalDeviceVulkan12Features deviceVulkanUpdate = {0};
    deviceVulkanUpdate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    deviceVulkanUpdate.runtimeDescriptorArray = VK_TRUE;
    deviceVulkanUpdate.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

    VkDeviceCreateInfo deviceInfo = {(VkStructureType)0};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = &deviceVulkanUpdate;
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
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
    vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);

    ZENGINE_PRINT("Creating command pool...\n"); //---------------------------------------------------------------------------------------------------------------
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
    VkCommandPoolCreateInfo commandPoolInfo = {(VkStructureType)0};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    ZENGINE_THROW(vkCreateCommandPool(device_, &commandPoolInfo, NULL, &commandPool));

    ZENGINE_PRINT("Creating swapChain...\n"); //---------------------------------------------------------------------------------------------------------------
    createSwapChain();
    ZENGINE_PRINT("Creating command buffers...\n");
    commandBuffers = (VkCommandBuffer*)malloc(imageCount * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo commandBufferInfo = {(VkStructureType)0};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandPool = commandPool;
    commandBufferInfo.commandBufferCount = imageCount;
    vkAllocateCommandBuffers(device_, &commandBufferInfo, commandBuffers);

    ZENGINE_PRINT("Initializing shaders...\n");
    VkPipelineShaderStageCreateInfo shaderStages[2] = {(VkStructureType)0};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = createShaderModule("shaders/z.vert.spv");
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = createShaderModule("shaders/z.frag.spv");
    shaderStages[1].pName = "main";

    ZENGINE_PRINT("Rasterizing pipeline data...\n");
    VkVertexInputBindingDescription bindingDescription = {0};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription inputAttributes[2];
    inputAttributes[0].binding = 0;
    inputAttributes[0].location = 0;
    inputAttributes[0].format = VK_FORMAT_R32G32_SFLOAT;
    inputAttributes[0].offset = 0;
    inputAttributes[1].binding = 0;
    inputAttributes[1].location = 1;
    inputAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    inputAttributes[1].offset = 8;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {(VkStructureType)0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = inputAttributes;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {(VkStructureType)0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    VkPipelineViewportStateCreateInfo viewportState = {(VkStructureType)0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer = {(VkStructureType)0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.lineWidth = 1.f;

    VkPipelineMultisampleStateCreateInfo multisampling = {(VkStructureType)0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {(VkStructureType)0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending = {(VkStructureType)0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {(VkStructureType)0};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {(VkStructureType)0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    ZENGINE_PRINT("Creating pipeline layout...\n");
    VkDescriptorSetLayoutBinding layoutBindings[2] = {(VkStructureType)0};
    layoutBindings[0].binding = 0;
    if (uniformBuffer) { layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
    else { layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layoutBindings[1].descriptorCount = ZENGINE_MAX_TEXTURES;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {(VkStructureType)0};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = layoutBindings;
    ZENGINE_THROW(vkCreateDescriptorSetLayout(device_, &layoutInfo, NULL, &descriptorSetLayout));

    VkDescriptorPoolSize poolSizes[2] = {{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}, {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ZENGINE_MAX_TEXTURES * (ZENGINE_MAX_TEXTURES + 1)}};
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {(VkStructureType)0};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolInfo.poolSizeCount = 2;
    descriptorPoolInfo.pPoolSizes = poolSizes;
    descriptorPoolInfo.maxSets = ZENGINE_MAX_TEXTURES + 1;
    ZENGINE_THROW(vkCreateDescriptorPool(device_, &descriptorPoolInfo, NULL, &descriptorPool));

    VkPushConstantRange pipelinePushConstantRange = {VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT, 0, 20};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {(VkStructureType)0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pipelinePushConstantRange;
    ZENGINE_THROW(vkCreatePipelineLayout(device_, &pipelineLayoutInfo, NULL, &pipelineLayout));

    VkGraphicsPipelineCreateInfo pipelineInfo = {(VkStructureType)0};
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
    ZENGINE_THROW(vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline)); // WINDOWS ERROR

    vkDestroyShaderModule(device_, shaderStages[0].module, NULL);
    vkDestroyShaderModule(device_, shaderStages[1].module, NULL);

    ZENGINE_PRINT("Initing textures...\n");
    VkDescriptorSetAllocateInfo allocInfo = {(VkStructureType)0};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;
    ZENGINE_THROW(vkAllocateDescriptorSets(device_, &allocInfo, &spriteDataDescriptorSet));

    textureBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    textureBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    textureBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    textureAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    textureAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    textureAllocInfo.commandPool = commandPool;
    textureAllocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(device_, &textureAllocInfo, &textureCommandBuffer);

    textureMoreAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

    textureImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    textureImageInfo.imageType = VK_IMAGE_TYPE_2D;
    textureImageInfo.extent.depth = 1;
    textureImageInfo.mipLevels = 1;
    textureImageInfo.arrayLayers = 1;
    textureImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    textureImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    textureImageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    textureImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    textureImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    textureRegion.bufferOffset = 0;
    textureRegion.bufferRowLength = 0;
    textureRegion.bufferImageHeight = 0;
    textureRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    textureRegion.imageSubresource.mipLevel = 0;
    textureRegion.imageSubresource.baseArrayLayer = 0;
    textureRegion.imageSubresource.layerCount = 1;
    textureRegion.imageExtent.depth = 1;

    textureBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    textureBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    textureSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    textureSubmitInfo.commandBufferCount = 1;
    textureSubmitInfo.pCommandBuffers = &textureCommandBuffer;

    textureViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    textureViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    textureViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    textureViewInfo.subresourceRange.baseMipLevel = 0;
    textureViewInfo.subresourceRange.levelCount = 1;
    textureViewInfo.subresourceRange.baseArrayLayer = 0;
    textureViewInfo.subresourceRange.layerCount = 1;

    VkSamplerCreateInfo textureSamplerInfo = {(VkStructureType)0};
    textureSamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    textureSamplerInfo.magFilter = VK_FILTER_NEAREST;
    textureSamplerInfo.minFilter = VK_FILTER_NEAREST;
    textureSamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    textureSamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    textureSamplerInfo.anisotropyEnable = VK_FALSE;
    textureSamplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
    textureSamplerInfo.unnormalizedCoordinates = VK_FALSE;
    textureSamplerInfo.compareEnable = VK_FALSE;
    textureSamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    textureSamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    vkCreateSampler(device_, &textureSamplerInfo, NULL, &spriteTextures[0].sampler);

    textureMoreImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    textureMoreImageInfo.sampler = spriteTextures[0].sampler;

    textureImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    textureImageWrite.dstSet = spriteDataDescriptorSet;
    textureImageWrite.dstBinding = 1;
    textureImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureImageWrite.descriptorCount = 1;
    textureImageWrite.pImageInfo = &textureMoreImageInfo;

    ZENGINE_PRINT("Initing sprites...\n");
    camera.zoom[0]     = 1.f; camera.zoom[1]     = 1.f;
    camera.position[0] = 0.f; camera.position[1] = 0.f;
    camera.aspect      = (float)windowExtent.width / (float)windowExtent.height;

    /* load zmodel */
    zmodel.vertices[0].pos[0] = -.5f; zmodel.vertices[0].pos[1] = -.5f;
    zmodel.vertices[0].cord[0] = 0.f; zmodel.vertices[0].cord[1] = 0.f;
    zmodel.vertices[1].pos[0] = .5f;  zmodel.vertices[1].pos[1] = -.5f;
    zmodel.vertices[1].cord[0] = 1.f; zmodel.vertices[1].cord[1] = 0.f;
    zmodel.vertices[2].pos[0] = -.5f; zmodel.vertices[2].pos[1] = .5f;
    zmodel.vertices[2].cord[0] = 0.f; zmodel.vertices[2].cord[1] = 1.f;
    zmodel.vertices[3].pos[0] = .5f;  zmodel.vertices[3].pos[1] = .5f;
    zmodel.vertices[3].cord[0] = 1.f; zmodel.vertices[3].cord[1] = 1.f;

    createBuffer(&zmodel.vertexBuffer, sizeof(Vertex) * 4, 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vkMapMemory(device_, zmodel.vertexBuffer.memory, 0, zmodel.vertexBuffer.bufferSize, 0, &zmodel.vertexBuffer.mapped);
    memcpy(zmodel.vertexBuffer.mapped, (const void*)zmodel.vertices, (unsigned int)(sizeof(Vertex) * 4));
    vkUnmapMemory(device_, zmodel.vertexBuffer.memory);

    if (uniformBuffer) { createBuffer(&spriteDataBuffer, sizeof(Sprite) * ZENGINE_MAX_SPRITES, 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); }
    else { createBuffer(&spriteDataBuffer, sizeof(Sprite) * ZENGINE_MAX_SPRITES, 1, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); }
    vkMapMemory(device_, spriteDataBuffer.memory, 0, spriteDataBuffer.bufferSize, 0, &spriteDataBuffer.mapped);

    /* allocate info */
    VkDescriptorBufferInfo bufferInfo = {0};
    bufferInfo.buffer = spriteDataBuffer.buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(Sprite) * ZENGINE_MAX_SPRITES;

    ZENGINE_PRINT("Writing sprite descriptor sets...\n");
    VkWriteDescriptorSet bufferWrite = {(VkStructureType)0};
    bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    bufferWrite.dstSet = spriteDataDescriptorSet;
    bufferWrite.dstBinding = 0;
    bufferWrite.dstArrayElement = 0;
    if (uniformBuffer) { bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; }
    else { bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; }
    bufferWrite.descriptorCount = 1;
    bufferWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device_, 1, &bufferWrite, 0, NULL);
    ZENGINE_PRINT("ZEngine successfuly initialized!\n");
}

void ZEngineRender() {
    vkWaitForFences(device_, 1, &inFlightFences[currentFrame], VK_TRUE, 0xFFFFFFFFFFFFFFFF);
    /* resize window */
    VkResult result = vkAcquireNextImageKHR(device_, swapChain, 0xFFFFFFFFFFFFFFFF, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        /* recreate swapchain */
        vkDeviceWaitIdle(device_);
        oldSwapChain = swapChain;
        deleteSwapChain();
        createSwapChain();

        framebufferResized = 0;
        camera.aspect = (float)windowExtent.width / (float)windowExtent.height;
        return;
    }

    /* create command buffer */
    VkCommandBufferBeginInfo beginInfo = {(VkStructureType)0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    VkCommandBuffer commandBuffer = commandBuffers[currentImageIndex];
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    /* begin render pass */
    VkClearValue clearValues[2] = {0};
    clearValues[1].depthStencil = (VkClearDepthStencilValue){ 1.f, 0 };

    VkRenderPassBeginInfo renderPassInfo = {(VkStructureType)0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[currentImageIndex];
    renderPassInfo.renderArea.extent = windowExtent;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = windowExtent;

    VkViewport viewport = {0.f, 0.f, (float)windowExtent.width, (float)windowExtent.height, 0.f, 1.f};
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    /* render sprites */
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &spriteDataDescriptorSet, 0, NULL);
    vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Camera), &camera);

#ifdef ZENGINE_SPRITE_MAPMODE_MANUAL
    if (ZEngineSpriteRemap) {
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

    if (imagesInFlight[currentImageIndex] != VK_NULL_HANDLE) { vkWaitForFences(device_, 1, &imagesInFlight[currentImageIndex], VK_TRUE, 0xFFFFFFFFFFFFFFFF); }
    imagesInFlight[currentImageIndex] = inFlightFences[currentFrame];

    const VkSemaphore waitSemaphores[1] = { imageAvailableSemaphores[currentFrame] };
    const VkPipelineStageFlags waitStages[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    const VkSemaphore signalSemaphores[1] = { renderFinishedSemaphores[currentImageIndex] };

    VkSubmitInfo submitInfo = {(VkStructureType)0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device_, 1, &inFlightFences[currentFrame]);
    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFences[currentFrame]);

    VkPresentInfoKHR presentInfo = {(VkStructureType)0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapChain;
    presentInfo.pImageIndices = &currentImageIndex;

    vkQueuePresentKHR(presentQueue_, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void ZEngineDeinit() {
    ZEngineClose = 1;
    ZENGINE_PRINT("Waiting for GPU...\n"); vkDeviceWaitIdle(device_);

    ZENGINE_PRINT("Freeing command buffers\n"); vkFreeCommandBuffers(device_, commandPool, imageCount, commandBuffers);
    ZENGINE_PRINT("Clearing command buffers\n"); free(commandBuffers);

    ZENGINE_PRINT("Freeing graphics pipeline\n"); vkDestroyPipeline(device_, graphicsPipeline, NULL);
    ZENGINE_PRINT("Freeing pipeline layout\n");   vkDestroyPipelineLayout(device_, pipelineLayout, NULL);
    ZENGINE_PRINT("Freeing discriptor pool\n");   vkFreeDescriptorSets(device_, descriptorPool, 1, &spriteDataDescriptorSet);

    ZENGINE_PRINT("Freeing textures\n");
    for (unsigned int i = 0; i < ZENGINE_MAX_TEXTURES; i++) { deleteTexture(i); }
    vkDestroySampler(device_, spriteTextures[0].sampler, NULL);
    vkFreeCommandBuffers(device_, commandPool, 1, &textureCommandBuffer);

    ZENGINE_PRINT("Freeing buffers\n");
    vkDestroyBuffer(device_, zmodel.vertexBuffer.buffer, NULL);
    vkDestroyBuffer(device_, spriteDataBuffer.buffer, NULL);
    vkFreeMemory(device_, spriteDataBuffer.memory, NULL);
    vkFreeMemory(device_, zmodel.vertexBuffer.memory, NULL);
    spriteDataBuffer.buffer = VK_NULL_HANDLE;
    zmodel.vertexBuffer.buffer = VK_NULL_HANDLE;
    spriteDataBuffer.memory = VK_NULL_HANDLE;
    zmodel.vertexBuffer.memory = VK_NULL_HANDLE;

    ZENGINE_PRINT("Freeing swapchain\n"); deleteSwapChain();
    free(swapChainSupport.formats);
    free(swapChainSupport.presentModes);

    ZENGINE_PRINT("Freeing descriptor set layout\n"); vkDestroyDescriptorSetLayout(device_, descriptorSetLayout, NULL);
    ZENGINE_PRINT("Freeing descriptor pool\n");       vkDestroyDescriptorPool(device_, descriptorPool, NULL);
    ZENGINE_PRINT("Freeing command pool\n");          vkDestroyCommandPool(device_, commandPool, NULL);
    vkDestroySwapchainKHR(device_, swapChain, NULL);
    vkDestroySwapchainKHR(device_, oldSwapChain, NULL);
    ZENGINE_PRINT("Destroying device\n");             vkDestroyDevice(device_, NULL);
    ZENGINE_PRINT("Freeing window surface\n");        vkDestroySurfaceKHR(instance, surface_, NULL);
#ifdef ZENGINE_ENABLE_VALIDATION_LAYERS
    ZENGINE_PRINT("Destroying debug messenger\n");
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (vkDestroyDebugUtilsMessenger != NULL) { vkDestroyDebugUtilsMessenger(instance, debugMessenger, NULL); }
#endif
    ZENGINE_PRINT("Destroying instance\n");           vkDestroyInstance(instance, NULL);
}

#undef ZENGINE_IMPLEMENTATION
#endif // ZENGINE_IMPLEMENTATION

#ifdef __cplusplus
    }
#endif

#endif // ZENGINE_H
