////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Backend/Window/BackendGui.hpp>

#include <TGUI/DefaultBackendWindow.hpp>
#include <TGUI/Event.hpp>
#include <TGUI/Loading/ImageLoader.hpp>

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #include <TGUI/Backend/SFML-Graphics.hpp>
#elif TGUI_HAS_BACKEND_SFML_OPENGL3
    #include <TGUI/Backend/SFML-OpenGL3.hpp>
#elif TGUI_HAS_BACKEND_SDL_GPU
    #include <TGUI/Backend/SDL-GPU.hpp>
#elif TGUI_HAS_BACKEND_SDL_RENDERER
    #include <TGUI/Backend/SDL-Renderer.hpp>
#elif TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
    #include <TGUI/Backend/SDL-TTF-OpenGL3.hpp>
#elif TGUI_HAS_BACKEND_SDL_TTF_GLES2
    #include <TGUI/Backend/SDL-TTF-GLES2.hpp>
#elif TGUI_HAS_BACKEND_SDL_OPENGL3
    #include <TGUI/Backend/SDL-OpenGL3.hpp>
#elif TGUI_HAS_BACKEND_SDL_GLES2
    #include <TGUI/Backend/SDL-GLES2.hpp>
#elif TGUI_HAS_BACKEND_SDL_VULKAN
    #include <TGUI/Backend/SDL-Vulkan.hpp>

    #include <SDL3/SDL_vulkan.h>
    #define TGUI_DEFAULT_WINDOW_USES_VULKAN 1
#elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2 || TGUI_HAS_BACKEND_GLFW_VULKAN
    #if TGUI_HAS_BACKEND_GLFW_OPENGL3
        #include <TGUI/Backend/GLFW-OpenGL3.hpp>
    #elif TGUI_HAS_BACKEND_GLFW_GLES2
        #include <TGUI/Backend/GLFW-GLES2.hpp>
    #else
        #include <TGUI/Backend/GLFW-Vulkan.hpp>
        #define TGUI_DEFAULT_WINDOW_USES_VULKAN 1
    #endif

    #define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extension loader
    #include <GLFW/glfw3.h>

    #include <queue>
#elif TGUI_HAS_BACKEND_RAYLIB
    #include <TGUI/Backend/raylib.hpp>
#endif

#if TGUI_DEFAULT_WINDOW_USES_VULKAN
    #include <TGUI/Backend/Renderer/Vulkan/SwapchainVulkan.hpp>

    #include <cstring>
    #include <functional>
    #include <vector>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_DEFAULT_WINDOW_USES_VULKAN

    // Functions are loaded via vkGetInstanceProcAddr so that TGUI doesn't have to to link to the Vulkan loader
    #define TGUI_LOAD_VULKAN_FUNCTION(instance, func) \
        const auto func = reinterpret_cast<PFN_##func>(getInstanceProcAddr(instance, #func))

    namespace
    {
        // Draws a frame to the window via the swapchain
        template <typename GuiType>
        void drawVulkanFrame(GuiType& gui, SwapchainVulkan& swapchain, Vector2u framebufferSize)
        {
            const VkCommandBuffer cmdBuffer = swapchain.beginFrame(framebufferSize, {200, 200, 200});
            if (cmdBuffer == VK_NULL_HANDLE)
                return; // The window is probably minimized

            gui.setRenderPass(swapchain.getRenderPass(), swapchain.getColorFormat(), SwapchainVulkan::FramesInFlight);
            gui.draw(cmdBuffer);
            swapchain.endFrame();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Creates a Vulkan instance and a device with a queue that can render and present to the window
        VulkanContext createVulkanDevice(PFN_vkGetInstanceProcAddr getInstanceProcAddr,
                                         std::vector<const char*> instanceExtensions,
                                         const std::function<bool(VkInstance, VkPhysicalDevice, std::uint32_t)>& isPresentationSupported)
        {
            if (!getInstanceProcAddr)
                throw Exception("Failed to load the Vulkan library");

            TGUI_LOAD_VULKAN_FUNCTION(VK_NULL_HANDLE, vkCreateInstance);
            TGUI_LOAD_VULKAN_FUNCTION(VK_NULL_HANDLE, vkEnumerateInstanceExtensionProperties);
            if (!vkCreateInstance || !vkEnumerateInstanceExtensionProperties)
                throw Exception("Failed to load the Vulkan functions");

            VkApplicationInfo appInfo = {};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pEngineName = "TGUI";
            appInfo.apiVersion = VK_API_VERSION_1_0;

            VkInstanceCreateInfo instanceInfo = {};
            instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceInfo.pApplicationInfo = &appInfo;

    #ifdef VK_KHR_portability_enumeration
            // The portability extension has to be enabled to find devices that use MoltenVK on macOS
            std::uint32_t availableExtensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());
            for (const auto& extension : availableExtensions)
            {
                if (std::strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
                {
                    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
                    instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
                }
            }
    #endif

            VulkanContext vulkanContext;
            vulkanContext.getInstanceProcAddr = getInstanceProcAddr;
            instanceInfo.enabledExtensionCount = static_cast<std::uint32_t>(instanceExtensions.size());
            instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
            if (vkCreateInstance(&instanceInfo, nullptr, &vulkanContext.instance) != VK_SUCCESS)
                throw Exception("Failed to create Vulkan instance");

            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkDestroyInstance);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkEnumeratePhysicalDevices);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkEnumerateDeviceExtensionProperties);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkGetPhysicalDeviceQueueFamilyProperties);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkGetPhysicalDeviceProperties);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkCreateDevice);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkGetDeviceQueue);

            // Pick a device that has a queue family that supports both graphics and presenting, preferring a discrete GPU
            std::uint32_t physicalDeviceCount = 0;
            vkEnumeratePhysicalDevices(vulkanContext.instance, &physicalDeviceCount, nullptr);
            std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
            vkEnumeratePhysicalDevices(vulkanContext.instance, &physicalDeviceCount, physicalDevices.data());

            bool hasPortabilitySubset = false;
            for (const VkPhysicalDevice physicalDevice : physicalDevices)
            {
                std::uint32_t deviceExtensionCount = 0;
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
                std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());

                bool hasSwapchain = false;
                bool hasPortability = false;
                for (const auto& extension : deviceExtensions)
                {
                    if (std::strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
                        hasSwapchain = true;
                    else if (std::strcmp(extension.extensionName, "VK_KHR_portability_subset") == 0)
                        hasPortability = true;
                }
                if (!hasSwapchain)
                    continue;

                std::uint32_t queueFamilyCount = 0;
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
                std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
                vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

                std::uint32_t queueFamilyIndex = 0;
                while ((queueFamilyIndex < queueFamilyCount)
                       && (!(queueFamilies[queueFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                           || !isPresentationSupported(vulkanContext.instance, physicalDevice, queueFamilyIndex)))
                    ++queueFamilyIndex;
                if (queueFamilyIndex == queueFamilyCount)
                    continue;

                VkPhysicalDeviceProperties properties;
                vkGetPhysicalDeviceProperties(physicalDevice, &properties);
                const bool isDiscreteGpu = (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
                if ((vulkanContext.physicalDevice == VK_NULL_HANDLE) || isDiscreteGpu)
                {
                    vulkanContext.physicalDevice = physicalDevice;
                    vulkanContext.queueFamilyIndex = queueFamilyIndex;
                    hasPortabilitySubset = hasPortability;
                }

                if (isDiscreteGpu)
                    break;
            }

            if (vulkanContext.physicalDevice == VK_NULL_HANDLE)
            {
                vkDestroyInstance(vulkanContext.instance, nullptr);
                throw Exception("Failed to find a Vulkan device that can render to the window");
            }

            const float queuePriority = 1;
            VkDeviceQueueCreateInfo queueInfo = {};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = vulkanContext.queueFamilyIndex;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &queuePriority;

            std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
            if (hasPortabilitySubset)
                deviceExtensions.push_back("VK_KHR_portability_subset");

            VkDeviceCreateInfo deviceCreateInfo = {};
            deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            deviceCreateInfo.queueCreateInfoCount = 1;
            deviceCreateInfo.pQueueCreateInfos = &queueInfo;
            deviceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size());
            deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
            if (vkCreateDevice(vulkanContext.physicalDevice, &deviceCreateInfo, nullptr, &vulkanContext.device) != VK_SUCCESS)
            {
                vkDestroyInstance(vulkanContext.instance, nullptr);
                throw Exception("Failed to create Vulkan device");
            }

            vkGetDeviceQueue(vulkanContext.device, vulkanContext.queueFamilyIndex, 0, &vulkanContext.queue);
            return vulkanContext;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void destroyVulkanDevice(const VulkanContext& vulkanContext)
        {
            if (!vulkanContext.instance)
                return;

            const PFN_vkGetInstanceProcAddr getInstanceProcAddr = vulkanContext.getInstanceProcAddr;
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkDestroyDevice);
            TGUI_LOAD_VULKAN_FUNCTION(vulkanContext.instance, vkDestroyInstance);
            if (vulkanContext.device)
                vkDestroyDevice(vulkanContext.device, nullptr);
            vkDestroyInstance(vulkanContext.instance, nullptr);
        }
    } // namespace
#endif

#if TGUI_HAS_BACKEND_SFML_GRAPHICS || TGUI_HAS_BACKEND_SFML_OPENGL3
    class BackendWindowSFML : public DefaultBackendWindow
    {
    public:
        BackendWindowSFML(unsigned int width, unsigned int height, const String& title)
        {
            sf::ContextSettings settings;
    #if !TGUI_HAS_BACKEND_SFML_GRAPHICS
            settings.majorVersion = 3;
            settings.minorVersion = 3;
            settings.attributeFlags = sf::ContextSettings::Attribute::Core;
    #endif

    #if SFML_VERSION_MAJOR >= 3
            m_window.create(
                sf::VideoMode{
                    { width,
                      height }},
                title.toStdString(),
                sf::Style::Default,
                sf::State::Windowed,
                settings);
    #else
            m_window.create(sf::VideoMode{width, height}, title.toStdString(), sf::Style::Default, settings);
    #endif

            m_gui = std::make_unique<Gui>(m_window);
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});
        }

        ~BackendWindowSFML() override
        {
            // Destroy the gui before closing the window, as closing the window destroys the OpenGL context
            // that will still be used when destructing the backend renderer.
            m_gui = nullptr;
            close();
            BackendSFML::cleanupLeakedCursors();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            return m_window.isOpen();
        }

        void close() override
        {
            m_window.close();
        }

        bool pollEvent(Event& event) override
        {
    #if SFML_VERSION_MAJOR >= 3
            while (const auto eventSFML = m_window.pollEvent())
            {
                if (m_gui->convertEvent(*eventSFML, event))
                    return true;
            }

            return false; // No new events
    #else
            sf::Event eventSFML;
            while (m_window.pollEvent(eventSFML))
            {
                if (m_gui->convertEvent(eventSFML, event))
                    return true;
            }

            return false; // No new events
    #endif
        }

        void draw() override
        {
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            m_window.display();
        }

        void mainLoop(Color clearColor) override
        {
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((getResourcePath() / filename).asString(), iconSize);
            if (pixelPtr)
    #if SFML_VERSION_MAJOR >= 3
                m_window.setIcon({iconSize.x, iconSize.y}, pixelPtr.get());
    #else
                m_window.setIcon(iconSize.x, iconSize.y, pixelPtr.get());
    #endif
        }

    private:
    #if TGUI_HAS_BACKEND_SFML_GRAPHICS
        sf::RenderWindow m_window;
    #else
        sf::Window m_window;
    #endif
        std::unique_ptr<Gui> m_gui;
    };

#elif TGUI_HAS_BACKEND_SDL_GPU || TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_GLES2 \
    || TGUI_HAS_BACKEND_SDL_VULKAN || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2

    class BackendWindowSDL : public DefaultBackendWindow
    {
    public:
        BackendWindowSDL(unsigned int width, unsigned int height, const String& title)
        {
            SDL_Init(SDL_INIT_VIDEO);

    #if TGUI_HAS_BACKEND_SDL_GPU || TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
            TTF_Init();
    #endif

    #if TGUI_HAS_BACKEND_SDL_GPU
            m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
                                           false,
                                           nullptr);
    #elif TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    #elif TGUI_HAS_BACKEND_SDL_GLES2 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    #endif

    #if SDL_MAJOR_VERSION >= 3
        #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            SDL_WindowFlags backendWindowFlags = SDL_WINDOW_VULKAN;
        #else
            SDL_WindowFlags backendWindowFlags = SDL_WINDOW_OPENGL;
        #endif
            m_window = SDL_CreateWindow(title.toStdString().c_str(),
                                        static_cast<int>(width),
                                        static_cast<int>(height),
                                        backendWindowFlags | SDL_WINDOW_RESIZABLE);
    #else
            m_window = SDL_CreateWindow(title.toStdString().c_str(),
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        static_cast<int>(width),
                                        static_cast<int>(height),
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    #endif

    #if TGUI_HAS_BACKEND_SDL_GPU
            SDL_ClaimWindowForGPUDevice(m_device, m_window);
            m_gui = std::make_unique<Gui>(m_window, m_device);
    #elif TGUI_HAS_BACKEND_SDL_RENDERER
        #if SDL_MAJOR_VERSION >= 3
            m_renderer = SDL_CreateRenderer(m_window, nullptr);
        #else
            m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
        #endif
            m_gui = std::make_unique<Gui>(m_window, m_renderer);
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});
    #elif TGUI_DEFAULT_WINDOW_USES_VULKAN
            Uint32 sdlExtensionCount = 0;
            const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
            if (!sdlExtensions)
                throw Exception("Vulkan is not available for SDL windows on this system. Error: " + String(SDL_GetError()));

            m_vulkanContext = createVulkanDevice(reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr()),
                                                 {sdlExtensions, sdlExtensions + sdlExtensionCount},
                                                 [](VkInstance instance, VkPhysicalDevice physicalDevice, std::uint32_t queueFamilyIndex)
                                                 {
                                                     return SDL_Vulkan_GetPresentationSupport(instance, physicalDevice, queueFamilyIndex);
                                                 });
            m_gui = std::make_unique<Gui>(m_window, m_vulkanContext);
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});
    #else
            m_glContext = SDL_GL_CreateContext(m_window);
            m_gui = std::make_unique<Gui>(m_window);
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});
    #endif
            m_windowOpen = true;
        }

        ~BackendWindowSDL() override
        {
    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            m_swapchain = nullptr;
    #endif
            m_gui = nullptr; // Gui must be destroyed before destroying SDL window

    #if TGUI_HAS_BACKEND_SDL_GPU
            if (m_device && m_window)
                SDL_ReleaseWindowFromGPUDevice(m_device, m_window);
            if (m_window)
                SDL_DestroyWindow(m_window);
            if (m_device)
                SDL_DestroyGPUDevice(m_device);
    #elif TGUI_HAS_BACKEND_SDL_RENDERER
            if (m_renderer)
                SDL_DestroyRenderer(m_renderer);
            if (m_window)
                SDL_DestroyWindow(m_window);
    #elif TGUI_DEFAULT_WINDOW_USES_VULKAN
            destroyVulkanDevice(m_vulkanContext);
            if (m_window)
                SDL_DestroyWindow(m_window);
    #else
        #if SDL_MAJOR_VERSION >= 3
            if (m_glContext)
                SDL_GL_DestroyContext(m_glContext);
        #else
            if (m_glContext)
                SDL_GL_DeleteContext(m_glContext);
        #endif
            if (m_window)
                SDL_DestroyWindow(m_window);
    #endif

    #if TGUI_HAS_BACKEND_SDL_GPU || TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
            TTF_Quit();
    #endif
            SDL_Quit();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            return m_windowOpen;
        }

        void close() override
        {
            m_windowOpen = false;

            // We send a quit event in case we are using the main loop of the gui.
            // We can't put a boolean in the mainLoop function of this class because it could be called directly on the gui object
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        bool pollEvent(Event& event) override
        {
            SDL_Event eventSDL;
            while (SDL_PollEvent(&eventSDL) != 0)
            {
                if (m_gui->convertEvent(eventSDL, event))
                    return true;
            }

            return false; // No new events
        }

        void draw() override
        {
    #if TGUI_HAS_BACKEND_SDL_GPU
            SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
            if (!cmdBuffer)
                return;
            SDL_GPUTexture* swapchainTexture;
            if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuffer, m_window, &swapchainTexture, nullptr, nullptr))
                return;
            if (swapchainTexture)
            {
                m_gui->prepareDraw(cmdBuffer);

                SDL_GPUColorTargetInfo colorTargetInfo = {};
                colorTargetInfo.texture = swapchainTexture;
                colorTargetInfo.clear_color = {200.f / 255.f, 200.f / 255.f, 200.f / 255.f, 1.f};
                colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
                colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

                SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, nullptr);
                m_gui->draw(renderPass);
                SDL_EndGPURenderPass(renderPass);
            }
            SDL_SubmitGPUCommandBuffer(cmdBuffer);
    #elif TGUI_HAS_BACKEND_SDL_RENDERER
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            SDL_RenderPresent(m_renderer);
    #elif TGUI_DEFAULT_WINDOW_USES_VULKAN
            if (!m_swapchain || m_swapchain->isSurfaceLost())
            {
                m_swapchain = nullptr; // A window can't have multiple swapchains at the same time
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                if (!SDL_Vulkan_CreateSurface(m_window, m_vulkanContext.instance, nullptr, &surface))
                    throw Exception("Failed to create a Vulkan surface for the window. Error: " + String(SDL_GetError()));
                m_swapchain = std::make_unique<SwapchainVulkan>(std::static_pointer_cast<BackendRendererVulkan>(
                                                                    getBackend()->getRenderer()),
                                                                surface);
            }

            int framebufferWidth = 0;
            int framebufferHeight = 0;
            SDL_GetWindowSizeInPixels(m_window, &framebufferWidth, &framebufferHeight);
            drawVulkanFrame(*m_gui,
                            *m_swapchain,
                            {static_cast<unsigned int>(framebufferWidth), static_cast<unsigned int>(framebufferHeight)});
    #else
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            SDL_GL_SwapWindow(m_window);
    #endif
        }

        void mainLoop(Color clearColor) override
        {
    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            m_swapchain = nullptr; // The gui creates its own swapchain for the window
    #endif
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((Filesystem::Path(getResourcePath()) / filename).asString(), iconSize);
            if (!pixelPtr)
                return;

    #if SDL_MAJOR_VERSION >= 3
            SDL_Surface* icon = SDL_CreateSurfaceFrom(static_cast<int>(iconSize.x),
                                                      static_cast<int>(iconSize.y),
                                                      SDL_PIXELFORMAT_RGBA32,
                                                      pixelPtr.get(),
                                                      4 * static_cast<int>(iconSize.x));
    #else
            SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom(pixelPtr.get(),
                                                                   static_cast<int>(iconSize.x),
                                                                   static_cast<int>(iconSize.y),
                                                                   32,
                                                                   4 * static_cast<int>(iconSize.x),
                                                                   SDL_PIXELFORMAT_RGBA32);
    #endif
            if (!icon)
                return;

            SDL_SetWindowIcon(m_window, icon);
            SDL_DestroySurface(icon);
        }

    private:
        SDL_Window* m_window = nullptr;
    #if TGUI_HAS_BACKEND_SDL_GPU
        SDL_GPUDevice* m_device = nullptr;
    #elif TGUI_HAS_BACKEND_SDL_RENDERER
        SDL_Renderer* m_renderer = nullptr;
    #elif TGUI_DEFAULT_WINDOW_USES_VULKAN
        VulkanContext m_vulkanContext;
        std::unique_ptr<SwapchainVulkan> m_swapchain; // Only exists when draw() is used outside the main loop
    #else
        SDL_GLContext m_glContext = nullptr;
    #endif
        std::unique_ptr<Gui> m_gui;
        bool m_windowOpen = false;
    };

#elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2 || TGUI_HAS_BACKEND_GLFW_VULKAN
    class BackendWindowGLFW : public DefaultBackendWindow
    {
    public:
        BackendWindowGLFW(unsigned int width, unsigned int height, const String& title)
        {
            glfwInit();

    #if TGUI_HAS_BACKEND_GLFW_OPENGL3
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Required for macOS
    #elif TGUI_HAS_BACKEND_GLFW_GLES2
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    #else
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    #endif

            m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.toStdString().c_str(), nullptr, nullptr);

    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            std::uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            if (!glfwExtensions)
                throw Exception("Vulkan is not available for GLFW windows on this system");

            m_vulkanContext = createVulkanDevice(reinterpret_cast<PFN_vkGetInstanceProcAddr>(
                                                     glfwGetInstanceProcAddress(nullptr, "vkGetInstanceProcAddr")),
                                                 {glfwExtensions, glfwExtensions + glfwExtensionCount},
                                                 [](VkInstance instance, VkPhysicalDevice physicalDevice, std::uint32_t queueFamilyIndex)
                                                 {
                                                     return glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, queueFamilyIndex)
                                                            == GLFW_TRUE;
                                                 });
            m_gui = std::make_unique<Gui>(m_window, m_vulkanContext);
    #else
            glfwMakeContextCurrent(m_window);
            glfwSwapInterval(1);

            m_gui = std::make_unique<Gui>(m_window);
    #endif
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});

            // Set callback functions to push new events on a queue that is accessed by pollEvent
            glfwSetWindowUserPointer(m_window, this);
            glfwSetWindowFocusCallback(m_window,
                                       [](GLFWwindow* window, int focused)
                                       {
                                           auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                           const auto event = static_cast<Gui*>(backendWindow->getGui())->convertWindowFocusEvent(focused);
                                           if (event)
                                               backendWindow->pushEvent(*event);
                                       });
            glfwSetFramebufferSizeCallback(m_window,
                                           [](GLFWwindow* window, int fbWidth, int fbHeight)
                                           {
                                               auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                               const auto event = static_cast<Gui*>(backendWindow->getGui())
                                                                      ->convertSizeEvent(fbWidth, fbHeight);
                                               if (event)
                                                   backendWindow->pushEvent(*event);
                                           });
            glfwSetCharCallback(m_window,
                                [](GLFWwindow* window, unsigned int codepoint)
                                {
                                    auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                    const auto event = static_cast<Gui*>(backendWindow->getGui())->convertCharEvent(codepoint);
                                    if (event)
                                        backendWindow->pushEvent(*event);
                                });
            glfwSetKeyCallback(m_window,
                               [](GLFWwindow* window, int key, int scancode, int action, int mods)
                               {
                                   auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                   const auto event = static_cast<Gui*>(backendWindow->getGui())
                                                          ->convertKeyEvent(key, scancode, action, mods);
                                   if (event)
                                       backendWindow->pushEvent(*event);
                               });
            glfwSetScrollCallback(m_window,
                                  [](GLFWwindow* window, double xoffset, double yoffset)
                                  {
                                      auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                      const auto event = static_cast<Gui*>(backendWindow->getGui())->convertScrollEvent(xoffset, yoffset);
                                      if (event)
                                          backendWindow->pushEvent(*event);
                                  });
            glfwSetCursorPosCallback(m_window,
                                     [](GLFWwindow* window, double xpos, double ypos)
                                     {
                                         auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                         const auto event = static_cast<Gui*>(backendWindow->getGui())->convertCursorPosEvent(xpos, ypos);
                                         if (event)
                                             backendWindow->pushEvent(*event);
                                     });
            glfwSetMouseButtonCallback(m_window,
                                       [](GLFWwindow* window, int button, int action, int mods)
                                       {
                                           auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                                           const auto event = static_cast<Gui*>(backendWindow->getGui())
                                                                  ->convertMouseButtonEvent(button, action, mods);
                                           if (event)
                                               backendWindow->pushEvent(*event);
                                       });
        }

        ~BackendWindowGLFW() override
        {
    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            m_swapchain = nullptr;
    #endif
            m_gui = nullptr; // Gui must be destroyed before destroying GLFW window

    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            destroyVulkanDevice(m_vulkanContext);
    #endif

            if (m_window)
                glfwDestroyWindow(m_window);

            glfwTerminate();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            return !glfwWindowShouldClose(m_window);
        }

        void close() override
        {
            glfwSetWindowShouldClose(m_window, 1);
        }

        bool pollEvent(Event& event) override
        {
            if (m_events.empty())
            {
                glfwPollEvents();

                // If the window has been closed then generate a Close event.
                // We will cancel closing the window, as we expect the calling code
                // to call our close() function as result of the Close event.
                // This is to maintain the same behavior between different backends.
                if (!m_closeEventGenerated && glfwWindowShouldClose(m_window))
                {
                    m_closeEventGenerated = true;
                    glfwSetWindowShouldClose(m_window, GLFW_FALSE);
                    event.type = Event::Type::Closed;
                    return true;
                }
            }

            if (!m_events.empty())
            {
                event = m_events.front();
                m_events.pop();
                return true;
            }

            // No new events
            return false;
        }

        void draw() override
        {
    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            if (!m_swapchain || m_swapchain->isSurfaceLost())
            {
                m_swapchain = nullptr; // A window can't have multiple swapchains at the same time
                VkSurfaceKHR surface = VK_NULL_HANDLE;
                if (glfwCreateWindowSurface(m_vulkanContext.instance, m_window, nullptr, &surface) != VK_SUCCESS)
                    throw Exception("Failed to create a Vulkan surface for the window");
                m_swapchain = std::make_unique<SwapchainVulkan>(std::static_pointer_cast<BackendRendererVulkan>(
                                                                    getBackend()->getRenderer()),
                                                                surface);
            }

            int framebufferWidth = 0;
            int framebufferHeight = 0;
            glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);
            drawVulkanFrame(*m_gui,
                            *m_swapchain,
                            {static_cast<unsigned int>(framebufferWidth), static_cast<unsigned int>(framebufferHeight)});
    #else
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            glfwSwapBuffers(m_window);
    #endif
        }

        void mainLoop(Color clearColor) override
        {
    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
            m_swapchain = nullptr; // The gui creates its own swapchain for the window
    #endif
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((Filesystem::Path(getResourcePath()) / filename).asString(), iconSize);
            if (!pixelPtr)
                return;

            GLFWimage image;
            image.width = static_cast<int>(iconSize.x);
            image.height = static_cast<int>(iconSize.y);
            image.pixels = static_cast<unsigned char*>(pixelPtr.get());

            glfwSetWindowIcon(m_window, 1, &image);
        }

        void pushEvent(Event event)
        {
            m_events.emplace(event);
        }

    private:
        GLFWwindow* m_window = nullptr;
    #if TGUI_DEFAULT_WINDOW_USES_VULKAN
        VulkanContext m_vulkanContext;
        std::unique_ptr<SwapchainVulkan> m_swapchain; // Only exists when draw() is used outside the main loop
    #endif
        std::unique_ptr<Gui> m_gui;
        std::queue<Event> m_events;
        bool m_closeEventGenerated = false;
    };

#elif TGUI_HAS_BACKEND_RAYLIB
    class BackendWindowRaylib : public DefaultBackendWindow
    {
    public:
        BackendWindowRaylib(unsigned int width, unsigned int height, const String& title)
        {
            SetTraceLogLevel(LOG_WARNING);
            SetConfigFlags(FLAG_WINDOW_RESIZABLE);
            InitWindow(static_cast<int>(width), static_cast<int>(height), title.toStdString().c_str());
            SetExitKey(KEY_NULL);

            m_gui = std::make_unique<Gui>();
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});

            m_mouseOnWindow = IsCursorOnScreen();
            m_windowOpen = true;
        }

        ~BackendWindowRaylib() override
        {
            m_gui = nullptr; // Gui must be destroyed before destroying raylib window
            CloseWindow();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            // We don't check WindowShouldClose() here because we first want to generate a Close event
            // when the window is closed. The calling code should handle the Close event and call
            // the close() function of this class, which sets m_windowOpen to false.
            // This is to maintain the same behavior between different backends.
            return m_windowOpen;
        }

        void close() override
        {
            m_windowOpen = false;
            m_gui->endMainLoop();
        }

        bool pollEvent(Event& event) override
        {
            if (m_nextEventIndex >= m_events.size())
            {
                if (m_eventsPolled)
                    return false;

                m_events = m_gui->generateEventQueue(true);
                m_nextEventIndex = 0;
                m_eventsPolled = true;

                // If the window has been closed then generate a Close event.
                // The calling code should handle the Close event and call the close() function of this class.
                // This is to maintain the same behavior between different backends.
                if (WindowShouldClose())
                {
                    if (!m_closeEventGenerated)
                    {
                        m_closeEventGenerated = true;
                        event.type = Event::Type::Closed;
                        return true;
                    }
                    // This is the second attempt to close the window (this should not happen), just close it
                    close();
                }
            }

            if (m_nextEventIndex < m_events.size())
            {
                event = std::move(m_events[m_nextEventIndex]);
                ++m_nextEventIndex;
                return true;
            }

            return false;
        }

        void draw() override
        {
            BeginDrawing();
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            EndDrawing();
            m_eventsPolled = false;
        }

        void mainLoop(Color clearColor) override
        {
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((Filesystem::Path(getResourcePath()) / filename).asString(), iconSize);
            if (!pixelPtr)
                return;

            static_assert(sizeof(Image) == sizeof(void*) + 4 * sizeof(int),
                          "Assuming layout of Image class (C++20 aggregate initialization would solve this)");
            Image icon = {const_cast<std::uint8_t*>(pixelPtr.get()),
                          static_cast<int>(iconSize.x),
                          static_cast<int>(iconSize.y),
                          1,
                          PIXELFORMAT_UNCOMPRESSED_R8G8B8A8};
            SetWindowIcon(icon);
        }

    private:
        std::unique_ptr<Gui> m_gui;
        std::vector<Event> m_events;
        std::size_t m_nextEventIndex = 0;
        bool m_mouseOnWindow = false;
        bool m_eventsPolled = false;
        bool m_windowOpen = false;
        bool m_closeEventGenerated = false;
    };

#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<DefaultBackendWindow> DefaultBackendWindow::create(unsigned int width, unsigned int height, const String& title)
    {
#if TGUI_HAS_BACKEND_SFML_GRAPHICS || TGUI_HAS_BACKEND_SFML_OPENGL3
        return std::make_shared<BackendWindowSFML>(width, height, title);
#elif TGUI_HAS_BACKEND_SDL_GPU || TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_GLES2 \
    || TGUI_HAS_BACKEND_SDL_VULKAN || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
        return std::make_shared<BackendWindowSDL>(width, height, title);
#elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2 || TGUI_HAS_BACKEND_GLFW_VULKAN
        return std::make_shared<BackendWindowGLFW>(width, height, title);
#elif TGUI_HAS_BACKEND_RAYLIB
        return std::make_shared<BackendWindowRaylib>(width, height, title);
#else
        TGUI_ASSERT(false, "DefaultBackendWindow can't be used when TGUI was build without a backend");
        (void)width;
        (void)height;
        (void)title;
        return nullptr;
#endif
    }
} // namespace tgui
