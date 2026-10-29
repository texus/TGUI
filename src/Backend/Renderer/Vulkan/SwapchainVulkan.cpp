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
#include <TGUI/Backend/Renderer/Vulkan/SwapchainVulkan.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    SwapchainVulkan::SwapchainVulkan(std::shared_ptr<BackendRendererVulkan> renderer, VkSurfaceKHR surface) :
        m_renderer{std::move(renderer)},
        m_device{m_renderer->getVulkanContext().device},
        m_surface{surface},
        m_frames(FramesInFlight)
    {
        const auto& fn = m_renderer->getFunctions();
        const VulkanContext& vulkanContext = m_renderer->getVulkanContext();

        // The surface is owned by this object, but the destructor won't be called if the constructor throws
        try
        {
            // The surface and swapchain functions are only available when the required extensions were enabled
            if (!fn.vkDestroySurfaceKHR || !fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR || !fn.vkGetPhysicalDeviceSurfaceFormatsKHR
                || !fn.vkGetPhysicalDeviceSurfaceSupportKHR || !fn.vkAcquireNextImageKHR || !fn.vkCreateSwapchainKHR
                || !fn.vkDestroySwapchainKHR || !fn.vkGetSwapchainImagesKHR || !fn.vkQueuePresentKHR)
            {
                throw Exception(
                    "Creating SwapchainVulkan failed: the VK_KHR_surface instance extension and VK_KHR_swapchain "
                    "device extension have to be enabled");
            }

            VkBool32 presentSupported = VK_FALSE;
            fn.vkGetPhysicalDeviceSurfaceSupportKHR(vulkanContext.physicalDevice, vulkanContext.queueFamilyIndex, m_surface, &presentSupported);
            if (!presentSupported)
            {
                throw Exception(
                    "Creating SwapchainVulkan failed: the queue family doesn't support presenting to the window surface");
            }

            // Pick a non-sRGB format, as colors in TGUI aren't converted from sRGB to linear space
            std::uint32_t formatCount = 0;
            fn.vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext.physicalDevice, m_surface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            fn.vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext.physicalDevice, m_surface, &formatCount, formats.data());
            if (formats.empty())
                throw Exception("Creating SwapchainVulkan failed: no surface formats available");

            m_surfaceFormat = formats[0];
            for (const auto& format : formats)
            {
                if (((format.format == VK_FORMAT_B8G8R8A8_UNORM) || (format.format == VK_FORMAT_R8G8B8A8_UNORM))
                    && (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
                {
                    m_surfaceFormat = format;
                    break;
                }
            }

            createVulkanObjects();
        }
        catch (const Exception&)
        {
            destroyVulkanObjects();
            if (fn.vkDestroySurfaceKHR)
                fn.vkDestroySurfaceKHR(vulkanContext.instance, m_surface, nullptr);
            throw;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SwapchainVulkan::~SwapchainVulkan()
    {
        const auto& fn = m_renderer->getFunctions();

        // Wait until the GPU no longer uses the swapchain images and the objects of the frames in flight
        fn.vkQueueWaitIdle(m_renderer->getVulkanContext().queue);

        destroySwapchainImageResources();
        fn.vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        destroyVulkanObjects();
        fn.vkDestroySurfaceKHR(m_renderer->getVulkanContext().instance, m_surface, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkCommandBuffer SwapchainVulkan::beginFrame(Vector2u framebufferSize, Color clearColor)
    {
        const auto& fn = m_renderer->getFunctions();

        if ((framebufferSize.x == 0) || (framebufferSize.y == 0) || m_surfaceLost)
            return VK_NULL_HANDLE;

        if ((m_swapchain == VK_NULL_HANDLE) || m_swapchainOutdated || (framebufferSize != m_framebufferSize))
        {
            if (!recreateSwapchain(framebufferSize))
                return VK_NULL_HANDLE;
        }

        FrameSync& frame = m_frames[m_currentFrame];
        fn.vkWaitForFences(m_device, 1, &frame.inFlightFence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());

        VkResult result = fn.vkAcquireNextImageKHR(m_device,
                                                   m_swapchain,
                                                   std::numeric_limits<std::uint64_t>::max(),
                                                   frame.imageAvailableSemaphore,
                                                   VK_NULL_HANDLE,
                                                   &m_imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            // Try again with a new swapchain
            if (!recreateSwapchain(framebufferSize))
                return VK_NULL_HANDLE;

            result = fn.vkAcquireNextImageKHR(m_device,
                                              m_swapchain,
                                              std::numeric_limits<std::uint64_t>::max(),
                                              frame.imageAvailableSemaphore,
                                              VK_NULL_HANDLE,
                                              &m_imageIndex);
        }

        if ((result != VK_SUCCESS) && (result != VK_SUBOPTIMAL_KHR))
        {
            if (result == VK_ERROR_SURFACE_LOST_KHR)
                m_surfaceLost = true;
            else
                m_swapchainOutdated = true;
            return VK_NULL_HANDLE;
        }

        // Only reset the fence when we are certain that we will submit work that signals it
        fn.vkResetFences(m_device, 1, &frame.inFlightFence);
        fn.vkResetCommandBuffer(frame.cmdBuffer, 0);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        fn.vkBeginCommandBuffer(frame.cmdBuffer, &beginInfo);

        VkClearValue clearValue = {};
        clearValue.color.float32[0] = clearColor.getRed() / 255.f;
        clearValue.color.float32[1] = clearColor.getGreen() / 255.f;
        clearValue.color.float32[2] = clearColor.getBlue() / 255.f;
        clearValue.color.float32[3] = clearColor.getAlpha() / 255.f;

        // The clear value of an sRGB attachment is specified in linear space, while the color in TGUI is an sRGB value
        if (BackendRendererVulkan::isSrgbFormat(m_surfaceFormat.format))
        {
            for (unsigned int i = 0; i < 3; ++i)
            {
                float& value = clearValue.color.float32[i];
                value = (value <= 0.04045f) ? (value / 12.92f) : std::pow((value + 0.055f) / 1.055f, 2.4f);
            }
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffers[m_imageIndex];
        renderPassInfo.renderArea.extent = m_extent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearValue;
        fn.vkCmdBeginRenderPass(frame.cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        return frame.cmdBuffer;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SwapchainVulkan::endFrame()
    {
        const auto& fn = m_renderer->getFunctions();

        FrameSync& frame = m_frames[m_currentFrame];
        fn.vkCmdEndRenderPass(frame.cmdBuffer);
        fn.vkEndCommandBuffer(frame.cmdBuffer);

        const VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &frame.imageAvailableSemaphore;
        submitInfo.pWaitDstStageMask = &waitStage;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &frame.cmdBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_renderFinishedSemaphores[m_imageIndex];
        fn.vkQueueSubmit(m_renderer->getVulkanContext().queue, 1, &submitInfo, frame.inFlightFence);

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_imageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &m_imageIndex;
        // When the compositor rotates the image, Android keeps returning VK_SUBOPTIMAL_KHR, so it is ignored then
        const VkResult result = fn.vkQueuePresentKHR(m_renderer->getVulkanContext().queue, &presentInfo);
        if (result == VK_ERROR_SURFACE_LOST_KHR)
            m_surfaceLost = true;
        else if ((result != VK_SUCCESS) && ((result != VK_SUBOPTIMAL_KHR) || !m_rotatedByCompositor))
            m_swapchainOutdated = true;

        m_currentFrame = (m_currentFrame + 1) % m_frames.size();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkRenderPass SwapchainVulkan::getRenderPass() const
    {
        return m_renderPass;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkFormat SwapchainVulkan::getColorFormat() const
    {
        return m_surfaceFormat.format;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SwapchainVulkan::isSurfaceLost() const
    {
        return m_surfaceLost;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SwapchainVulkan::createVulkanObjects()
    {
        const auto& fn = m_renderer->getFunctions();

        // Create a render pass that clears the swapchain image and leaves it ready for presenting
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_surfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // The layout transition at the start of the render pass has to wait for the image to be acquired
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        if (fn.vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
            throw Exception("Creating SwapchainVulkan failed: failed to create render pass");

        m_renderer->setObjectName(VK_OBJECT_TYPE_RENDER_PASS, m_renderPass, "TGUI swapchain render pass");

        // Create the command buffers and synchronization objects for each frame that can be in flight
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolInfo.queueFamilyIndex = m_renderer->getVulkanContext().queueFamilyIndex;
        if (fn.vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
            throw Exception("Creating SwapchainVulkan failed: failed to create command pool");

        m_renderer->setObjectName(VK_OBJECT_TYPE_COMMAND_POOL, m_commandPool, "TGUI swapchain command pool");

        for (auto& frame : m_frames)
        {
            VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
            cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferAllocInfo.commandPool = m_commandPool;
            cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufferAllocInfo.commandBufferCount = 1;
            if (fn.vkAllocateCommandBuffers(m_device, &cmdBufferAllocInfo, &frame.cmdBuffer) != VK_SUCCESS)
                throw Exception("Creating SwapchainVulkan failed: failed to allocate command buffer");

            // The fence starts signaled because the first wait would otherwise never finish
            VkFenceCreateInfo fenceInfo = {};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            if (fn.vkCreateFence(m_device, &fenceInfo, nullptr, &frame.inFlightFence) != VK_SUCCESS)
                throw Exception("Creating SwapchainVulkan failed: failed to create fence");

            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            if (fn.vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &frame.imageAvailableSemaphore) != VK_SUCCESS)
                throw Exception("Creating SwapchainVulkan failed: failed to create semaphore");

            m_renderer->setObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, frame.cmdBuffer, "TGUI swapchain command buffer");
            m_renderer->setObjectName(VK_OBJECT_TYPE_FENCE, frame.inFlightFence, "TGUI swapchain in-flight fence");
            m_renderer->setObjectName(VK_OBJECT_TYPE_SEMAPHORE, frame.imageAvailableSemaphore, "TGUI image available semaphore");
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SwapchainVulkan::destroyVulkanObjects()
    {
        const auto& fn = m_renderer->getFunctions();

        // All destroy functions accept VK_NULL_HANDLE, so this function can be used when creation failed halfway
        for (const auto& frame : m_frames)
        {
            fn.vkDestroySemaphore(m_device, frame.imageAvailableSemaphore, nullptr);
            fn.vkDestroyFence(m_device, frame.inFlightFence, nullptr);
        }

        fn.vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        fn.vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SwapchainVulkan::recreateSwapchain(Vector2u framebufferSize)
    {
        const auto& fn = m_renderer->getFunctions();

        const VulkanContext& vulkanContext = m_renderer->getVulkanContext();

        VkSurfaceCapabilitiesKHR capabilities;
        const VkResult capabilitiesResult = fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanContext.physicalDevice, m_surface, &capabilities);
        if (capabilitiesResult != VK_SUCCESS)
        {
            m_surfaceLost = (capabilitiesResult == VK_ERROR_SURFACE_LOST_KHR);
            return false;
        }

        // TGUI doesn't rotate its rendering, so when the display is rotated (e.g. on Android) we let the compositor
        // rotate the image. The image then has the size of the window instead of the size of the rotated surface.
        VkSurfaceTransformFlagBitsKHR preTransform = capabilities.currentTransform;
        if ((capabilities.currentTransform != VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
            && (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR))
        {
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }

        // The extent has to match the window size, unless the surface lets us choose the size
        VkExtent2D extent = capabilities.currentExtent;
        if ((extent.width == std::numeric_limits<std::uint32_t>::max()) || (preTransform != capabilities.currentTransform))
        {
            extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, framebufferSize.x));
            extent.height = std::max(capabilities.minImageExtent.height,
                                     std::min(capabilities.maxImageExtent.height, framebufferSize.y));
        }
        if ((extent.width == 0) || (extent.height == 0))
            return false;

        std::uint32_t imageCount = capabilities.minImageCount + 1;
        if ((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount))
            imageCount = capabilities.maxImageCount;

        // Prefer an opaque window, but use any supported mode otherwise
        VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        for (const VkCompositeAlphaFlagBitsKHR flag :
             {VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
              VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
              VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
              VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR})
        {
            if (capabilities.supportedCompositeAlpha & static_cast<VkCompositeAlphaFlagsKHR>(flag))
            {
                compositeAlpha = flag;
                break;
            }
        }

        // Make sure the GPU no longer uses the old swapchain before replacing it.
        // The swapchain remains marked as outdated until all objects have been created successfully.
        fn.vkQueueWaitIdle(vulkanContext.queue);
        destroySwapchainImageResources();
        m_swapchainOutdated = true;

        // FIFO present mode is always supported and enables vsync
        VkSwapchainCreateInfoKHR swapchainInfo = {};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = m_surface;
        swapchainInfo.minImageCount = imageCount;
        swapchainInfo.imageFormat = m_surfaceFormat.format;
        swapchainInfo.imageColorSpace = m_surfaceFormat.colorSpace;
        swapchainInfo.imageExtent = extent;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.preTransform = preTransform;
        swapchainInfo.compositeAlpha = compositeAlpha;
        swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.oldSwapchain = m_swapchain;

        VkSwapchainKHR newSwapchain = VK_NULL_HANDLE;
        const VkResult result = fn.vkCreateSwapchainKHR(m_device, &swapchainInfo, nullptr, &newSwapchain);
        fn.vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_swapchain = newSwapchain;
        if (result != VK_SUCCESS)
        {
            m_surfaceLost = (result == VK_ERROR_SURFACE_LOST_KHR);
            return false;
        }

        m_rotatedByCompositor = (preTransform != capabilities.currentTransform);

        m_renderer->setObjectName(VK_OBJECT_TYPE_SWAPCHAIN_KHR, m_swapchain, "TGUI swapchain");
        m_extent = extent;
        m_framebufferSize = framebufferSize;

        // Create the image views, framebuffers and semaphores for each image in the swapchain
        fn.vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
        std::vector<VkImage> images(imageCount);
        fn.vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, images.data());

        for (const VkImage image : images)
        {
            VkImageViewCreateInfo viewInfo = {};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = image;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = m_surfaceFormat.format;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.layerCount = 1;

            VkImageView imageView = VK_NULL_HANDLE;
            if (fn.vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
                return false;
            m_imageViews.push_back(imageView);
            m_renderer->setObjectName(VK_OBJECT_TYPE_IMAGE, image, "TGUI swapchain image");
            m_renderer->setObjectName(VK_OBJECT_TYPE_IMAGE_VIEW, imageView, "TGUI swapchain image view");

            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = &imageView;
            framebufferInfo.width = extent.width;
            framebufferInfo.height = extent.height;
            framebufferInfo.layers = 1;

            VkFramebuffer framebuffer = VK_NULL_HANDLE;
            if (fn.vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
                return false;
            m_framebuffers.push_back(framebuffer);
            m_renderer->setObjectName(VK_OBJECT_TYPE_FRAMEBUFFER, framebuffer, "TGUI swapchain framebuffer");

            // The semaphore that is waited on by the presentation engine can only be reused when the image is acquired again
            VkSemaphoreCreateInfo semaphoreInfo = {};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkSemaphore semaphore = VK_NULL_HANDLE;
            if (fn.vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS)
                return false;
            m_renderFinishedSemaphores.push_back(semaphore);
            m_renderer->setObjectName(VK_OBJECT_TYPE_SEMAPHORE, semaphore, "TGUI swapchain render finished semaphore");
        }

        m_swapchainOutdated = false;
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SwapchainVulkan::destroySwapchainImageResources()
    {
        const auto& fn = m_renderer->getFunctions();

        for (const VkSemaphore semaphore : m_renderFinishedSemaphores)
            fn.vkDestroySemaphore(m_device, semaphore, nullptr);
        for (const VkFramebuffer framebuffer : m_framebuffers)
            fn.vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        for (const VkImageView imageView : m_imageViews)
            fn.vkDestroyImageView(m_device, imageView, nullptr);

        m_renderFinishedSemaphores.clear();
        m_framebuffers.clear();
        m_imageViews.clear();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
