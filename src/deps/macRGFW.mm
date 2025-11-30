#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

#include "RGFW.h"

extern "C" {
VkResult RGFW_CreateVulkanSurface(RGFW_window* win, VkInstance instance, VkSurfaceKHR* surface, void* views) {
    NSView* view = (NSView*)views;

    [view setWantsLayer:YES];

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    metalLayer.device = MTLCreateSystemDefaultDevice();
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.contentsScale = view.window.backingScaleFactor;
    metalLayer.frame = view.bounds;

    [view setLayer:metalLayer];

    VkMacOSSurfaceCreateInfoMVK createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.pView = (__bridge void*)view;

    return vkCreateMacOSSurfaceMVK(instance, &createInfo, nullptr, surface);
}
}