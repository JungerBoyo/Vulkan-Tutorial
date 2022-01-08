#include "VlkApp/VlkApp.h"
#include "Window.h"
using namespace VulkanTut;

int main()
{
    Window::InitGLFW();
    Window win(800, 600);

    VlkApp vkApp{};

    auto instanceExtensions = Window::getVlkExtensions();
    instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::vector<const char*> deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    auto[wpx, hpx] = win.getResolutionPx();

    win.SetWindowResizeCallback([&vkApp](int32_t w, int32_t h){vkApp.getRecreationInfo().Set(w, h);});

    vkApp.CreateInstance(instanceExtensions);
    vkApp.SetSurface(win.getVlkSurface(vkApp.getInstance()));
    vkApp.PickPhysicalDevice(deviceExtensions);
    vkApp.CreateLogicalDevice(deviceExtensions);
    vkApp.CreateSwapChain(wpx, hpx);
    vkApp.CreateImageViews();
    vkApp.CreateRenderPass();
    vkApp.CreateProgram("Shaders/spirv/vert.spv", "Shaders/spirv/frag.spv");
    vkApp.CreateDescriptorSetLayout();
    vkApp.CreatePipeline();
    vkApp.CreateCommandPool();
    vkApp.CreateDepthBuffer();
    vkApp.CreateSchFramebuffers();
    vkApp.CreateTexture({"stbimage/Lenna.png"});
    vkApp.CreateTextureImageView();
    vkApp.CreateTextureSampler();
    vkApp.CreateDescriptorPool();
    vkApp.CreateQuad();
    vkApp.CreateUniformBuffers();
    vkApp.CreateDescriptorSets();
    vkApp.CreateCommandBuffers();
    vkApp.CreateSemaphores();
    vkApp.CreateFences();

    while(!win.IsClosed())
    {
        glfwPollEvents();

        if(!win.IsMinimized())
            vkApp.DrawFrame();
    }

    vkApp.Delete();

    return 0;
}
