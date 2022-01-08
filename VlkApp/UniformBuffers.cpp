#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>

#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;

uint32_t VlkApp::FindMemType(VkPhysicalDevice pDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(pDevice, &memProps);

    for(uint32_t i{0}; i<memProps.memoryTypeCount; ++i)
        if((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    LOG("no suitable memory type was found");
    return UINT32_MAX;
}


void VlkApp::CreateUniformBuffers()
{
    _uboBuffs.resize(_swapChainImages.size());
    _uboBuffsMem.resize(_swapChainImages.size());

    for(uint32_t i{0}; i<_swapChainImages.size(); ++i)
    {
        auto[buff, memory] = CreateBuffer(Quad::uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        _uboBuffs[i] = buff;
        _uboBuffsMem[i] = memory;
    }
}

void VlkApp::Update(uint32_t imgID)
{
    static auto beginTime = std::chrono::high_resolution_clock::now();

    auto currTime = std::chrono::high_resolution_clock::now();

    float time = std::chrono::duration<float, std::chrono::seconds::period>(currTime - beginTime).count();

    transform rot{};

    rot.model = glm::rotate(glm::mat4(1.f), time * glm::radians(90.f), glm::vec3(.0f, .0f, 1.f));
    rot.view = glm::lookAt(glm::vec3(2.f), glm::vec3(.0f), glm::vec3(.0f, .0f, 1.f));
    rot.proj = glm::perspective(glm::radians(45.f), _swapChainExtent.width/static_cast<float>(_swapChainExtent.height), .1f, 10.f);
    rot.proj[1][1] *= -1.f;


    void* data;
    vkMapMemory(_device, _uboBuffsMem[imgID], 0, Quad::uboSize, 0, &data);

    memcpy(data, &rot, Quad::uboSize);

    vkUnmapMemory(_device, _uboBuffsMem[imgID]);
}

void VlkApp::CreateDescriptorPool()
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = _swapChainImages.size();
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = _swapChainImages.size();


    VkDescriptorPoolCreateInfo descPoolCreateInfo{};
    descPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolCreateInfo.poolSizeCount = poolSizes.size();
    descPoolCreateInfo.pPoolSizes = poolSizes.data();
    descPoolCreateInfo.maxSets = _swapChainImages.size();

    if(vkCreateDescriptorPool(_device, &descPoolCreateInfo, nullptr, &_descPool) != VK_SUCCESS)
    {
        LOG("creation of descriptor pool failed");
    }
}

void VlkApp::CreateDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(_swapChainImages.size(), _descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = _descPool;
    allocInfo.descriptorSetCount = _swapChainImages.size();
    allocInfo.pSetLayouts = layouts.data();

    _descSets.resize(_swapChainImages.size());

    if(vkAllocateDescriptorSets(_device, &allocInfo, _descSets.data()) != VK_SUCCESS)
    {
        LOG("Allocation of descriptor sets failed");
    }

    for(size_t i{0}; i<_swapChainImages.size(); ++i)
    {
        VkDescriptorBufferInfo buffInfo{};
        buffInfo.buffer = _uboBuffs[i];
        buffInfo.offset = 0;
        buffInfo.range = Quad::uboSize;

        VkDescriptorImageInfo imgInfo{};
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imgInfo.imageView = _texImgView;
        imgInfo.sampler = _texSampler;

        std::array<VkWriteDescriptorSet, 2> descWrites{};
        descWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrites[0].dstSet = _descSets[i];
        descWrites[0].dstBinding = 0;
        descWrites[0].dstArrayElement = 0;
        descWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrites[0].descriptorCount = 1;
        descWrites[0].pBufferInfo = &buffInfo;
        descWrites[0].pImageInfo = nullptr;
        descWrites[0].pTexelBufferView = nullptr;

        descWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrites[1].dstSet = _descSets[i];
        descWrites[1].dstBinding = 1;
        descWrites[1].dstArrayElement = 0;
        descWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descWrites[1].descriptorCount = 1;
        descWrites[1].pBufferInfo = nullptr;
        descWrites[1].pImageInfo = &imgInfo;
        descWrites[1].pTexelBufferView = nullptr;


        vkUpdateDescriptorSets(_device, descWrites.size(), descWrites.data(), 0, nullptr);
    }
}

