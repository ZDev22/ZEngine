#include "model.hpp"

Model::Model(Device& device, const std::vector<Vertex>& vertices) : device(device), vertices(vertices) {
    vertexBuffer = std::make_unique<Buffer>(device, sizeof(Vertex) * vertices.size(), 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    vertexBuffer->map();
    vertexBuffer->writeToBuffer(static_cast<const void*>(vertices.data()), sizeof(Vertex) * vertices.size());
    vertexBuffer->unmap();
}

Model::~Model() { vertices.clear(); }

void Model::bind(VkCommandBuffer commandBuffer) {
    VkBuffer buffers[] = { vertexBuffer->getBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

void Model::draw(VkCommandBuffer commandBuffer, unsigned int instanceCount, unsigned int firstInstance) { vkCmdDraw(commandBuffer, vertices.size(), instanceCount, 0, firstInstance); }