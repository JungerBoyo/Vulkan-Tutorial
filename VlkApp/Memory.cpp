#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;

std::tuple<VkBuffer, VkDeviceMemory>
VlkApp::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode mode)
{
    VkBuffer buff{VK_NULL_HANDLE};

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = mode;

    if(vkCreateBuffer(_device, &bufferInfo, nullptr, &buff) != VK_SUCCESS)
    {
        LOG("creation of vertex buffer failed");
    }

    VkDeviceMemory buffMemory{VK_NULL_HANDLE};

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device, buff, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemType(_physicalDevice, memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(_device, &allocInfo, nullptr, &buffMemory) != VK_SUCCESS)
    {
        LOG("Allocation of triangle vbo memory failed");
    }

    vkBindBufferMemory(_device, buff, buffMemory, 0);

    return {buff, buffMemory};
}

std::tuple<VkImage, VkDeviceMemory>
VlkApp::CreateImage(uint32_t w, uint32_t h, VkFormat format, VkImageTiling tiling,
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkSharingMode mode)
{
    VkImage image;
    VkDeviceMemory imageMemory;

    VkImageCreateInfo imgInfo{};
    imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imgInfo.imageType = VK_IMAGE_TYPE_2D;
    imgInfo.extent.width = w;
    imgInfo.extent.height = h;
    imgInfo.extent.depth = 1;
    imgInfo.mipLevels = 1;
    imgInfo.arrayLayers = 1;
    imgInfo.format = format;
    imgInfo.tiling = tiling;
    imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgInfo.usage = usage;
    imgInfo.sharingMode = mode;
    imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgInfo.flags = 0;

    if(vkCreateImage(_device, &imgInfo, nullptr, &image) != VK_SUCCESS)
    {
        LOG("Failed to create texture");
    }

    VkMemoryRequirements memRequirementsImg;
    vkGetImageMemoryRequirements(_device, image, &memRequirementsImg);

    VkMemoryAllocateInfo allocInfoImg{};
    allocInfoImg.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfoImg.allocationSize = memRequirementsImg.size;
    allocInfoImg.memoryTypeIndex = FindMemType(_physicalDevice, memRequirementsImg.memoryTypeBits, properties);

    if(vkAllocateMemory(_device, &allocInfoImg, nullptr, &imageMemory) != VK_SUCCESS)
    {
        LOG("Allocation of image memory failed");
    }

    vkBindImageMemory(_device, image, imageMemory, 0);

    return {image, imageMemory};
}

void VlkApp::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    auto cmdBuff = BeginCmd();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if(((format & VK_FORMAT_D32_SFLOAT_S8_UINT) == format) ||
           ((format & VK_FORMAT_D24_UNORM_S8_UINT) == format))
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

    }
    else
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    VkPipelineStageFlags srcStage{0};
    VkPipelineStageFlags dstStage{0};

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT|VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else
    {
        LOG("undefined layout transition");
    }


    vkCmdPipelineBarrier(cmdBuff, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndCmd(cmdBuff);
}


void VlkApp::CopyBufferToImage(VkBuffer buff, VkImage img, uint32_t w, uint32_t h)
{
    auto cmdBuff = BeginCmd();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {w, h, 1};

    vkCmdCopyBufferToImage(cmdBuff, buff, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndCmd(cmdBuff);
}


