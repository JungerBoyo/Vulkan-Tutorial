#ifndef VULKANTUT2_QUAD_H
#define VULKANTUT2_QUAD_H

#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <array>

#include "transform.h"

namespace VulkanTut
{
    class Quad
    {
        struct Vertex
        {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec2 texCoord;
        };

        public:
            Quad() = default;
            void Create(VkDevice, VkPhysicalDevice, VkQueue, VkCommandPool transferCmdPool);
            void Delete() const;

            auto vbo() const { return _vbo; }
            auto ibo() const { return _ibo; }


            static constexpr std::array<Vertex, 8> vertices
            {{
                {{-.5f ,-.5f, 0.f}, {.5f, .2323f, .1f}, {.0f, 1.f}},
                {{ .5f ,-.5f, 0.f}, {.0f, .93f, .223f}, {1.f, 1.f}},
                {{ .5f , .5f, 0.f}, {.2f, .299f, .89f}, {1.f, 0.f}},
                {{-.5f , .5f, 0.f}, {.232f, .23f, .01f}, {0.f, 0.f}},

                {{-.5f ,-.5f, -.5f}, {.5f, .2323f, .1f}, {.0f, 1.f}},
                {{ .5f ,-.5f, -.5f}, {.0f, .93f, .223f}, {1.f, 1.f}},
                {{ .5f , .5f, -.5f}, {.2f, .299f, .89f}, {1.f, 0.f}},
                {{-.5f , .5f, -.5f}, {.232f, .23f, .01f}, {0.f, 0.f}}
            }};

            static constexpr std::array<uint16_t, 12> indices{{0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4}};

            static constexpr uint32_t vSize{sizeof(Vertex) * vertices.size()};
            static constexpr uint32_t iSize{sizeof(uint16_t) * indices.size()};
            static constexpr uint32_t uboSize{sizeof(transform)};

            static auto GetUBODescriptorLayoutBinding()
            {
                VkDescriptorSetLayoutBinding uboLayout{};
                uboLayout.binding = 0;
                uboLayout.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uboLayout.descriptorCount = 1;
                uboLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                uboLayout.pImmutableSamplers = nullptr;

                return uboLayout;
            }

            static auto GetVertexBindingDescription()
            {
                VkVertexInputBindingDescription bindingDesc{};
                bindingDesc.binding = 0;
                bindingDesc.stride = sizeof(Vertex);
                bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDesc;
            }

            static auto GetVertexAttribDescriptions()
            {
                std::array<VkVertexInputAttributeDescription, 3> attribDescs{};

                attribDescs[0].binding = 0;
                attribDescs[0].location = 0;
                attribDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attribDescs[0].offset = offsetof(Vertex, pos);

                attribDescs[1].binding = 0;
                attribDescs[1].location = 1;
                attribDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attribDescs[1].offset = offsetof(Vertex, color);

                attribDescs[2].binding = 0;
                attribDescs[2].location = 2;
                attribDescs[2].format = VK_FORMAT_R32G32_SFLOAT;
                attribDescs[2].offset = offsetof(Vertex, texCoord);

                return attribDescs;
            }

        private:
            void CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize);
            std::tuple<VkBuffer, VkDeviceMemory> CreateBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags);
            VkDeviceMemory AllocateVertexBuffer(VkBuffer, VkMemoryPropertyFlags);
            uint32_t FindMemType(uint32_t typeFilter, VkMemoryPropertyFlags);

        private:
            VkQueue _queue{VK_NULL_HANDLE};
            VkCommandPool _cmdPool{VK_NULL_HANDLE};
            VkPhysicalDevice _pDevice{VK_NULL_HANDLE};
            VkDevice _device{VK_NULL_HANDLE};

            VkBuffer _vbo{VK_NULL_HANDLE};
            VkDeviceMemory _vboMemory{VK_NULL_HANDLE};

            VkBuffer _ibo{VK_NULL_HANDLE};
            VkDeviceMemory _iboMemory{VK_NULL_HANDLE};
    };

}

#endif