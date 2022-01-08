#ifndef VULKANTUT2_EXTFNINVOKERS_H
#define VULKANTUT2_EXTFNINVOKERS_H

#include <vulkan/vulkan.h>

namespace VulkanTut
{

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

}

#endif
