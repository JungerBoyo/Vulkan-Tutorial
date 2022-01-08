#include "VlkApp.h"
#include "vkErrLog.h"

using namespace VulkanTut;

bool VlkApp::CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions)
{
    uint32_t extCount{0};
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);

    std::vector<VkExtensionProperties> avExtProperties(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, avExtProperties.data());

    for(const auto* ext : extensions)
    {
        bool extFound{false};

        for(const auto& avExt : avExtProperties)
            if(!strcmp(ext, avExt.extensionName))
            {
                _usedInstancedExtensions.push_back(avExt);
                extFound = true;
                break;
            }

        if(!extFound)
        {
            LOG_ARGS("extension {} not found", ext);
            return false;
        }
    }

    return true;
}

bool VlkApp::CheckValidationLayersSupport()
{
    uint32_t layerCount{0};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> avLayerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, avLayerProperties.data());

    for(const auto* layer : ValidationLayers)
    {
        bool layerFound{false};

        for(const auto& avLayer : avLayerProperties)
            if(!strcmp(layer, avLayer.layerName))
            {
                layerFound = true;
                break;
            }

        if(!layerFound)
        {
            LOG_ARGS("layer {} not found", layer);
            return false;
        }
    }

    return true;
}

void VlkApp::CreateInstance(const std::vector<const char*>& instanceExtensions)
{
    if(EnableValidationLayers && !CheckValidationLayersSupport())
    {
        LOG("requested validation layers not available");
        return;
    }

    if(!CheckInstanceExtensionsSupport(instanceExtensions))
    {
        LOG("requested extensions not available");
        return;
    }

    ///App info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.pEngineName = "no engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    ///instance info, here defining used extensions
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = instanceExtensions.size();
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{InitVulkanDebugging()};
    if(EnableValidationLayers)
    {
        createInfo.enabledLayerCount = ValidationLayers.size();
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
        createInfo.pNext = &debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0; /// global VALIDATION layers count
        createInfo.pNext = nullptr;
    }

    if(vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
    {
        LOG("vk instance creation failed");
    }

    if(CreateDebugUtilsMessengerEXT(_instance, &debugCreateInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
    {
        LOG("vulkan debug set up failed");
    }
}

VkDebugUtilsMessengerCreateInfoEXT VlkApp::InitVulkanDebugging()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    createInfo.messageSeverity = //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = &VulkanDebugCallback;
    createInfo.pUserData = nullptr;

    return createInfo;
}

