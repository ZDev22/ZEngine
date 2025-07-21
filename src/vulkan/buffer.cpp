#include "buffer.hpp"

#include <stdexcept>
#include <cstring>

Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment) : device{ device }, bufferSize{ instanceSize * instanceCount } {
    bufferSize = ((instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1)) * instanceCount;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = bufferSize;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device.device(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) { throw std::runtime_error("failed to create buffer!"); }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device.device(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = device.findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags);

    if (vkAllocateMemory(device.device(), &allocInfo, nullptr, &memory) != VK_SUCCESS) { throw std::runtime_error("failed to allocate buffer memory!"); }

    vkBindBufferMemory(device.device(), buffer, memory, 0);
}

void Buffer::map() { vkMapMemory(device.device(), memory, 0, bufferSize, 0, &mapped); }
void Buffer::writeToBuffer(const void* data, VkDeviceSize size, VkDeviceSize offset) { std::memcpy(static_cast<char*>(mapped) + offset, data, static_cast<size_t>(size)); }

void Buffer::unmap() {
    if (mapped) {
        vkUnmapMemory(device.device(), memory);
        mapped = nullptr;
    }
}