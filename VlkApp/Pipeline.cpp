#include "VlkApp.h"
#include "errLog.h"
#include "quad.h"

using namespace VulkanTut;

void VlkApp::CreateProgram(std::string_view vSh, std::string_view fSh)
{
    _shader = {_device, vSh, fSh};
}

void VlkApp::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

    std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings = {Quad::GetUBODescriptorLayoutBinding(),
                                                                  GetSamplerLayoutBinding()             };
    layoutCreateInfo.pBindings = layoutBindings.data();
    layoutCreateInfo.bindingCount = layoutBindings.size();

    if(vkCreateDescriptorSetLayout(_device, &layoutCreateInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS)
    {
        LOG("descriptor creation failed");
    }
}

void VlkApp::CreatePipeline()
{
    VkPipelineShaderStageCreateInfo vShCreateInfo{};
    vShCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vShCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vShCreateInfo.module = _shader.vertModule();
    vShCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fShCreateInfo{};
    fShCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fShCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fShCreateInfo.module = _shader.fragModule();
    fShCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] { vShCreateInfo, fShCreateInfo };

    auto bindingDesc = Quad::GetVertexBindingDescription();
    auto attribDescs = Quad::GetVertexAttribDescriptions();

    VkPipelineVertexInputStateCreateInfo vInputInfo{};
    vInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vInputInfo.vertexBindingDescriptionCount = 1;
    vInputInfo.pVertexBindingDescriptions = &bindingDesc;
    vInputInfo.vertexAttributeDescriptionCount = 3;
    vInputInfo.pVertexAttributeDescriptions = attribDescs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAsmCreateInfo{};
    inputAsmCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAsmCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAsmCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = .0f;
    viewport.y = .0f;
    viewport.width = static_cast<float>(_swapChainExtent.width);
    viewport.height = static_cast<float>(_swapChainExtent.height);
    viewport.minDepth = .0f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = _swapChainExtent;

    VkPipelineViewportStateCreateInfo vpCreateInfo{};
    vpCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vpCreateInfo.pViewports = &viewport;
    vpCreateInfo.viewportCount = 1;
    vpCreateInfo.pScissors = &scissor;
    vpCreateInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth = 1.f;
    rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizerCreateInfo.depthBiasConstantFactor = .0f;
    rasterizerCreateInfo.depthBiasClamp = .0f;
    rasterizerCreateInfo.depthBiasSlopeFactor = .0f;

    VkPipelineMultisampleStateCreateInfo msCreateInfo{};
    msCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msCreateInfo.sampleShadingEnable = VK_FALSE;
    msCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    msCreateInfo.minSampleShading = 1.f;
    msCreateInfo.pSampleMask = nullptr;
    msCreateInfo.alphaToCoverageEnable = VK_FALSE;
    msCreateInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.minDepthBounds = .0f;
    depthStencilStateCreateInfo.maxDepthBounds = 1.f;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.front = {};
    depthStencilStateCreateInfo.back = {};

    VkPipelineColorBlendAttachmentState blendAttachment{};
    blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                     VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT |
                                     VK_COLOR_COMPONENT_A_BIT ;
    blendAttachment.blendEnable = VK_FALSE;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo blendStateCreateInfo{};
    blendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendStateCreateInfo.logicOpEnable = VK_FALSE;
    blendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    blendStateCreateInfo.attachmentCount = 1;
    blendStateCreateInfo.pAttachments = &blendAttachment;
    blendStateCreateInfo.blendConstants[0] = .0f;
    blendStateCreateInfo.blendConstants[1] = .0f;
    blendStateCreateInfo.blendConstants[2] = .0f;
    blendStateCreateInfo.blendConstants[3] = .0f;

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = 0;
    dynamicStateCreateInfo.pDynamicStates = nullptr;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pSetLayouts = &_descriptorSetLayout;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;

    if(vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
    {
        LOG("pipeline layout creation failed");
    }

    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = 2;
    createInfo.pStages = shaderStages;
    createInfo.pVertexInputState = &vInputInfo;
    createInfo.pInputAssemblyState = &inputAsmCreateInfo;
    createInfo.pViewportState = &vpCreateInfo;
    createInfo.pRasterizationState = &rasterizerCreateInfo;
    createInfo.pMultisampleState = &msCreateInfo;
    createInfo.pDepthStencilState = nullptr;
    createInfo.pColorBlendState = &blendStateCreateInfo;
    createInfo.pDynamicState = &dynamicStateCreateInfo;
    createInfo.layout = _pipelineLayout;
    createInfo.renderPass = _renderPass;
    createInfo.subpass = 0;
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;
    createInfo.pDepthStencilState = &depthStencilStateCreateInfo;

    if(vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &_pipeline) != VK_SUCCESS)
    {
        LOG("creation of graphics pipeline failed");
    }
}

void VlkApp::CreateRenderPass()
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindSupportedDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = _swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDependency subpassDependency{};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentRef;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments {{colorAttachment, depthAttachment}};

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachments.size();
    renderPassCreateInfo.pAttachments = attachments.data();
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &subpassDependency;

    if(vkCreateRenderPass(_device, &renderPassCreateInfo, nullptr, &_renderPass) != VK_SUCCESS)
    {
        LOG("creation of render pass failed");
    }
}


