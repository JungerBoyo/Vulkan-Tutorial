cmake_minimum_required(VERSION 3.21)
project(vulkantut2)

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_NAME} "VlkApp/" "stbimage/" "logging/" "ConstexprMap/" "Quad/" "transform/")

add_subdirectory(ConstexprMap)

add_executable(${PROJECT_NAME} main.cpp
                Window.h
                logging/errLog.h
                VlkApp/VlkApp.h
                VlkApp/InstanceCreation.cpp
                logging/vkErrLog.h
                VlkApp/EXTFnInvokers.h VlkApp/EXTFnInvokers.cpp
                VlkApp/PhysicalDevice.cpp
                VlkApp/LogicalDevice.cpp
                VlkApp/SwapChain.cpp
                VlkApp/ImageViews.cpp
                VlkApp/Shader.h VlkApp/Shader.cpp
                VlkApp/Pipeline.cpp
                VlkApp/Framebuffer.cpp
                VlkApp/Commands.cpp
                VlkApp/Rendering.cpp
                Quad/quad.h Quad/quad.cpp
                transform/transform.h VlkApp/UniformBuffers.cpp
                stbimage/Img.h stbimage/Img.cpp VlkApp/Texture.cpp
                VlkApp/Memory.cpp VlkApp/DepthBuffer.cpp)


target_link_libraries(${PROJECT_NAME}
        constexprMap
        fmt
        glfw3 X11 Xxf86vm Xrandr pthread Xi Xinerama Xcursor
        vulkan dl
        )
