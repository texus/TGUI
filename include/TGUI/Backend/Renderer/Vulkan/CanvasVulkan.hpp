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

#ifndef TGUI_CANVAS_VULKAN_HPP
#define TGUI_CANVAS_VULKAN_HPP

#include <TGUI/Backend/Renderer/BackendRenderTarget.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendTextureVulkan.hpp>

#include <TGUI/Widgets/CanvasBase.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief CanvasVulkan provides a way to directly render Vulkan contents on a widget
    ///
    /// When gui.draw() is called, all widgets are drawn at once. If you wish to have custom Vulkan rendering inbetween
    /// TGUI widgets (e.g. draw to the background of a child window) then you need to use a CanvasVulkan widget.
    ///
    /// The canvas widget is essentially just a wrapper around an image with the VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT flag.
    /// You draw your Vulkan contents on top of the canvas instead of on the window. The canvas is then added to the gui
    /// between the widgets where you want the rendering to appear.
    ///
    /// The image has the VK_FORMAT_R8G8B8A8_UNORM format. It is in the VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout
    /// when the gui is drawn, so a render pass that draws to the image must leave it in that layout. The initialLayout
    /// of the attachment can be VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL (or VK_IMAGE_LAYOUT_UNDEFINED when clearing it).
    /// The render pass must be submitted to the same queue as the gui and it needs external dependencies in both directions:
    /// - A dependency from VK_SUBPASS_EXTERNAL to your subpass, with VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT in its
    ///   srcStageMask. Your rendering may otherwise overwrite the image while the gui is still reading from it in an earlier
    ///   frame, or while TGUI is still clearing it after the canvas was resized.
    /// - A dependency from your subpass to VK_SUBPASS_EXTERNAL, with VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT in its
    ///   dstStageMask and VK_ACCESS_SHADER_READ_BIT in its dstAccessMask. The implicit dependency at the end of a render
    ///   pass doesn't make your rendering visible to the gui, which would otherwise read the image before you finished
    ///   drawing to it.
    ///
    /// When using dynamic rendering instead of a render pass, pipeline barriers with the same stage and access masks have to
    /// be recorded before and after the rendering, which also have to transition the layout of the image.
    ///
    /// Example:
    /// @code
    /// VkAttachmentDescription attachment = {};
    /// attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    /// attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    /// attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    /// attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    /// attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    /// attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    /// attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Use VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL with LOAD_OP_LOAD
    /// attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ///
    /// std::array<VkSubpassDependency, 2> dependencies = {};
    /// // Wait for the gui to stop reading from the image (and for TGUI to finish clearing it) before drawing to it
    /// dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    /// dependencies[0].dstSubpass = 0;
    /// dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    /// dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    /// dependencies[0].srcAccessMask = 0;
    /// dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    /// // Make sure the gui only reads from the image after drawing to it has finished
    /// dependencies[1].srcSubpass = 0;
    /// dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    /// dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    /// dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    /// dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    /// dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    ///
    /// // Pass the attachment and dependencies to vkCreateRenderPass to create canvasRenderPass
    ///
    /// VkRenderPassBeginInfo renderPassInfo = {};
    /// renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    /// renderPassInfo.renderPass = canvasRenderPass; // finalLayout must be VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    /// renderPassInfo.framebuffer = canvasFramebuffer; // Created with canvas->getImageView()
    /// renderPassInfo.renderArea.extent = {canvasWidth, canvasHeight};
    /// vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    /// // Draw to the image of the canvas
    /// vkCmdEndRenderPass(cmdBuffer);
    /// @endcode
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API CanvasVulkan : public CanvasBase
    {
    public:
        using Ptr = std::shared_ptr<CanvasVulkan>;            //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const CanvasVulkan>; //!< Shared constant widget pointer

        static constexpr char StaticWidgetType[] = "CanvasVulkan"; //!< Type name of the widget

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Constructor
        /// @param typeName     Type of the widget
        /// @param initRenderer Should the renderer be initialized? Should be true unless a derived class initializes it.
        /// @see create
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        explicit CanvasVulkan(const char* typeName = StaticWidgetType, bool initRenderer = true);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CanvasVulkan(const CanvasVulkan& copy);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move constructor
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CanvasVulkan(CanvasVulkan&& copy) = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of copy assignment operator
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CanvasVulkan& operator=(const CanvasVulkan& right);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move assignment operator
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CanvasVulkan& operator=(CanvasVulkan&& right) = default;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new canvas widget
        ///
        /// @param size  Size of the canvas
        ///
        /// @return The new canvas
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] static CanvasVulkan::Ptr create(const Layout2d& size = {"100%", "100%"});

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Makes a copy of another canvas
        ///
        /// @param canvas  The other canvas
        ///
        /// @return The new canvas
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] static CanvasVulkan::Ptr copy(const CanvasVulkan::ConstPtr& canvas);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the size of the widget
        ///
        /// @param size  The new size of the widget
        ///
        /// The internal image is recreated (and cleared) when the size changes.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSize(const Layout2d& size) override;
        using Widget::setSize;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draw the widget to a render target
        ///
        /// @param target Render target to draw to
        /// @param states Current render states
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void draw(BackendRenderTarget& target, RenderStates states) const override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Access the internal image on which you can render
        ///
        /// @return Vulkan image, or VK_NULL_HANDLE if the canvas has no size
        ///
        /// The image has a size that is equal to the size of the canvas widget.
        /// It was created with the VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_SAMPLED_BIT and
        /// VK_IMAGE_USAGE_TRANSFER_DST_BIT usage flags.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkImage getImage() const
        {
            return m_backendTexture->getInternalImage();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Access the view of the internal image on which you can render
        ///
        /// @return Vulkan image view, or VK_NULL_HANDLE if the canvas has no size
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkImageView getImageView() const
        {
            return m_backendTexture->getInternalImageView();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the widget
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] Widget::Ptr clone() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        Vector2u m_textureSize;
        std::shared_ptr<BackendTextureVulkan> m_backendTexture;
    };
} // namespace tgui

#endif // TGUI_CANVAS_VULKAN_HPP
