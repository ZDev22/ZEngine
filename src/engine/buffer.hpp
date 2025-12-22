#pragma once

#include "device.hpp"
#include <string.h>

struct Buffer {
public:
    inline Buffer(Device& device, VkDeviceSize instanceSize, unsigned int instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags) : device(device), bufferSize(instanceSize * instanceCount) {

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vkCreateBuffer(device.device(), &bufferInfo, nullptr, &buffer);

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device.device(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

        vkAllocateMemory(device.device(), &allocInfo, nullptr, &memory);
        vkBindBufferMemory(device.device(), buffer, memory, 0);
    }
    ~Buffer() = default;

    inline VkBuffer getBuffer() const { return buffer; }
    inline void map() {
        void* temp = nullptr;
        vkMapMemory(device.device(), memory, 0, bufferSize, 0, &temp);
        mapped = (char*)temp;
    }
    inline void writeToBuffer(const void* data, unsigned int size) { memcpy(mapped, data, size); }
    inline void unmap() {
        if (mapped) {
            vkUnmapMemory(device.device(), memory);
            mapped = nullptr;
        }
    }

private:
    Device& device;
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize bufferSize;
    void* mapped = nullptr;
};