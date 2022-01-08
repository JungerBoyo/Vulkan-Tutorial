#include "VlkApp.h"
#include "errLog.h"
#include <set>
#include <string_view>
using namespace VulkanTut;


void VlkApp::PickPhysicalDevice(const std::vector<const char*>& deviceExtensions)
{
    uint32_t deviceCount{0};
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

    if(!deviceCount)
    {
        LOG("no gpu was found");
        return;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    for(const auto& device : devices)
    {
        if(IsDeviceSuitable(device, deviceExtensions))
        {
            _physicalDevice = device;
            break;
        }
    }

    if(_physicalDevice == VK_NULL_HANDLE)
    {
        LOG("no suitable gpu was found");
    }
}

bool VlkApp::IsDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions)
{
    auto indices = FindQueueFamilies(device);
    auto extSupported = CheckDeviceExtensionsSupport(device, deviceExtensions);

    bool isSwapChainSuitable{false};
    if(extSupported)
    {
        auto swapChainSupport = QuerySwapChainSupport(device);
        isSwapChainSuitable = !swapChainSupport.formats.empty()             &&
                              !swapChainSupport.presentationModes.empty();
    }

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);


    return indices.graphicsFamily.has_value() &&
           indices.presentFamily.has_value()  &&
           indices.transferFamily.has_value() &&
           features.samplerAnisotropy         &&
           extSupported && isSwapChainSuitable;
}

bool VlkApp::CheckDeviceExtensionsSupport(VkPhysicalDevice device, const std::vector<const char *>& deviceExtensions)
{
    uint32_t extCount{0};
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> avExts(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, avExts.data());

    std::set<std::string_view> requiredExts(deviceExtensions.cbegin(), deviceExtensions.cend());

    for(const auto& avExt : avExts)
        requiredExts.erase(avExt.extensionName);

    return requiredExts.empty();
}

VlkApp::QueueFamilyIndices VlkApp::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamiliesCount{0};
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, queueFamilies.data());

    int32_t i{0};
    bool isPresGraphShareSameQueue{false};
    for(const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentationSupport{false};
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentationSupport);
        if(presentationSupport)
        {
            indices.presentFamily = i;

            if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                isPresGraphShareSameQueue = true;
                indices.graphicsFamily = i;
            }
        }

        if(!isPresGraphShareSameQueue && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            indices.graphicsFamily = i;

        if(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
            if(!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !presentationSupport)
                indices.transferFamily = i;
        ++i;
    }

    if(!indices.transferFamily.has_value())
        indices.transferFamily = indices.graphicsFamily.value();

    return indices;
}






