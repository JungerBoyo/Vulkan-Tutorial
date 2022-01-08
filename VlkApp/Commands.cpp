#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;

void VlkApp::CreateCommandPool()
{
    auto queueFamilyIndices = FindQueueFamilies(_physicalDevice);

    VkCommandPoolCreateInfo cmdPoolCreateInfo{};
    cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    cmdPoolCreateInfo.flags = 0;

    if(vkCreateCommandPool(_device, &cmdPoolCreateInfo, nullptr, &_cmdPool) != VK_SUCCESS)
    {
        LOG("command pool creation failed");
    }
}

void VlkApp::CreateCommandBuffers()
{
    _cmdBuffers.resize(_swapChainFbos.size());

    VkCommandBufferAllocateInfo allocCreateInfo{};
    allocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocCreateInfo.commandPool = _cmdPool;
    allocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocCreateInfo.commandBufferCount = static_cast<uint32_t>(_cmdBuffers.size());

    if(vkAllocateCommandBuffers(_device, &allocCreateInfo, _cmdBuffers.data()) != VK_SUCCESS)
    {
        LOG("allocation of cmd buffers failed");
    }

    for(size_t i{0}; i<_cmdBuffers.size(); ++i)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if(vkBeginCommandBuffer(_cmdBuffers[i], &beginInfo) != VK_SUCCESS)
        {
            LOG_ARGS("failed to begin recording {} cmd buffer", i);
        }

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = _renderPass;
        renderPassBeginInfo.framebuffer = _swapChainFbos[i];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = _swapChainExtent;

        std::array<VkClearValue, 2> clearColorValues{};
        clearColorValues[0].color = {{.0f, .0f, .0f, 1.f}};
        clearColorValues[1].depthStencil = {1.f, 0};
        renderPassBeginInfo.pClearValues = clearColorValues.data();
        renderPassBeginInfo.clearValueCount = clearColorValues.size();

        vkCmdBeginRenderPass(_cmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(_cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

        VkDeviceSize offset{0};
        VkBuffer vboBuffer{_quad.vbo()};
        VkBuffer iboBuffer{_quad.ibo()};

        vkCmdBindVertexBuffers(_cmdBuffers[i], 0, 1, &vboBuffer, &offset);
        vkCmdBindIndexBuffer(_cmdBuffers[i], iboBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(_cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descSets[i], 0, nullptr);

        vkCmdDrawIndexed(_cmdBuffers[i], Quad::indices.size(), 1, 0, 0, 0);

        vkCmdEndRenderPass(_cmdBuffers[i]);

        if(vkEndCommandBuffer(_cmdBuffers[i]) != VK_SUCCESS)
        {
            LOG_ARGS("recording of {} command buffer failed", i);
        }
    }
}

VkCommandBuffer VlkApp::BeginCmd()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _cmdPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuff;
    vkAllocateCommandBuffers(_device, &allocInfo, &cmdBuff);

    VkCommandBufferBeginInfo cmdBuffBeginInfo{};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(cmdBuff, &cmdBuffBeginInfo);

    return cmdBuff;
}

void VlkApp::EndCmd(VkCommandBuffer cmdBuff)
{
    vkEndCommandBuffer(cmdBuff);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuff;

    vkQueueSubmit(_transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_transferQueue);

    vkFreeCommandBuffers(_device, _cmdPool, 1, &cmdBuff);
}






