#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include "device.hpp"

#include <cstring>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <set>
#include <stdexcept>

#include <vulkan/vulkan.h>

using namespace std;

bool enableValidationLayers = true;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    std::cerr << "[Validation Layer] " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func) func(instance, debugMessenger, pAllocator);
}

Device::Device(AppWindow& window) : window{ window } {
    cout << "[Device] Creating instance..." << std::endl; createInstance();
    cout << "[Device] Creating debug messenger..." << std::endl; setupDebugMessenger();
    cout << "[Device] Creating surface..." << std::endl; createSurface();
    cout << "[Device] Creating physical device..." << std::endl; pickPhysicalDevice();
    cout << "[Device] Creating logical device..." << std::endl; createLogicalDevice();
    cout << "[Device] Creating command pool..." << std::endl; createCommandPool();
    cout << "[Device] Device constructed!" << std::endl;
}

Device::~Device() {
    cout << "[Device] Destroying command pool..." << std::endl;
    vkDestroyCommandPool(device_, commandPool, nullptr);
    cout << "[Device] Destroying logical device..." << std::endl;
    vkDestroyDevice(device_, nullptr);
    if (enableValidationLayers && debugMessenger != VK_NULL_HANDLE) {
        cout << "[Device] Destroying debug messenger..." << std::endl;
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    cout << "[Device] Destroying surface..." << std::endl;
    vkDestroySurfaceKHR(instance, surface_, nullptr);
    cout << "[Device] Destroying instance..." << std::endl;
    vkDestroyInstance(instance, nullptr);
    cout << "[Device] Device destroyed!" << std::endl;
}

void Device::createInstance() {
    cout << "[Device] instance function called" << std::endl;
    if (enableValidationLayers) {
        cout << "[Device] Checking validation support" << std::endl;
        if (!checkValidationLayerSupport()) {
            std::cerr << "[Device] Warning: Validation layers requested but not available. Continuing without them." << std::endl;
            enableValidationLayers = false;
        } else {
            cout << "[Device] Validation layer check successful" << std::endl;
        }
    }
    cout << "[Device] Creating app info..." << std::endl;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanTest";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "ZEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    cout << "[Device] Creating instance info..." << std::endl;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    cout << "[Device] Getting extensions..." << std::endl;
    auto extensions = getRequiredExtensions();
    cout << "[Device] Enabling Vulkan instance extensions (" << extensions.size() << "):" << std::endl;
    for (const auto* ext : extensions) {
        cout << "  - " << (ext ? ext : "<nullptr>") << endl;
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        cout << "[Device] Enabling validation layers (" << validationLayers.size() << "):" << std::endl;
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        for (const char* layer : validationLayers) {
            cout << "  - " << layer << endl;
        }
        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }
    cout << "[Device] Creating Vulkan instance (vkCreateInstance)..." << std::endl;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("[Device] Failed to create Vulkan instance");
    }
    cout << "[Device] Vulkan instance created successfully!" << std::endl;
    hasGflwRequiredInstanceExtensions();
    cout << "[Device] createInstance finished" << std::endl;
}

void Device::setupDebugMessenger() {
    if (!enableValidationLayers) {
        cout << "[Device] Validation layers disabled, skipping debug messenger" << std::endl;
        return;
    }
    cout << "[Device] Setting up debug messenger..." << std::endl;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        std::cerr << "[Device] Warning: Failed to create debug messenger." << std::endl;
        debugMessenger = VK_NULL_HANDLE;
    } else {
        cout << "[Device] Debug messenger created!" << std::endl;
    }
}

void Device::createSurface() {
    cout << "[Device] Creating window surface..." << std::endl;
    window.createWindowSurface(instance, &surface_);
    cout << "[Device] Window surface created!" << std::endl;
}

void Device::pickPhysicalDevice() {
    cout << "[Device] Enumerating physical devices..." << std::endl;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    cout << "[Device] Found " << deviceCount << " physical device(s)" << std::endl;

    if (deviceCount == 0) {
        throw std::runtime_error("[Device] No Vulkan-compatible GPUs found");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (size_t i = 0; i < devices.size(); ++i) {
        cout << "[Device] Checking device " << i << " for suitability..." << std::endl;
        if (isDeviceSuitable(devices[i])) {
            physicalDevice = devices[i];
            cout << "[Device] Suitable device found at index " << i << "" << std::endl;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        std::cerr << "[Device] Warning: No suitable GPU found, using first available" << std::endl;
        physicalDevice = devices[0];
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    std::cout << "[Device] Selected GPU: " << properties.deviceName << std::endl;
}

void Device::createLogicalDevice() {
    cout << "[Device] Finding queue families..." << std::endl;
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        cout << "[Device] Queue family: " << queueFamily << endl;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    std::vector<const char*> updatedDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    cout << "[Device] Device extensions to be enabled (" << updatedDeviceExtensions.size() << "):" << std::endl;
    for (const auto* ext : updatedDeviceExtensions) {
        cout << "  - " << ext << endl;
    }
    createInfo.enabledExtensionCount = static_cast<uint32_t>(updatedDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = updatedDeviceExtensions.data();

    if (enableValidationLayers) {
        cout << "[Device] Enabling validation layers for logical device (" << validationLayers.size() << "):" << std::endl;
        for (const char* layer : validationLayers) {
            cout << "  - " << layer << endl;
        }
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    cout << "[Device] Creating logical device..." << std::endl;
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device_) != VK_SUCCESS) {
        throw std::runtime_error("[Device] failed to create logical device!");
    }
    cout << "[Device] Logical device created!" << std::endl;
    vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
    cout << "[Device] Graphics queue obtained." << std::endl;
    vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
    cout << "[Device] Present queue obtained." << std::endl;
}

void Device::createCommandPool() {
    cout << "[Device] Finding physical queue families for command pool..." << std::endl;
    QueueFamilyIndices queueFamilyIndices = findPhysicalQueueFamilies();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    cout << "[Device] Creating command pool..." << std::endl;
    if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("[Device] failed to create command pool!");
    }
    cout << "[Device] Command pool created!" << std::endl;
}

bool Device::isDeviceSuitable(VkPhysicalDevice device) {
    cout << "[Device] Checking device suitability..." << std::endl;
    QueueFamilyIndices indices = findQueueFamilies(device);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        cout << "[Device] Device supports required extensions." << std::endl;
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        cout << "[Device] SwapChain formats: " << swapChainSupport.formats.size()
            << ", present modes: " << swapChainSupport.presentModes.size() << endl;
    }
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    bool suitable = indices.isComplete() && extensionsSupported && swapChainAdequate;
    cout << "[Device] Device suitability: " << suitable << endl;
    return suitable;
}

bool Device::checkValidationLayerSupport() {
    uint32_t layerCount;
    cout << "[Device] Calling 'vkEnumerateInstanceLayerProperties'" << std::endl;
    VkResult result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    if (result != VK_SUCCESS) {
        std::cerr << "[Device] Failed to enumerate instance layer properties: " << result << std::endl;
        return false;
    }

    std::vector<VkLayerProperties> availableLayers(layerCount);
    cout << "[Device] Calling 'vkEnumerateInstanceLayerProperties' (again)" << std::endl;
    result = vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    if (result != VK_SUCCESS) {
        std::cerr << "[Device] Failed to enumerate instance layer properties: " << result << std::endl;
        return false;
    }

    std::cout << "[Device] Available validation layers (" << layerCount << "):" << std::endl;
    for (const auto& layerProperties : availableLayers) {
        std::cout << "  - " << layerProperties.layerName << std::endl;
    }

    if (validationLayers.empty()) {
        std::cerr << "[Device] No validation layers requested." << std::endl;
        return false;
    }

    std::cout << "[Device] Requested validation layers:" << std::endl;
    for (const char* layerName : validationLayers) {
        if (!layerName) {
            std::cerr << "[Device]  - Invalid (null) layer name!" << std::endl;
            return false;
        }
        std::cout << "  - " << layerName << std::endl;
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            std::cout << "[Device] Validation layer not found: " << layerName << std::endl;
            return false;
        }
    }
    std::cout << "[Device] All requested validation layers are supported." << std::endl;
    return true;
}

std::vector<const char*> Device::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (!glfwExtensions || glfwExtensionCount == 0) {
        std::cerr << "[Device] ERROR: GLFW failed to return required Vulkan instance extensions!" << std::endl;
        std::cerr << "[Device]  - Is the Vulkan SDK installed?" << std::endl;
        std::cerr << "[Device]  - Are your graphics drivers up to date?" << std::endl;
        std::cerr << "[Device]  - Does your system support Vulkan?" << std::endl;
        throw std::runtime_error("[Device] GLFW: Failed to get required Vulkan instance extensions.");
    }

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    std::cout << "[Device] Extensions found via GLFW (" << extensions.size() << "):" << std::endl;
    for (const auto* ext : extensions) {
        std::cout << "  - " << (ext ? ext : "<nullptr>") << std::endl;
    }

    return extensions;
}

void Device::hasGflwRequiredInstanceExtensions() {
    cout << "[Device] Checking GLFW required instance extensions..." << std::endl;
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::unordered_set<std::string> available;
    for (const auto& ext : extensions) {
        available.insert(ext.extensionName);
    }

    for (const auto& required : getRequiredExtensions()) {
        if (available.find(required) == available.end()) {
            std::cerr << "[Device] Warning: Missing required GLFW extension: " << required << std::endl;
        } else {
            cout << "[Device] Found required GLFW extension: " << required << endl;
        }
    }
    cout << "[Device] Checked all GLFW required instance extensions." << std::endl;
}

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
    cout << "[Device] Checking device extension support..." << std::endl;
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::vector<const char*> updatedDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    std::set<std::string> requiredExtensions(updatedDeviceExtensions.begin(), updatedDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        cout << "[Device] Device extension found: " << extension.extensionName << endl;
        requiredExtensions.erase(extension.extensionName);
    }
    cout << "[Device] Device extensions still required: " << requiredExtensions.size() << endl;
    for (const auto& ext : requiredExtensions) {
        cout << "  - " << ext << endl;
    }
    bool result = requiredExtensions.empty();
    cout << "[Device] Device extension support result: " << result << endl;
    return result;
}

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) {
    cout << "[Device] Finding queue families..." << std::endl;
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        cout << "[Device] Checking queue family " << i << ": queueCount=" << queueFamily.queueCount << " flags=" << queueFamily.queueFlags << endl;
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = true;
            cout << "[Device] Graphics family found at index " << i << endl;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = true;
            cout << "[Device] Present family found at index " << i << endl;
        }
        if (indices.isComplete()) break;
        i++;
    }
    cout << "[Device] Queue family search complete" << std::endl;
    return indices;
}

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) {
    cout << "[Device] Querying swapchain support..." << std::endl;
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);
    cout << "[Device] Swapchain format count: " << formatCount << endl;
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
    }
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);
    cout << "[Device] Swapchain present mode count: " << presentModeCount << endl;
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, details.presentModes.data());
    }
    cout << "[Device] Swapchain support queried" << std::endl;
    return details;
}

VkFormat Device::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
    cout << "[Device] Finding supported format..." << std::endl;
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            cout << "[Device] Supported format found (linear): " << format << endl;
            return format;
        }
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            cout << "[Device] Supported format found (optimal): " << format << endl;
            return format;
        }
    }
    throw std::runtime_error("[Device] failed to find supported format!");
}

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    cout << "[Device] Finding memory type..." << std::endl;
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            cout << "[Device] Found memory type: " << i << endl;
            return i;
        }
    }

    throw std::runtime_error("[Device] failed to find suitable memory type!");
}

VkResult Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    cout << "[Device] Creating buffer, size = " << size << endl;
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("[Device] failed to create buffer! VkResult: " + std::to_string(result));
    }
    cout << "[Device] Buffer created." << std::endl;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);
    cout << "[Device] Buffer memory requirements - size: " << memRequirements.size << ", typeBits: " << memRequirements.memoryTypeBits << endl;

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory);
    if (result != VK_SUCCESS) {
        vkDestroyBuffer(device_, buffer, nullptr);
        throw std::runtime_error("[Device] failed to allocate buffer memory! VkResult: " + std::to_string(result));
    }
    cout << "[Device] Buffer memory allocated." << std::endl;

    result = vkBindBufferMemory(device_, buffer, bufferMemory, 0);
    if (result != VK_SUCCESS) {
        vkDestroyBuffer(device_, buffer, nullptr);
        vkFreeMemory(device_, bufferMemory, nullptr);
        throw std::runtime_error("[Device] failed to bind buffer memory! VkResult: " + std::to_string(result));
    }
    cout << "[Device] Buffer memory bound." << std::endl;

    return VK_SUCCESS;
}

VkCommandBuffer Device::beginSingleTimeCommands() {
    cout << "[Device] beginSingleTimeCommands" << std::endl;
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
    cout << "[Device] Single time command buffer begun" << std::endl;
    return commandBuffer;
}

void Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    cout << "[Device] endSingleTimeCommands" << std::endl;
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue_);

    vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
    cout << "[Device] Single time command buffer ended and freed" << std::endl;
}

void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    cout << "[Device] copyBuffer - size: " << size << endl;
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();
    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    endSingleTimeCommands(commandBuffer);
    cout << "[Device] copyBuffer complete" << std::endl;
}

void Device::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) {
    cout << "[Device] copyBufferToImage - w: " << width << ", h: " << height << ", layers: " << layerCount << endl;
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
    cout << "[Device] copyBufferToImage complete" << std::endl;
}

VkResult Device::createImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
    cout << "[Device] createImageWithInfo" << std::endl;
    VkResult result = vkCreateImage(device_, &imageInfo, nullptr, &image);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("[Device] failed to create image! VkResult: " + std::to_string(result));
    }
    cout << "[Device] Image created." << std::endl;

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device_, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory);
    if (result != VK_SUCCESS) {
        vkDestroyImage(device_, image, nullptr);
        throw std::runtime_error("[Device] failed to allocate image memory! VkResult: " + std::to_string(result));
    }

    result = vkBindImageMemory(device_, image, imageMemory, 0);
    if (result != VK_SUCCESS) {
        vkDestroyImage(device_, image, nullptr);
        vkFreeMemory(device_, imageMemory, nullptr);
        throw std::runtime_error("[Device] failed to bind image memory! VkResult: " + std::to_string(result));
    }
    cout << "[Device] Image memory allocated and bound." << std::endl;

    return VK_SUCCESS;
}

void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    cout << "[Device] Populating debug messenger create info..." << std::endl;
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;  
}

VkDevice Device::getDevice() {
    cout << "[Device] getDevice called" << std::endl;
    return device_;
}