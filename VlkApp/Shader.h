#ifndef VULKANTUT2_SHADER_H
#define VULKANTUT2_SHADER_H

#include <vulkan/vulkan.h>
#include <string_view>
#include <vector>

namespace VulkanTut
{
    class ShaderVF
    {
        public:
            ShaderVF() = default;
            ShaderVF(VkDevice device, std::string_view vSh, std::string_view fSh);

            void Delete() const;

            [[nodiscard]] auto vertModule() const { return _vshModule; }
            [[nodiscard]] auto fragModule() const { return _fshModule; }

        private:
            VkShaderModule CreateModule(const std::vector<char>& code);

        private:
            VkDevice _device;
            VkShaderModule _vshModule;
            VkShaderModule _fshModule;
    };
}

#endif