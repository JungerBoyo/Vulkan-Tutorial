#ifndef VULKANTUT2_TRANSFORM_H
#define VULKANTUT2_TRANSFORM_H

#include <glm/mat4x4.hpp>

namespace VulkanTut
{
    struct transform
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
}

#endif