#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(binding = 0) uniform mvpUBO
{
    mat4 model;
    mat4 view;
    mat4 proj;
};

layout(location = 0) out vec3 vColor;
layout(location = 1) out vec2 vTexCoord;

void main()
{
    vColor = inColor;
    vTexCoord = inTexCoord;
    gl_Position = proj * view * model * vec4(inPosition, 1.0);
}
