#include "Shader.h"
#include "errLog.h"
#include <fstream>

static std::vector<char> ParseShader(std::string_view path)
{
    std::ifstream stream(path.data(), std::ios::binary|std::ios::ate);

    auto size = static_cast<size_t>(stream.tellg());
    std::vector<char> code(size);

    stream.seekg(0);
    stream.read(code.data(), size);

    stream.close();

    return code;
}

VulkanTut::ShaderVF::ShaderVF(VkDevice device, std::string_view vSh, std::string_view fSh)
    : _device(device)
{
    const auto vCode = ParseShader(vSh);
    const auto fCode = ParseShader(fSh);

    _vshModule = CreateModule(vCode);
    _fshModule = CreateModule(fCode);
}

VkShaderModule VulkanTut::ShaderVF::CreateModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    if(vkCreateShaderModule(_device, &createInfo, nullptr, &module) != VK_SUCCESS)
    {
        LOG_ARGS("creation of shader module failed, code size {}", code.size());
        return VK_NULL_HANDLE;
    }

    return module;
}

void VulkanTut::ShaderVF::Delete() const
{
    vkDestroyShaderModule(_device, _vshModule, nullptr);
    vkDestroyShaderModule(_device, _fshModule, nullptr);
}




