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

#ifndef TGUI_BACKEND_RENDER_TARGET_VULKAN_HPP
#define TGUI_BACKEND_RENDER_TARGET_VULKAN_HPP

#include <TGUI/Backend/Renderer/BackendRenderTarget.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendTextureVulkan.hpp>

#include <vulkan/vulkan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class BackendRendererVulkan;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Render target implementation that uses Vulkan
    ///
    /// Before the gui can be drawn, either setRenderPass or setDynamicRenderingFormats has to be called to specify where
    /// the gui will be rendered to.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRenderTargetVulkan : public BackendRenderTarget
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param renderer  Renderer that holds the Vulkan device
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendRenderTargetVulkan(std::shared_ptr<BackendRendererVulkan> renderer);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        ///
        /// The destructor waits until the queue is idle, to make sure the GPU no longer uses any of the resources.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendRenderTargetVulkan() override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function isn't used by this backend
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setClearColor(const Color& color) override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function isn't used by this backend
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void clearScreen() override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the render target about which part of the window is used for rendering
        ///
        /// @param view        Defines which part of the gui is being shown
        /// @param viewport    Defines which part of the window is being rendered to
        /// @param targetSize  Size of the window
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setView(FloatRect view, FloatRect viewport, Vector2f targetSize) override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is not used by this backend, use drawGui(root, VkCommandBuffer) instead
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawGui(const std::shared_ptr<RootContainer>&) override
        {
            TGUI_ASSERT(false, "The Vulkan backend requires that you pass a VkCommandBuffer to the draw function");
            throw Exception("The Vulkan backend requires that you pass a VkCommandBuffer to the draw function");
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the render target about the render pass in which the gui will be drawn
        ///
        /// @param renderPass         Render pass that will be active when drawGui is called
        /// @param colorFormat        Format of the color attachment of the subpass
        /// @param maxFramesInFlight  Maximum number of frames that your render loop lets the GPU execute at the same time
        ///                           (e.g. the amount of per-frame fences that you wait on). When drawGui is called, the
        ///                           frame that was drawn maxFramesInFlight frames earlier must have finished executing,
        ///                           otherwise TGUI can overwrite buffers that the GPU is still reading from. A value that
        ///                           is higher than needed only costs a bit of memory.
        /// @param subpass            Index of the subpass that will be active when drawGui is called
        /// @param samples            Amount of samples of the color attachment
        ///
        /// The subpass must have exactly one color attachment. sRGB formats are supported, but alpha blending then happens
        /// in linear space, so semi-transparent parts can look slightly different than with a UNORM format.
        /// Changing maxFramesInFlight waits until the queue is idle, as the resources of all frames are recreated. It thus
        /// shouldn't be changed while a command buffer in which the gui was drawn still has to be submitted.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setRenderPass(VkRenderPass renderPass,
                           VkFormat colorFormat,
                           std::uint32_t maxFramesInFlight,
                           std::uint32_t subpass = 0,
                           VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the render target that the gui will be drawn with dynamic rendering (vkCmdBeginRendering)
        ///
        /// @param colorFormat        Format of the color attachment
        /// @param maxFramesInFlight  Maximum number of frames that your render loop lets the GPU execute at the same time
        ///                           (e.g. the amount of per-frame fences that you wait on). When drawGui is called, the
        ///                           frame that was drawn maxFramesInFlight frames earlier must have finished executing,
        ///                           otherwise TGUI can overwrite buffers that the GPU is still reading from. A value that
        ///                           is higher than needed only costs a bit of memory.
        /// @param depthFormat        Format of the depth attachment, or VK_FORMAT_UNDEFINED if there is none
        /// @param stencilFormat      Format of the stencil attachment, or VK_FORMAT_UNDEFINED if there is none
        /// @param samples            Amount of samples of the attachments
        ///
        /// Exactly one color attachment must be used. sRGB formats are supported, but alpha blending then happens in
        /// linear space, so semi-transparent parts can look slightly different than with a UNORM format.
        /// Changing maxFramesInFlight waits until the queue is idle, as the resources of all frames are recreated. It thus
        /// shouldn't be changed while a command buffer in which the gui was drawn still has to be submitted.
        /// Dynamic rendering requires Vulkan 1.3 or the VK_KHR_dynamic_rendering extension, and the dynamicRendering feature
        /// has to be enabled on the device.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setDynamicRenderingFormats(VkFormat colorFormat,
                                        std::uint32_t maxFramesInFlight,
                                        VkFormat depthFormat = VK_FORMAT_UNDEFINED,
                                        VkFormat stencilFormat = VK_FORMAT_UNDEFINED,
                                        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws the gui and all of its widgets
        ///
        /// @param root       Root container that holds all widgets in the gui
        /// @param cmdBuffer  Command buffer to record the draw commands in, which must be inside an active render pass
        ///
        /// @return True if there weren't any errors, false if something went wrong (e.g. could not create a buffer)
        ///
        /// This function should be called once per frame. Textures may still be uploaded to the GPU during this call,
        /// which happens by submitting a separate command buffer to the queue.
        /// The command buffer must be submitted to the queue that was passed to the renderer in the VulkanContext,
        /// because the texture uploads are only synchronized with work that is submitted to that same queue.
        ///
        /// @warning Bound pipeline, vertex buffer, index buffer and descriptor sets will be changed by this function
        /// @warning Viewport and scissor are changed by this function. The scissor will contain the entire target afterwards.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool drawGui(const std::shared_ptr<RootContainer>& root, VkCommandBuffer cmdBuffer);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a vertex array. This is called by the other draw functions.
        ///
        /// @param states       Render states to use for drawing
        /// @param vertices     Pointer to first element in array of vertices
        /// @param vertexCount  Amount of elements in the vertex array
        /// @param indices      Pointer to first element in array of indices
        /// @param indexCount   Amount of elements in the indices array
        /// @param texture      Texture to use, or nullptr when drawing colored triangles
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawVertexArray(const RenderStates& states,
                             const Vertex* vertices,
                             std::size_t vertexCount,
                             const unsigned int* indices,
                             std::size_t indexCount,
                             const std::shared_ptr<BackendTexture>& texture) override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Called from addClippingLayer and removeClippingLayer to apply the clipping
        ///
        /// @param clipRect      View rectangle to apply
        /// @param clipViewport  Viewport to apply
        ///
        /// Both rectangles may be empty when nothing that will be drawn is going to be visible.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateClipping(FloatRect clipRect, FloatRect clipViewport) override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Removes the current pipeline. It is only destroyed once the frames that might still be using it have finished.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void retirePipeline();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Changes the amount of frame resources. Waits until the queue is idle when the amount changes.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMaxFramesInFlight(std::uint32_t maxFramesInFlight);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destroys the buffers and retired pipelines of all frames
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void destroyFrameResources();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        struct DrawCommand
        {
            std::size_t indexCount;
            std::shared_ptr<BackendTextureVulkan::ImageResources> imageResources;
            bool smooth;
            VkRect2D clipRect;
        };

        // Resources that can only be reused once the GPU has finished executing the frame
        struct FrameResources
        {
            VkBuffer buffer = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkDeviceSize bufferSize = 0;
            void* mappedMemory = nullptr;
            std::vector<DrawCommand> drawCommands; // Keeps the used images alive until the frame is no longer in flight
            std::vector<VkPipeline> retiredPipelines;
        };

        std::shared_ptr<BackendRendererVulkan> m_renderer;
        std::unique_ptr<BackendTextureVulkan> m_emptyTexture;

        Transform m_projectionTransform;

        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkRenderPass m_renderPass = VK_NULL_HANDLE;
        std::uint32_t m_subpass = 0;
        VkFormat m_colorFormat = VK_FORMAT_UNDEFINED;
        VkFormat m_depthFormat = VK_FORMAT_UNDEFINED;
        VkFormat m_stencilFormat = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits m_samples = VK_SAMPLE_COUNT_1_BIT;

        std::vector<FrameResources> m_frames;
        std::size_t m_currentFrame = 0;

        // Used while drawGui is being executed
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        VkRect2D m_clipRect = {};
    };
} // namespace tgui

#endif // TGUI_BACKEND_RENDER_TARGET_VULKAN_HPP
