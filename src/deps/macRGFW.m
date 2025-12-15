#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_macos.h>

#include "RGFW.h"

#ifdef __cplusplus
extern "C" {
#endif
VkResult RGFW_CreateVulkanSurface(RGFW_window* win, VkInstance instance, VkSurfaceKHR* surface, void* view /*Take this in because all we have access to is forward declarations*/) {
    NSView* nsview = (NSView*)view;
    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    metalLayer.device = MTLCreateSystemDefaultDevice();
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = NO;

    nsview.wantsLayer = YES;
    nsview.layer = metalLayer;

    VkMacOSSurfaceCreateInfoMVK createInfo = { VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK, NULL, 0, nsview};
    return vkCreateMacOSSurfaceMVK(instance, &createInfo, NULL, surface);
}
#ifdef __cplusplus
}
#endif