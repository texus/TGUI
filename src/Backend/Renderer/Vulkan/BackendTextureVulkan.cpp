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

#include <TGUI/Backend/Renderer/Vulkan/BackendRendererVulkan.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendTextureVulkan.hpp>

#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    BackendTextureVulkan::ImageResources::ImageResources(std::shared_ptr<BackendRendererVulkan> rendererVulkan) :
        renderer{std::move(rendererVulkan)}
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureVulkan::ImageResources::~ImageResources()
    {
        const auto& fn = renderer->getFunctions();

        // The image can't be destroyed while the GPU is still copying pixels to it
        if (uploadId != 0)
            renderer->waitForUpload(uploadId);

        if (descriptorSet != VK_NULL_HANDLE)
            renderer->freeTextureDescriptorSet(descriptorSet, descriptorPool);

        const VkDevice device = renderer->getVulkanContext().device;
        fn.vkDestroyImageView(device, imageView, nullptr);
        fn.vkDestroyImage(device, image, nullptr);
        fn.vkFreeMemory(device, memory, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureVulkan::BackendTextureVulkan(std::shared_ptr<BackendRendererVulkan> renderer, VkImageUsageFlags additionalUsageFlags) :
        m_renderer{std::move(renderer)},
        m_usageFlags{VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | additionalUsageFlags}
    {
        TGUI_ASSERT(m_renderer, "renderer passed to BackendTextureVulkan can't be a nullptr");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureVulkan::loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth)
    {
        const auto& fn = m_renderer->getFunctions();

        // The smooth parameter doesn't matter here, because the sampler is chosen when drawing
        const bool reuseImage = (m_imageResources && (size.x == m_imageSize.x) && (size.y == m_imageSize.y));

        BackendTexture::loadTextureOnly(size, pixels, smooth);

        if (reuseImage && !pixels)
            return true;

        // When the size changes, a new image is created. The old image is kept alive by draw commands that still use it.
        if (!reuseImage)
        {
            m_imageResources = createImageResources(size);
            if (!m_imageResources)
                return false;
        }

        // Copy the pixels to a staging buffer
        const VkDevice device = m_renderer->getVulkanContext().device;
        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
        if (pixels)
        {
            const VkDeviceSize dataSize = static_cast<VkDeviceSize>(size.x) * size.y * 4;
            if (!m_renderer->createBuffer(dataSize,
                                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                          stagingBuffer,
                                          stagingMemory))
            {
                m_imageResources = nullptr;
                return false;
            }

            m_renderer->setObjectName(VK_OBJECT_TYPE_BUFFER, stagingBuffer, "TGUI texture staging buffer");
            m_renderer->setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, stagingMemory, "TGUI texture staging buffer memory");

            void* data = nullptr;
            if (fn.vkMapMemory(device, stagingMemory, 0, dataSize, 0, &data) != VK_SUCCESS)
            {
                fn.vkDestroyBuffer(device, stagingBuffer, nullptr);
                fn.vkFreeMemory(device, stagingMemory, nullptr);
                m_imageResources = nullptr;
                return false;
            }

            std::memcpy(data, pixels, static_cast<std::size_t>(dataSize));
            fn.vkUnmapMemory(device, stagingMemory);
        }

        // Copy the pixels to the image (or clear the image when no pixels were provided) and transition its layout.
        // The renderer takes ownership of the staging buffer and destroys it once the GPU has finished the upload.
        const VkImage image = m_imageResources->image;
        const std::uint64_t uploadId = m_renderer->submitUploadCommands(
            [&](VkCommandBuffer cmdBuffer)
            {
                // The old contents of the image are discarded. If the image is being reused then the barrier has to wait
                // until earlier frames that might still be drawing the image are finished reading from it, and until
                // earlier uploads to the image are finished writing to it.
                VkImageMemoryBarrier barrier = {};
                barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                barrier.srcAccessMask = reuseImage ? VK_ACCESS_TRANSFER_WRITE_BIT : 0;
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                barrier.image = image;
                barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.levelCount = 1;
                barrier.subresourceRange.layerCount = 1;
                fn.vkCmdPipelineBarrier(cmdBuffer,
                                        reuseImage ? (VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT)
                                                   : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        0,
                                        0,
                                        nullptr,
                                        0,
                                        nullptr,
                                        1,
                                        &barrier);

                if (pixels)
                {
                    VkBufferImageCopy region = {};
                    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    region.imageSubresource.layerCount = 1;
                    region.imageExtent = {size.x, size.y, 1};
                    fn.vkCmdCopyBufferToImage(cmdBuffer, stagingBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
                }
                else
                {
                    const VkClearColorValue clearColor = {};
                    fn.vkCmdClearColorImage(cmdBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &barrier.subresourceRange);
                }

                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                fn.vkCmdPipelineBarrier(cmdBuffer,
                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                        0,
                                        0,
                                        nullptr,
                                        0,
                                        nullptr,
                                        1,
                                        &barrier);
            },
            stagingBuffer,
            stagingMemory);

        // Don't keep an image that might not be in the expected layout
        if (uploadId == 0)
        {
            m_imageResources = nullptr;
            return false;
        }

        m_imageResources->uploadId = uploadId;
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkImage BackendTextureVulkan::getInternalImage() const
    {
        return m_imageResources ? m_imageResources->image : VK_NULL_HANDLE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkImageView BackendTextureVulkan::getInternalImageView() const
    {
        return m_imageResources ? m_imageResources->imageView : VK_NULL_HANDLE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTextureVulkan::ImageResources> BackendTextureVulkan::createImageResources(Vector2u size) const
    {
        const auto& fn = m_renderer->getFunctions();

        const VkDevice device = m_renderer->getVulkanContext().device;
        auto resources = std::make_shared<ImageResources>(m_renderer);

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent = {size.x, size.y, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = m_usageFlags;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        if (fn.vkCreateImage(device, &imageInfo, nullptr, &resources->image) != VK_SUCCESS)
            return nullptr;

        m_renderer->setObjectName(VK_OBJECT_TYPE_IMAGE, resources->image, "TGUI texture image");

        VkMemoryRequirements memoryRequirements;
        fn.vkGetImageMemoryRequirements(device, resources->image, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = m_renderer->findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (allocInfo.memoryTypeIndex >= VK_MAX_MEMORY_TYPES)
            allocInfo.memoryTypeIndex = m_renderer->findMemoryType(memoryRequirements.memoryTypeBits, 0);
        if (allocInfo.memoryTypeIndex >= VK_MAX_MEMORY_TYPES)
            return nullptr;

        if (fn.vkAllocateMemory(device, &allocInfo, nullptr, &resources->memory) != VK_SUCCESS)
            return nullptr;

        m_renderer->setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, resources->memory, "TGUI texture image memory");

        if (fn.vkBindImageMemory(device, resources->image, resources->memory, 0) != VK_SUCCESS)
            return nullptr;

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = resources->image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.layerCount = 1;
        if (fn.vkCreateImageView(device, &viewInfo, nullptr, &resources->imageView) != VK_SUCCESS)
            return nullptr;

        m_renderer->setObjectName(VK_OBJECT_TYPE_IMAGE_VIEW, resources->imageView, "TGUI texture image view");

        resources->descriptorSet = m_renderer->allocateTextureDescriptorSet(resources->imageView, resources->descriptorPool);
        if (resources->descriptorSet == VK_NULL_HANDLE)
            return nullptr;

        return resources;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
