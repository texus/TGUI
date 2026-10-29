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

#ifndef TGUI_BACKEND_TEXTURE_VULKAN_HPP
#define TGUI_BACKEND_TEXTURE_VULKAN_HPP

#include <TGUI/Backend/Renderer/BackendTexture.hpp>

#include <vulkan/vulkan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class BackendRendererVulkan;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Texture implementation that uses Vulkan
    ///
    /// The image has the VK_FORMAT_R8G8B8A8_UNORM format and is kept in the VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL layout.
    ///
    /// Loading the texture submits the upload to the queue without waiting for it to finish. Work that accesses the image
    /// must be submitted to the same queue, with a dependency on VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT as source stage.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendTextureVulkan : public BackendTexture
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Vulkan objects that make up the texture
        ///
        /// These objects are shared with the draw commands of the render target, so that they aren't destroyed while
        /// the GPU might still be using them (e.g. when the texture is destroyed or resized).
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct ImageResources
        {
            ImageResources(std::shared_ptr<BackendRendererVulkan> renderer);
            ~ImageResources();

            ImageResources(const ImageResources&) = delete;
            ImageResources& operator=(const ImageResources&) = delete;

            std::shared_ptr<BackendRendererVulkan> renderer;
            VkImage image = VK_NULL_HANDLE;
            VkDeviceMemory memory = VK_NULL_HANDLE;
            VkImageView imageView = VK_NULL_HANDLE;
            VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
            VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
            std::uint64_t uploadId = 0; // Last upload to the image, the destructor waits for it to finish
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param renderer             Renderer that holds the Vulkan device
        /// @param additionalUsageFlags Usage flags for the image, on top of VK_IMAGE_USAGE_SAMPLED_BIT and
        ///                             VK_IMAGE_USAGE_TRANSFER_DST_BIT which are always set.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendTextureVulkan(std::shared_ptr<BackendRendererVulkan> renderer, VkImageUsageFlags additionalUsageFlags = 0);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from an array of 32-bits RGBA pixels, but don't take ownership of the pixels
        ///
        /// @param size   Width and height of the image to create
        /// @param pixels Pointer to array of size.x*size.y*4 bytes with RGBA pixels, or nullptr to create an empty texture
        /// @param smooth Should the smooth filter be enabled or not?
        ///
        /// @warning Unlike the load function, loadTextureOnly won't store the pixels and isTransparentPixel thus won't work
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth) override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the internal image
        ///
        /// @return Vulkan image, or VK_NULL_HANDLE if the texture hasn't been loaded yet
        ///
        /// The image is recreated when the texture is loaded with a different size.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkImage getInternalImage() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the view of the internal image
        ///
        /// @return Vulkan image view, or VK_NULL_HANDLE if the texture hasn't been loaded yet
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkImageView getInternalImageView() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns the Vulkan objects that make up the texture
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] const std::shared_ptr<ImageResources>& getImageResources() const
        {
            return m_imageResources;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Creates the image, its memory, the image view and the descriptor set
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] std::shared_ptr<ImageResources> createImageResources(Vector2u size) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        std::shared_ptr<BackendRendererVulkan> m_renderer;
        VkImageUsageFlags m_usageFlags;
        std::shared_ptr<ImageResources> m_imageResources;
    };
} // namespace tgui

#endif // TGUI_BACKEND_TEXTURE_VULKAN_HPP
