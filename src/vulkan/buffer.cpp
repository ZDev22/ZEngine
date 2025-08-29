#include "buffer.hpp"

#include <cstring>

Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags) : device(device), bufferSize(instanceSize * instanceCount) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = instanceSize;
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

void Buffer::map() { vkMapMemory(device.device(), memory, 0, bufferSize, 0, &mapped); }
void Buffer::writeToBuffer(const void* data, VkDeviceSize size) { std::memcpy(static_cast<char*>(mapped), data, static_cast<size_t>(size)); }

void Buffer::unmap() {
    if (mapped) {
        vkUnmapMemory(device.device(), memory);
        mapped = nullptr;
    }
}