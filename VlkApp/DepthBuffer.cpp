#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;


VkFormat VlkApp::FindSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for(auto format : formats)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &properties);

        if((tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) ||
           (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features))
            return format;
    }

    return static_cast<VkFormat>(INT32_MAX);
}


VkFormat VlkApp::FindSupportedDepthFormat() const
{
    return FindSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                                VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void VlkApp::CreateDepthBuffer()
{
    VkFormat depthFormat = FindSupportedDepthFormat();
                           
    std::tie(_depthImg, _depthImgMemory) = CreateImage(_swapChainExtent.width, _swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
                                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _depthImgView = CreateImageView(_depthImg, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    TransitionImageLayout(_depthImg, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

