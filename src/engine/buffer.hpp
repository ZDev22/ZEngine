#pragma once

#include "device.hpp"

struct Buffer {
public:
    Buffer(Device& device, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags);
    ~Buffer() = default;

    VkBuffer getBuffer() const { return buffer; }
    void map();
    void writeToBuffer(const void* data, unsigned int size);
    void unmap();

private:
    Device& device;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize;
    void* mapped = nullptr;
};