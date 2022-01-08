#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;

void VlkApp::CreateTexture(Img&& img)
{
    auto[stagingBuff, stagingBuffMem] = CreateBuffer(img.pixels.size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|
                                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(_device, stagingBuffMem, 0, img.pixels.size(), 0, &data);

    memcpy(data, img.pixels.data(), img.pixels.size());

    vkUnmapMemory(_device, stagingBuffMem);


    auto[vkimg, imgMemory] = CreateImage(img.width, img.height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                                       VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _texImg = vkimg;
    _texMem = imgMemory;

    TransitionImageLayout(vkimg, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    CopyBufferToImage(stagingBuff, vkimg, img.width, img.height);

    TransitionImageLayout(vkimg, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(_device, stagingBuff, nullptr);
    vkFreeMemory(_device, stagingBuffMem, nullptr);
}

VkImageView VlkApp::CreateImageView(VkImage img, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageView imgView;

    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = img;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if(vkCreateImageView(_device, &imageViewCreateInfo, nullptr, &imgView) != VK_SUCCESS)
    {
        LOG("tex img view creation failed");
    }

    return imgView;
}


void VlkApp::CreateTextureImageView()
{
    _texImgView = CreateImageView(_texImg, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}


void VlkApp::CreateTextureSampler()
{
    VkSamplerCreateInfo samplerCreateInfo{};
    samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
    samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerCreateInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(_physicalDevice, &properties);
    samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable = VK_FALSE;
    samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode= VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerCreateInfo.mipLodBias = .0f;
    samplerCreateInfo.minLod = .0f;
    samplerCreateInfo.maxLod = .0f;

    if(vkCreateSampler(_device, &samplerCreateInfo, nullptr, &_texSampler) != VK_SUCCESS)
    {
        LOG("creation of tex sampler failed");
    }

}

VkDescriptorSetLayoutBinding VlkApp::GetSamplerLayoutBinding()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    return samplerLayoutBinding;
}

