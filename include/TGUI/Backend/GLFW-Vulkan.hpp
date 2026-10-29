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

#ifndef TGUI_BACKEND_GLFW_VULKAN_INCLUDE_HPP
#define TGUI_BACKEND_GLFW_VULKAN_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_GLFW_VULKAN
    #error "TGUI wasn't build with the GLFW_VULKAN backend"
#endif

#include <TGUI/Backend/Font/FreeType/BackendFontFreeType.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendRendererVulkan.hpp>
#include <TGUI/Backend/Window/GLFW/BackendGLFW.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using GLFWwindow = struct GLFWwindow;

namespace tgui
{
    class SwapchainVulkan;

    inline namespace GLFW_VULKAN
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for GLFW, using Vulkan for rendering and FreeType for fonts
        ///
        /// When using the mainLoop() function, TGUI will create the swapchain and surface internally.
        /// Otherwise you must call setRenderPass or setDynamicRenderingFormats once before using the draw function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API Gui : public BackendGuiGLFW
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Default constructor
            ///
            /// @warning You must still call setWindow on the Gui before using any TGUI functions
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui();

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Constructor that immediately sets the window and Vulkan objects with which the gui should be drawn
            ///
            /// @param window         The GLFW window that will be used by the gui (created with GLFW_CLIENT_API set to GLFW_NO_API)
            /// @param vulkanContext  Vulkan objects that will be used for rendering
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(GLFWwindow* window, const VulkanContext& vulkanContext);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Destructor
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ~Gui() override;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window and Vulkan objects with which the gui should be drawn
            ///
            /// @param window         The GLFW window that will be used by the gui (created with GLFW_CLIENT_API set to GLFW_NO_API)
            /// @param vulkanContext  Vulkan objects that will be used for rendering
            ///
            /// @warning The vulkanContext is ignored if the backend was already created (e.g. by another Gui object).
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(GLFWwindow* window, const VulkanContext& vulkanContext);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Informs the gui about the render pass in which draw(cmdBuffer) will be called
            ///
            /// @param renderPass         Render pass that will be active when draw is called
            /// @param colorFormat        Format of the color attachment of the subpass
            /// @param maxFramesInFlight  Maximum number of frames that your render loop lets the GPU execute at the same time
            ///                           (e.g. the amount of per-frame fences that you wait on). When draw(cmdBuffer) is called,
            ///                           the frame that was drawn maxFramesInFlight frames earlier must have finished executing,
            ///                           otherwise TGUI can overwrite buffers that the GPU is still reading from. A value that is
            ///                           higher than needed only costs a bit of memory.
            /// @param subpass            Index of the subpass that will be active when draw is called
            /// @param samples            Amount of samples of the color attachment
            ///
            /// This function doesn't need to be called when using gui.mainLoop().
            /// The subpass must have exactly one color attachment. sRGB formats are supported, but alpha blending then
            /// happens in linear space, so semi-transparent parts can look slightly different than with a UNORM format.
            /// Changing maxFramesInFlight waits until the queue is idle, so it shouldn't be changed while a command buffer
            /// in which the gui was drawn still has to be submitted.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setRenderPass(VkRenderPass renderPass,
                               VkFormat colorFormat,
                               std::uint32_t maxFramesInFlight,
                               std::uint32_t subpass = 0,
                               VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Informs the gui that draw(cmdBuffer) will be called while using dynamic rendering (vkCmdBeginRendering)
            ///
            /// @param colorFormat        Format of the color attachment
            /// @param maxFramesInFlight  Maximum number of frames that your render loop lets the GPU execute at the same time
            ///                           (e.g. the amount of per-frame fences that you wait on). When draw(cmdBuffer) is called,
            ///                           the frame that was drawn maxFramesInFlight frames earlier must have finished executing,
            ///                           otherwise TGUI can overwrite buffers that the GPU is still reading from. A value that is
            ///                           higher than needed only costs a bit of memory.
            /// @param depthFormat        Format of the depth attachment, or VK_FORMAT_UNDEFINED if there is none
            /// @param stencilFormat      Format of the stencil attachment, or VK_FORMAT_UNDEFINED if there is none
            /// @param samples            Amount of samples of the attachments
            ///
            /// This function doesn't need to be called when using gui.mainLoop().
            /// Dynamic rendering requires Vulkan 1.3 or the VK_KHR_dynamic_rendering extension, and the dynamicRendering feature
            /// has to be enabled on the device.
            /// Exactly one color attachment must be used. sRGB formats are supported, but alpha blending then happens in linear space,
            /// so semi-transparent parts can look slightly different than with a UNORM format.
            /// Changing maxFramesInFlight waits until the queue is idle, so it shouldn't be changed while a command buffer
            /// in which the gui was drawn still has to be submitted.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setDynamicRenderingFormats(VkFormat colorFormat,
                                            std::uint32_t maxFramesInFlight,
                                            VkFormat depthFormat = VK_FORMAT_UNDEFINED,
                                            VkFormat stencilFormat = VK_FORMAT_UNDEFINED,
                                            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief This function is not used by this backend, use draw(VkCommandBuffer) instead
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TGUI_DEPRECATED("This backend requires a command buffer, use draw(VkCommandBuffer) instead") void draw() override
            {
                TGUI_ASSERT(false, "The GLFW_VULKAN backend requires that you pass a VkCommandBuffer to the draw function");
                throw Exception("The GLFW_VULKAN backend requires that you pass a VkCommandBuffer to the draw function");
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Draws all the widgets that were added to the gui
            ///
            /// @param cmdBuffer  Command buffer to record the draw commands in, which must be inside an active render pass
            ///
            /// This function should be called once per frame. The command buffer has to be submitted to the queue that
            /// was passed to the gui. Before calling this function, setRenderPass or setDynamicRenderingFormats must have
            /// been called.
            ///
            /// @warning Bound pipeline, vertex buffer, index buffer and descriptor sets will be changed by this function
            /// @warning Viewport and scissor are changed by this function. The scissor will contain the entire window afterwards.
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void draw(VkCommandBuffer cmdBuffer);

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Give the gui control over the main loop
            ///
            /// @param clearColor  background color of the window
            ///
            /// A surface and swapchain are created for the window when this function is called, and they are destroyed
            /// again before the function returns. This function can thus not be used while you have your own swapchain
            /// for the window.
            ///
            /// @see BackendGuiGLFW::mainLoop
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void mainLoop(Color clearColor = {240, 240, 240}) override;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////

        protected:
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Clears the window, draws the gui and then presents the frame
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void renderFrame(Color clearColor) override;

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Creates a new surface and swapchain for the window, replacing the existing ones
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void createSwapchain();

        protected:
            std::unique_ptr<SwapchainVulkan> m_swapchain; // Only exists while mainLoop is running
        };
    } // namespace GLFW_VULKAN
} // namespace tgui

#endif // TGUI_BACKEND_GLFW_VULKAN_INCLUDE_HPP
