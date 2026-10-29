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

#include <TGUI/extlibs/IncludeSDL.hpp>

#include <TGUI/Backend/Renderer/Vulkan/SwapchainVulkan.hpp>
#include <TGUI/Backend/SDL-Vulkan.hpp>

#include <SDL3/SDL_vulkan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    inline namespace SDL_VULKAN
    {
        Gui::Gui() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Gui::Gui(SDL_Window* window, const VulkanContext& vulkanContext)
        {
            setWindow(window, vulkanContext);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Gui::~Gui() = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setWindow(SDL_Window* window, const VulkanContext& vulkanContext)
        {
            if (!isBackendSet())
            {
                // SDL has already loaded the Vulkan loader, so its vkGetInstanceProcAddr is used when none was provided
                VulkanContext rendererContext = vulkanContext;
                if (!rendererContext.getInstanceProcAddr)
                {
                    rendererContext.getInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
                        SDL_Vulkan_GetVkGetInstanceProcAddr());
                }

                auto backend = std::make_shared<BackendSDL>();
                backend->setFontBackend(std::make_shared<BackendFontFactoryImpl<BackendFontFreeType>>());
                backend->setRenderer(std::make_shared<BackendRendererVulkan>(rendererContext));
                backend->setDestroyOnLastGuiDetatch(true);
                setBackend(backend);
            }

            const auto renderer = std::dynamic_pointer_cast<BackendRendererVulkan>(getBackend()->getRenderer());
            if (!renderer)
                throw Exception("SDL_VULKAN::Gui requires that the backend renderer is a BackendRendererVulkan");

            m_backendRenderTarget = std::make_shared<BackendRenderTargetVulkan>(renderer);
            setGuiWindow(window);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setRenderPass(VkRenderPass renderPass,
                                VkFormat colorFormat,
                                std::uint32_t maxFramesInFlight,
                                std::uint32_t subpass,
                                VkSampleCountFlagBits samples)
        {
            TGUI_ASSERT(m_backendRenderTarget, "Gui must be given a window before calling setRenderPass()");
            downcast<BackendRenderTargetVulkan>(m_backendRenderTarget)
                ->setRenderPass(renderPass, colorFormat, maxFramesInFlight, subpass, samples);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setDynamicRenderingFormats(VkFormat colorFormat,
                                             std::uint32_t maxFramesInFlight,
                                             VkFormat depthFormat,
                                             VkFormat stencilFormat,
                                             VkSampleCountFlagBits samples)
        {
            TGUI_ASSERT(m_backendRenderTarget, "Gui must be given a window before calling setDynamicRenderingFormats()");
            downcast<BackendRenderTargetVulkan>(m_backendRenderTarget)
                ->setDynamicRenderingFormats(colorFormat, maxFramesInFlight, depthFormat, stencilFormat, samples);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::draw(VkCommandBuffer cmdBuffer)
        {
            if (m_drawUpdatesTime)
                updateTime();

            TGUI_ASSERT(m_backendRenderTarget, "Gui must be given a window before calling draw()");
            downcast<BackendRenderTargetVulkan>(m_backendRenderTarget)->drawGui(m_container, cmdBuffer);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::presentScreen()
        {
            TGUI_ASSERT(false, "The SDL_VULKAN backend does not use the presentScreen function");
            throw Exception("The SDL_VULKAN backend does not use the presentScreen function");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::mainLoop(Color clearColor)
        {
            TGUI_ASSERT(m_window && m_backendRenderTarget,
                        "Gui must be given an SDL_Window (either at construction or via setWindow function) before mainLoop() is "
                        "called");

            createSwapchain();

            BackendGuiSDL::mainLoop(clearColor);
            m_swapchain = nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::renderFrame(Color clearColor)
        {
            TGUI_ASSERT(m_swapchain, "renderFrame can only be called from within mainLoop");

            // On Android the surface is destroyed when the app goes to the background, so we need a new one afterwards
            if (m_swapchain->isSurfaceLost())
                createSwapchain();

            int framebufferWidth = 0;
            int framebufferHeight = 0;
            SDL_GetWindowSizeInPixels(m_window, &framebufferWidth, &framebufferHeight);

            const VkCommandBuffer cmdBuffer = m_swapchain->beginFrame({static_cast<unsigned int>(framebufferWidth),
                                                                       static_cast<unsigned int>(framebufferHeight)},
                                                                      clearColor);
            if (cmdBuffer == VK_NULL_HANDLE)
                return; // The window is probably minimized

            setRenderPass(m_swapchain->getRenderPass(), m_swapchain->getColorFormat(), SwapchainVulkan::FramesInFlight);
            draw(cmdBuffer);
            m_swapchain->endFrame();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::createSwapchain()
        {
            // The old swapchain is destroyed first, as a window can't have multiple swapchains at the same time
            m_swapchain = nullptr;

            const auto renderer = std::static_pointer_cast<BackendRendererVulkan>(getBackend()->getRenderer());
            VkSurfaceKHR surface = VK_NULL_HANDLE;
            if (!SDL_Vulkan_CreateSurface(m_window, renderer->getVulkanContext().instance, nullptr, &surface))
            {
                throw Exception(
                    "SDL_VULKAN backend failed to create a Vulkan surface for the window. Error: " + String(SDL_GetError()));
            }
            m_swapchain = std::make_unique<SwapchainVulkan>(renderer, surface);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    } // namespace SDL_VULKAN

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
