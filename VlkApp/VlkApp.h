#ifndef VULKANTUT2_VLKAPP_H
#define VULKANTUT2_VLKAPP_H

#include "EXTFnInvokers.h"
#include "Shader.h"
#include "quad.h"
#include "Img.h"

#include <vulkan/vulkan.h>
#include <tuple>
#include <optional>
#include <vector>
#include <array>
#include <string>

namespace VulkanTut
{
    class VlkApp
    {
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> transferFamily;
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
        };

        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentationModes;
        };

        struct SchRecreationInfo
        {
            void Set(int32_t wpx, int32_t hpx) volatile
            {
                flag = true;
                this->wpx = wpx;
                this->hpx = hpx;
            }

            bool flag{false};
            int32_t wpx;
            int32_t hpx;
        };

        public:
            VlkApp() = default;

            void CreateInstance(const std::vector<const char*>& instanceExtensions);
            void PickPhysicalDevice(const std::vector<const char*>& deviceExtensions);
            void CreateLogicalDevice(const std::vector<const char*>& deviceExtensions);
            void CreateSwapChain(int32_t wpx, int32_t hpx, VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE);
            void CreateImageViews();
            void CreateRenderPass();
            void CreateProgram(std::string_view vSh, std::string_view fSh);
            void CreateDescriptorSetLayout();
            void CreatePipeline();
            void CreateSchFramebuffers();
            void CreateCommandPool();
            void CreateDepthBuffer();
            void CreateTexture(Img&&);
            void CreateTextureImageView();
            void CreateTextureSampler();
            void CreateDescriptorPool();
            void CreateDescriptorSets();
            void CreateQuad() { _quad.Create(_device, _physicalDevice, _graphicsQueue, _cmdPool); }
            void CreateUniformBuffers();
            void CreateCommandBuffers();
            void CreateSemaphores();
            void CreateFences();

            void DrawFrame();
            void RecreateSwapchain();
            void Update(uint32_t imgID);

            auto& getRecreationInfo() { return _recreationInfo; }
            auto getInstance() const { return _instance; }
            void SetSurface(VkSurfaceKHR surface) { _surface = surface; }

            void Delete() const
            {
                vkDeviceWaitIdle(_device);

                for(size_t i{0}; i<MAX_FRAMES_IN_FLIGHT; ++i)
                {
                    vkDestroyFence(_device, _inFlightFences[i], nullptr);
                    vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
                    vkDestroySemaphore(_device, _imgAvailableSemaphores[i], nullptr);
                }

                vkDestroyCommandPool(_device, _cmdPool, nullptr);

                vkDestroyImageView(_device, _depthImgView, nullptr);
                vkDestroyImage(_device, _depthImg, nullptr);
                vkFreeMemory(_device, _depthImgMemory, nullptr);

                for(auto fbo : _swapChainFbos)
                    vkDestroyFramebuffer(_device, fbo, nullptr);

                vkDestroyPipeline(_device, _pipeline, nullptr);
                vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
                vkDestroyRenderPass(_device, _renderPass, nullptr);

                _shader.Delete();

                for(auto imgView : _swapChainImageViews)
                    vkDestroyImageView(_device, imgView, nullptr);

                for(size_t i{0}; i<_swapChainImages.size(); ++i)
                {
                    vkDestroyBuffer(_device, _uboBuffs[i], nullptr);
                    vkFreeMemory(_device, _uboBuffsMem[i], nullptr);
                }

                vkDestroyDescriptorPool(_device, _descPool, nullptr);

                vkDestroySwapchainKHR(_device, _swapChain, nullptr);

                vkDestroySampler(_device, _texSampler, nullptr);
                vkDestroyImageView(_device, _texImgView, nullptr);
                vkDestroyImage(_device, _texImg, nullptr);
                vkFreeMemory(_device, _texMem, nullptr);

                vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
                _quad.Delete();

                vkDestroyDevice(_device, nullptr);

                if(EnableValidationLayers)
                    DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);

                vkDestroySurfaceKHR(_instance, _surface, nullptr);
                vkDestroyInstance(_instance, nullptr);
            }

            #ifdef NOT_DEBUG
                static constexpr bool EnableValidationLayers{false};
            #else
                static constexpr bool EnableValidationLayers{true};
            #endif

            static constexpr int32_t MAX_FRAMES_IN_FLIGHT{2};
            static constexpr std::array<const char*, 1> ValidationLayers
            {
                "VK_LAYER_KHRONOS_validation"
            };

        private:
            ///api instance
            VkDebugUtilsMessengerCreateInfoEXT InitVulkanDebugging();
            bool CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions);
            static bool CheckValidationLayersSupport();

            ///physical, logical device
            bool IsDeviceSuitable(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
            bool CheckDeviceExtensionsSupport(VkPhysicalDevice device, const std::vector<const char*>& deviceExtensions);
            QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

            ///swap chain
            SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice);
            static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avFormats);
            static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avPresModes);
            static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int32_t wpx, int32_t hpx);

            ///memory, buffers, images
            std::tuple<VkBuffer, VkDeviceMemory> CreateBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkSharingMode = VK_SHARING_MODE_EXCLUSIVE);
            std::tuple<VkImage, VkDeviceMemory> CreateImage(uint32_t w, uint32_t h, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkSharingMode = VK_SHARING_MODE_EXCLUSIVE);
            VkImageView CreateImageView(VkImage, VkFormat, VkImageAspectFlags);
            void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
            void CopyBufferToImage(VkBuffer buff, VkImage img, uint32_t w, uint32_t h);
            static uint32_t FindMemType(VkPhysicalDevice pDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

            ///tmp commands
            VkCommandBuffer BeginCmd();
            void EndCmd(VkCommandBuffer cmdBuff);

            ///tex
            static VkDescriptorSetLayoutBinding GetSamplerLayoutBinding();

            ///depth buffer
            VkFormat FindSupportedFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags) const;
            VkFormat FindSupportedDepthFormat() const;

        private:
            std::vector<VkExtensionProperties> _usedInstancedExtensions;

            ///api instance
            VkInstance _instance{VK_NULL_HANDLE};
            VkDebugUtilsMessengerEXT _debugMessenger{VK_NULL_HANDLE};

            ///physical device
            VkPhysicalDevice _physicalDevice{VK_NULL_HANDLE};

            ///logical device
            VkDevice _device{VK_NULL_HANDLE};
            VkQueue _graphicsQueue{VK_NULL_HANDLE};
            VkQueue _transferQueue{VK_NULL_HANDLE};
            ///window surface and presentation
            VkSurfaceKHR _surface{VK_NULL_HANDLE};
            VkQueue _presentationQueue{VK_NULL_HANDLE};

            ///swapchain
            volatile SchRecreationInfo _recreationInfo{};
            VkSwapchainKHR _swapChain{VK_NULL_HANDLE};
            std::vector<VkImage> _swapChainImages;
            VkFormat _swapChainImageFormat;
            VkExtent2D _swapChainExtent;
            ///image views
            std::vector<VkImageView> _swapChainImageViews;
            ///ubos
            std::vector<VkDescriptorSet> _descSets{VK_NULL_HANDLE};
            VkDescriptorPool _descPool{VK_NULL_HANDLE};
            std::vector<VkBuffer> _uboBuffs;
            std::vector<VkDeviceMemory> _uboBuffsMem;

            ///Pipeline
            VkPipelineLayout _pipelineLayout{VK_NULL_HANDLE};
            VkPipeline _pipeline{VK_NULL_HANDLE};
            ShaderVF _shader; ///shader
            VkRenderPass _renderPass{VK_NULL_HANDLE}; ///render pass
            VkDescriptorSetLayout _descriptorSetLayout{VK_NULL_HANDLE}; ///descriptor layout for quad ubo

            ///Framebuffer
            std::vector<VkFramebuffer> _swapChainFbos;

            ///Commands
            VkCommandPool _cmdPool;
            std::vector<VkCommandBuffer> _cmdBuffers;
            ///semaphores and fences
            std::vector<VkSemaphore> _imgAvailableSemaphores;
            std::vector<VkSemaphore> _renderFinishedSemaphores;
            std::vector<VkFence> _swapchainImgInFlightFences;
            std::vector<VkFence> _inFlightFences;

            ///quad (VBO, IBO, UBO)
            Quad _quad{};

            ///Texture
            VkImage _texImg{VK_NULL_HANDLE};
            VkSampler _texSampler{VK_NULL_HANDLE};
            VkImageView _texImgView{VK_NULL_HANDLE};
            VkDeviceMemory _texMem{VK_NULL_HANDLE};

            ///depth buffer
            VkImage _depthImg;
            VkDeviceMemory _depthImgMemory;
            VkImageView _depthImgView;
    };
}

#endif