#version 450

layout(location = 0) out vec4 fragment;

layout(location = 0) in vec3 vColor;
layout(location = 1) in vec2 vTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

void main()
{
    fragment = vec4(vColor * texture(texSampler, vTexCoord).rgb, 1.0);
}
