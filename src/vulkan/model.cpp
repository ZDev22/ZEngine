#include "model.hpp"

Model::Model(Device& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) : device{ device }, vertices{ vertices }, vertexCount{ static_cast<uint32_t>(vertices.size()) }, indexCount{ static_cast<uint32_t>(indices.size()) } {
    VkDeviceSize bufferSize = sizeof(Vertex) * vertices.size();

    vertexBuffer = std::make_unique<Buffer>(device, bufferSize, 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer->map();
    vertexBuffer->writeToBuffer(static_cast<const void*>(vertices.data()), sizeof(Vertex) * vertices.size());
    vertexBuffer->unmap();

    bufferSize = sizeof(uint32_t) * indices.size();

    indexBuffer = std::make_unique<Buffer>(device, bufferSize, 1, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    indexBuffer->map();
    indexBuffer->writeToBuffer(static_cast<const void*>(indices.data()), sizeof(uint32_t) * indices.size());
    indexBuffer->unmap();
}

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = { vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Model::draw(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t baseInstance) { vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, baseInstance); }