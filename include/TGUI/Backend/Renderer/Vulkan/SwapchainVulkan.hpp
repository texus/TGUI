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

#ifndef TGUI_SWAPCHAIN_VULKAN_HPP
#define TGUI_SWAPCHAIN_VULKAN_HPP

#include <TGUI/Color.hpp>
#include <TGUI/Vector2.hpp>

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class BackendRendererVulkan;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @internal
    /// @brief Swapchain that is used by the Vulkan backends to render to a window when gui.mainLoop() is called
    ///
    /// This class manages the swapchain, the render pass, the framebuffers, the command buffers and the synchronization
    /// objects that are needed to present frames to a window. It is an implementation detail of the Gui classes and
    /// DefaultBackendWindow, and isn't exported from the library. When you render the gui yourself, your own swapchain
    /// is used instead. Since a window can only have one swapchain, the object should only exist while it is being used.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class SwapchainVulkan
    {
    public:
        // Amount of frames that can be executing on the GPU at the same time, which has to be passed to setRenderPass.
        // With FIFO presentation, more frames would only increase the input latency.
        static constexpr std::uint32_t FramesInFlight = 2;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param renderer  Renderer that holds the Vulkan device
        /// @param surface   Surface of the window. Ownership is transferred, the destructor will destroy the surface.
        ///
        /// @throw Exception when the queue of the renderer can't present to the surface or when an object couldn't be created
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SwapchainVulkan(std::shared_ptr<BackendRendererVulkan> renderer, VkSurfaceKHR surface);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~SwapchainVulkan();

        SwapchainVulkan(const SwapchainVulkan&) = delete;
        SwapchainVulkan& operator=(const SwapchainVulkan&) = delete;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Starts a new frame
        ///
        /// @param framebufferSize  Size of the window in pixels
        /// @param clearColor       Color to clear the window with
        ///
        /// @return Command buffer in which a render pass was started, or VK_NULL_HANDLE if nothing can be rendered now
        ///         (e.g. because the window is minimized). The endFrame function must only be called when the returned
        ///         command buffer wasn't VK_NULL_HANDLE.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkCommandBuffer beginFrame(Vector2u framebufferSize, Color clearColor);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Ends the render pass, submits the command buffer and presents the frame
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void endFrame();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the render pass that is started by beginFrame
        ///
        /// @return Render pass, which remains the same during the lifetime of this object
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkRenderPass getRenderPass() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the format of the swapchain images
        ///
        /// @return Color format of the attachment in the render pass
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkFormat getColorFormat() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether the surface was lost and the swapchain can no longer be used
        ///
        /// @return True when the surface was lost, in which case a new object has to be created with a new surface
        ///
        /// This happens on Android when the app is put in the background, as the window surface is destroyed then.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] bool isSurfaceLost() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Creates the render pass, command buffers and synchronization objects
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void createVulkanObjects();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destroys the objects that were created in createVulkanObjects
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void destroyVulkanObjects();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Creates the swapchain and the objects that depend on it. Returns false if the window has no size.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool recreateSwapchain(Vector2u framebufferSize);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destroys the image views, framebuffers and semaphores that depend on the swapchain images
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void destroySwapchainImageResources();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        struct FrameSync
        {
            VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
            VkFence inFlightFence = VK_NULL_HANDLE;
            VkSemaphore imageAvailableSemaphore = VK_NULL_HANDLE;
        };

        std::shared_ptr<BackendRendererVulkan> m_renderer;
        VkDevice m_device = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSurfaceFormatKHR m_surfaceFormat = {};
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkExtent2D m_extent = {};
        Vector2u m_framebufferSize; // Window size that was passed when the swapchain was last created
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        VkCommandPool m_commandPool = VK_NULL_HANDLE;

        // One per swapchain image
        std::vector<VkImageView> m_imageViews;
        std::vector<VkFramebuffer> m_framebuffers;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;

        std::vector<FrameSync> m_frames;
        std::size_t m_currentFrame = 0;
        std::uint32_t m_imageIndex = 0;
        bool m_swapchainOutdated = false;
        bool m_surfaceLost = false;
        bool m_rotatedByCompositor = false;
    };
} // namespace tgui

#endif // TGUI_SWAPCHAIN_VULKAN_HPP
