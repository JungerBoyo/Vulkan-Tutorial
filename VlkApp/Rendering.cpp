#include "VlkApp.h"
#include "errLog.h"
using namespace VulkanTut;

void VlkApp::CreateSemaphores()
{
    _imgAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    for(size_t i{0}; i<MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if(vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_imgAvailableSemaphores[i]) != VK_SUCCESS ||
           vkCreateSemaphore(_device, &semaphoreCreateInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS)
        {
            LOG_ARGS("{} semaphore creation failed", i);
        }
    }
}

void VlkApp::CreateFences()
{
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    _swapchainImgInFlightFences.resize(_swapChainImages.size(), VK_NULL_HANDLE);

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i{0}; i<MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if(vkCreateFence(_device, &fenceCreateInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
        {
            LOG_ARGS("{} fence creation failed", i);
        }
    }
}

void VlkApp::DrawFrame()
{
    static size_t currentFrame{0};

    vkWaitForFences(_device, 1, &_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto result = vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imgAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }

    if(_swapchainImgInFlightFences[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(_device, 1, &_swapchainImgInFlightFences[imageIndex], VK_TRUE, UINT64_MAX);

    _swapchainImgInFlightFences[imageIndex] = _inFlightFences[currentFrame];

    Update(imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkPipelineStageFlags waitStage{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &_imgAvailableSemaphores[currentFrame];
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_cmdBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &_renderFinishedSemaphores[currentFrame];

    vkResetFences(_device, 1, &_inFlightFences[currentFrame]);
    if(vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        LOG("submission of command failed");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &_renderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &_swapChain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr;

    result = vkQueuePresentKHR(_presentationQueue, &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _recreationInfo.flag)
    {
        _recreationInfo.flag = false;
        RecreateSwapchain();
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
