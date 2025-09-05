#pragma once

#include "device.hpp"

class Buffer {
public:
    Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags);

    VkBuffer getBuffer() const { return buffer; }
    void map();
    void writeToBuffer(const void* data, size_t size);
    void unmap();

private:
    Device& device;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize;
    void* mapped = nullptr;
};