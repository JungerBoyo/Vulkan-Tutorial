#include "quad.h"
#include "errLog.h"

using namespace VulkanTut;


void Quad::Create(VkDevice device, VkPhysicalDevice pDevice, VkQueue queue, VkCommandPool transferCmdPool)
{
    _device = device;
    _pDevice = pDevice;
    _cmdPool = transferCmdPool;
    _queue = queue;

    ///vertex buff
    auto[stagingBuff, stagingBuffMem] = CreateBuffer(
            Quad::vSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    void* data;
    vkMapMemory(_device, stagingBuffMem, 0, Quad::vSize, 0, &data);

    memcpy(data, vertices.data(), Quad::vSize);

    vkUnmapMemory(_device, stagingBuffMem);
    data = nullptr;


    auto [vboBuff, vboBuffMem] = CreateBuffer(
            Quad::vSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    _vbo = vboBuff;
    _vboMemory = vboBuffMem;

    CopyBuffer(stagingBuff, vboBuff, Quad::vSize);

    vkDestroyBuffer(_device, stagingBuff, nullptr);
    vkFreeMemory(_device, stagingBuffMem, nullptr);

    ///index buffer
    auto[iboStagingBuff, iboStagingBuffMem] = CreateBuffer(
            Quad::iSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT|VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    vkMapMemory(_device, iboStagingBuffMem, 0, Quad::iSize, 0, &data);

    memcpy(data, indices.data(), Quad::iSize);

    vkUnmapMemory(_device, iboStagingBuffMem);
    data = nullptr;


    auto [iboBuff, iboBuffMem] = CreateBuffer(
            Quad::iSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    _ibo = iboBuff;
    _iboMemory = iboBuffMem;

    CopyBuffer(iboStagingBuff, iboBuff, Quad::iSize);

    vkDestroyBuffer(_device, iboStagingBuff, nullptr);
    vkFreeMemory(_device, iboStagingBuffMem, nullptr);
}

void Quad::CopyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _cmdPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    vkAllocateCommandBuffers(_device, &allocInfo, &cmdBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmdBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;

    vkCmdCopyBuffer(cmdBuffer, src, dst, 1, &copyRegion);

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_queue);

    vkFreeCommandBuffers(_device, _cmdPool, 1, &cmdBuffer);
}

uint32_t Quad::FindMemType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(_pDevice, &memProps);

    for(uint32_t i{0}; i<memProps.memoryTypeCount; ++i)
        if((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    LOG("no suitable memory type was found");
    return UINT32_MAX;
}

std::tuple<VkBuffer, VkDeviceMemory>
Quad::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
    VkBuffer buff{VK_NULL_HANDLE};

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(_device, &bufferInfo, nullptr, &buff) != VK_SUCCESS)
    {
        LOG("creation of vertex buffer failed");
    }

    return {buff, AllocateVertexBuffer(buff, properties)};
}

VkDeviceMemory Quad::AllocateVertexBuffer(VkBuffer buff, VkMemoryPropertyFlags properties)
{
    VkDeviceMemory buffMemory{VK_NULL_HANDLE};

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device, buff, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemType(memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(_device, &allocInfo, nullptr, &buffMemory) != VK_SUCCESS)
    {
        LOG("Allocation of triangle vbo memory failed");
    }

    vkBindBufferMemory(_device, buff, buffMemory, 0);

    return buffMemory;
}

void Quad::Delete() const
{
    vkDestroyBuffer(_device, _ibo, nullptr);
    vkFreeMemory(_device, _iboMemory, nullptr);

    vkDestroyBuffer(_device, _vbo, nullptr);
    vkFreeMemory(_device, _vboMemory, nullptr);
}








