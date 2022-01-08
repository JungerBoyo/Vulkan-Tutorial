#include "VlkApp.h"
#include "errLog.h"
#include <set>

using namespace VulkanTut;

void VlkApp::CreateLogicalDevice(const std::vector<const char*>& deviceExtensions)
{
    auto indices = FindQueueFamilies(_physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies{indices.presentFamily.value(),
                                           indices.graphicsFamily.value(),
                                           indices.transferFamily.value()};

    auto queuePriority = 1.f;
    for(auto queFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if(EnableValidationLayers)
    {
        createInfo.enabledLayerCount = ValidationLayers.size();
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
    {
        LOG("logical device creation failed");
    }

    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentationQueue);
    vkGetDeviceQueue(_device, indices.transferFamily.value(), 0, &_transferQueue);
}














