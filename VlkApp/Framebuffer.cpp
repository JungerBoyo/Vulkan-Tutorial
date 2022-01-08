#include "VlkApp.h"
#include "errLog.h"

using namespace VulkanTut;

void VlkApp::CreateSchFramebuffers()
{
    _swapChainFbos.resize(_swapChainImageViews.size());

    for(size_t i{0}; i<_swapChainImageViews.size(); ++i)
    {
        std::array<VkImageView, 2> attachments {{
                _swapChainImageViews[i],
                _depthImgView
        }};

        VkFramebufferCreateInfo fboCreateInfo{};
        fboCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fboCreateInfo.renderPass = _renderPass;
        fboCreateInfo.attachmentCount = attachments.size();
        fboCreateInfo.pAttachments = attachments.data();
        fboCreateInfo.width = _swapChainExtent.width;
        fboCreateInfo.height = _swapChainExtent.height;
        fboCreateInfo.layers = 1;

        if(vkCreateFramebuffer(_device, &fboCreateInfo, nullptr, &_swapChainFbos[i]) != VK_SUCCESS)
        {
            LOG_ARGS("sch fbo {} creation failed", i);
        }
    }
}





















