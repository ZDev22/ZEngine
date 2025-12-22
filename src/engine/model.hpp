#pragma once

#include "buffer.hpp"
#include <vector>

#include <memory>

struct Model {
public:
    struct Vertex {
        float position[2];
        float texCoord[2];

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        }

        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, texCoord);
            return attributeDescriptions;
        }
    };

    inline Model(Device& device, const std::vector<Vertex>& vertices) : device(device), vertices(vertices) {
        vertexBuffer = std::make_unique<Buffer>(device, sizeof(Vertex) * vertices.size(), 1, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        vertexBuffer->map();
        vertexBuffer->writeToBuffer(static_cast<const void*>(vertices.data()), sizeof(Vertex) * vertices.size());
        vertexBuffer->unmap();
    }
    inline ~Model() { vertices.clear(); }

    inline void bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer->getBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    inline void draw(VkCommandBuffer commandBuffer, unsigned int instanceCount, unsigned int firstInstance) { vkCmdDraw(commandBuffer, vertices.size(), instanceCount, 0, firstInstance); }
    inline const std::vector<Vertex>& getVertices() const { return vertices; }

private:
    Device& device;
    std::unique_ptr<Buffer> vertexBuffer;
    std::vector<Vertex> vertices;
};