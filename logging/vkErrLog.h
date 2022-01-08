#ifndef VULKANTUT2_VKERRLOG_H
#define VULKANTUT2_VKERRLOG_H

#include <vulkan/vulkan.h>
#include <ConstexprMap.h>
#include "errLog.h"

namespace VulkanTut
{
    static constexpr const std::array<std::pair<int32_t, const char *>, 7> DebugInfoTranslations
    {{
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, "verbose"},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT, "informational"},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, "warning"},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, "error"},

        {VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT, "general"},
        {VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT, "validation"},
        {VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT, "performance"},
    }};

    static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
    {
        static constexpr ConstexprMap<int32_t, const char*, 7> mappings(DebugInfoTranslations);


        LOG_ARGS("Vulkan {} debug message of {} severity, {}", mappings[messageType], mappings[messageSeverity],
                pCallbackData->pMessage);

        return VK_FALSE;
    }

}



#endif