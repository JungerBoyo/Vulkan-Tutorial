#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;

void VlkApp::CreateSwapChain(int32_t wpx, int32_t hpx, VkSwapchainKHR oldSwapchain)
{
    auto swapChainSupportDetails = QuerySwapChainSupport(_physicalDevice);

    auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    auto presentationMode = ChooseSwapPresentMode(swapChainSupportDetails.presentationModes);
    auto extent = ChooseSwapExtent(swapChainSupportDetails.capabilities, wpx, hpx);

    auto imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;
    if(swapChainSupportDetails.capabilities.maxImageCount > 0 &&
       swapChainSupportDetails.capabilities.maxImageCount < imageCount)
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = _surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto queueIndices = FindQueueFamilies(_physicalDevice);
    uint32_t queueFamilyIndices[]{queueIndices.presentFamily.value(), queueIndices.graphicsFamily.value()};

    if(queueIndices.graphicsFamily != queueIndices.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentationMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    if(vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
    {
        LOG("swapchain creation failed");
    }

    uint32_t schImageCount{0};
    vkGetSwapchainImagesKHR(_device, _swapChain, &schImageCount, nullptr);

    _swapChainImages.resize(schImageCount);
    vkGetSwapchainImagesKHR(_device, _swapChain, &schImageCount, _swapChainImages.data());

    _swapChainExtent = extent;
    _swapChainImageFormat = surfaceFormat.format;
}


void VlkApp::RecreateSwapchain()
{
    vkDeviceWaitIdle(_device);

    vkDestroyImageView(_device, _depthImgView, nullptr);
    vkDestroyImage(_device, _depthImg, nullptr);
    vkFreeMemory(_device, _depthImgMemory, nullptr);

    for(auto fbo : _swapChainFbos)
        vkDestroyFramebuffer(_device, fbo, nullptr);

    vkFreeCommandBuffers(_device, _cmdPool, _cmdBuffers.size(), _cmdBuffers.data());

    vkDestroyPipeline(_device, _pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
    vkDestroyRenderPass(_device, _renderPass, nullptr);

    for(auto imgView : _swapChainImageViews)
        vkDestroyImageView(_device, imgView, nullptr);

    for(size_t i{0}; i<_swapChainImages.size(); ++i)
    {
        vkDestroyBuffer(_device, _uboBuffs[i], nullptr);
        vkFreeMemory(_device, _uboBuffsMem[i], nullptr);
    }

    vkDestroyDescriptorPool(_device, _descPool, nullptr);

    auto oldSwapchain = _swapChain;
    CreateSwapChain(_recreationInfo.wpx, _recreationInfo.hpx, oldSwapchain);
    vkDestroySwapchainKHR(_device, oldSwapchain, nullptr);

    CreateImageViews();
    CreateRenderPass();
    CreatePipeline();
    CreateDepthBuffer();
    CreateSchFramebuffers();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets();
    CreateCommandBuffers();
}


VlkApp::SwapChainSupportDetails VlkApp::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

    if(formatCount)
        details.formats.resize(formatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());


    uint32_t presentationModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentationModeCount, nullptr);

    if(presentationModeCount)
        details.presentationModes.resize(formatCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentationModeCount, details.presentationModes.data());

    return details;
}


VkSurfaceFormatKHR VlkApp::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &avFormats)
{
    for(const auto& format : avFormats)
    {
        if(format.format == VK_FORMAT_B8G8R8A8_SRGB &&
           format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return avFormats[0];
}

VkPresentModeKHR VlkApp::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &avPresModes)
{
    for(const auto& mode : avPresModes)
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VlkApp::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int32_t wpx, int32_t hpx)
{
    if(capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;
    else
    {
        VkExtent2D actualExtent{};

        actualExtent.width = std::clamp((uint32_t)wpx, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp((uint32_t)hpx, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}
