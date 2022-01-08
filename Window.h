#ifndef VULKANTUT2_WINDOW_H
#define VULKANTUT2_WINDOW_H

#include <GLFW/glfw3.h>
#include <functional>
#include "errLog.h"

namespace VulkanTut
{
    class Window
    {
        struct WinData
        {
            bool Iconified{false};
            std::function<void(int32_t, int32_t)> winResizeCallback{nullptr};
        };

        public:
            Window(int32_t w, int32_t h)
            {
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

                _nativeWindow = glfwCreateWindow(w, h, "", nullptr, nullptr);

                glfwSetWindowUserPointer(_nativeWindow, &_winData);
                glfwSetWindowIconifyCallback(_nativeWindow, [](GLFWwindow* win, int32_t ic){
                    auto* ptr = static_cast<WinData*>(glfwGetWindowUserPointer(win));

                    ptr->Iconified = static_cast<bool>(ic);
                });
            }

            void SetWindowResizeCallback(const std::function<void(int32_t, int32_t)>& fn)
            {
                _winData.winResizeCallback = fn;

                glfwSetFramebufferSizeCallback(_nativeWindow,
                [](GLFWwindow* win, int32_t w, int32_t h){
                    auto* ptr = static_cast<WinData*>(glfwGetWindowUserPointer(win));

                    ptr->winResizeCallback(w, h);
                });
            }

            auto getResolutionPx() const
            {
                int32_t w, h;
                glfwGetFramebufferSize(_nativeWindow, &w, &h);

                return std::tuple{ w, h };
            }

            void SwapBuffers() const
            {
                glfwSwapBuffers(_nativeWindow);
            }

            bool IsMinimized() const
            {
                return _winData.Iconified;
            }

            bool IsClosed() const
            {
                return glfwWindowShouldClose(_nativeWindow);
            }

            static std::vector<const char*> getVlkExtensions()
            {
                uint32_t extensionCount{0};
                const char** extensionsPP;

                extensionsPP = glfwGetRequiredInstanceExtensions(&extensionCount);
                std::vector<const char*> extensions(extensionCount);
                for(size_t i{0}; i<extensionCount; ++i)
                    extensions[i] = extensionsPP[i];

                return extensions;
            }

            VkSurfaceKHR getVlkSurface(VkInstance vkInstance) const
            {
                VkSurfaceKHR surface;
                if(glfwCreateWindowSurface(vkInstance, _nativeWindow, nullptr, &surface) != VK_SUCCESS)
                {
                    LOG("window surface creation failed");
                    return VK_NULL_HANDLE;
                }
                else
                    return surface;
            }


            operator GLFWwindow*() const
            {
                return _nativeWindow;
            }

            static void InitGLFW()
            {
                if(!glfwInit())
                {
                    LOG("glfw init failed");
                }
            }

            ~Window()
            {
                glfwDestroyWindow(_nativeWindow);
                glfwTerminate();
            }

        private:
            WinData _winData{};
            GLFWwindow* _nativeWindow{nullptr};
    };
}

#endif