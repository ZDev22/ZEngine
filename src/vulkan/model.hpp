#pragma once

#include "buffer.hpp"

#include <memory>

#include <glm/glm.hpp>

class Model {
public:
    struct Vertex {
        glm::vec2 position;
        glm::vec2 texCoord;

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

    Model(Device& device, const std::vector<Vertex>& vertices);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer, uint32_t instanceCount, uint32_t firstInstance = 0);

    const std::vector<Vertex>& getVertices() const { return vertices; }

private:
    Device& device;
    std::unique_ptr<Buffer> vertexBuffer;
    std::vector<Vertex> vertices;
};