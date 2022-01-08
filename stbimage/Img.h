#ifndef VULKANTUT2_IMG_H
#define VULKANTUT2_IMG_H

#include <vector>
#include <string_view>

namespace VulkanTut
{
    using ubyte = unsigned char;

    struct Img
    {
        Img(std::string_view path);

        std::vector<ubyte> pixels;
        int32_t width;
        int32_t height;
        int32_t channels{4};
    };
}

#endif