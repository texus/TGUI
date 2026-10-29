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

#include <TGUI/Backend/Renderer/BackendText.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendRenderTargetVulkan.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendRendererVulkan.hpp>

#include <TGUI/Container.hpp>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <numeric>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static bool compareClipRects(const VkRect2D& rect1, const VkRect2D& rect2)
    {
        return (rect1.offset.x == rect2.offset.x) && (rect1.offset.y == rect2.offset.y)
               && (rect1.extent.width == rect2.extent.width) && (rect1.extent.height == rect2.extent.height);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Converts a rectangle in pixels to a scissor rectangle, which isn't allowed to have negative coordinates
    static VkRect2D createClipRect(FloatRect rect, Vector2f targetSize)
    {
        const long targetWidth = std::lround(targetSize.x);
        const long targetHeight = std::lround(targetSize.y);
        const long left = std::max(0L, std::min(targetWidth, std::lround(rect.left)));
        const long top = std::max(0L, std::min(targetHeight, std::lround(rect.top)));
        const long right = std::max(left, std::min(targetWidth, std::lround(rect.left) + std::lround(rect.width)));
        const long bottom = std::max(top, std::min(targetHeight, std::lround(rect.top) + std::lround(rect.height)));

        VkRect2D clipRect;
        clipRect.offset = {static_cast<std::int32_t>(left), static_cast<std::int32_t>(top)};
        clipRect.extent = {static_cast<std::uint32_t>(right - left), static_cast<std::uint32_t>(bottom - top)};
        return clipRect;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetVulkan::BackendRenderTargetVulkan(std::shared_ptr<BackendRendererVulkan> renderer) :
        m_renderer{std::move(renderer)}
    {
        TGUI_ASSERT(m_renderer, "renderer passed to BackendRenderTargetVulkan can't be a nullptr");

        // Create a solid white 1x1 texture to pass to the shader when we aren't drawing a texture
        m_emptyTexture = std::make_unique<BackendTextureVulkan>(m_renderer);
        auto pixels = MakeUniqueForOverwrite<std::uint8_t[]>(4); // 4 bytes to store RGBA values
        for (unsigned int i = 0; i < 4; ++i)
            pixels[i] = 255;
        if (!m_emptyTexture->load({1, 1}, std::move(pixels), false))
            throw Exception("Creating BackendRenderTargetVulkan failed: failed to create texture");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetVulkan::~BackendRenderTargetVulkan()
    {
        const auto& fn = m_renderer->getFunctions();

        // We don't know which frames are still being executed, so wait until the GPU is no longer using our resources.
        // The command buffers in which the gui was drawn are submitted to our queue, so we don't need to wait for other queues.
        const VkDevice device = m_renderer->getVulkanContext().device;
        fn.vkQueueWaitIdle(m_renderer->getVulkanContext().queue);

        destroyFrameResources();
        fn.vkDestroyPipeline(device, m_pipeline, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::setClearColor(const Color&)
    {
        // This function isn't used by this backend
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::clearScreen()
    {
        // This function isn't used by this backend
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::setView(FloatRect view, FloatRect viewport, Vector2f targetSize)
    {
        BackendRenderTarget::setView(view, viewport, targetSize);

        // The y-axis in Vulkan points downwards, so unlike in other backends we don't have to flip it
        m_projectionTransform = Transform();
        m_projectionTransform.translate({-1 - (2.f * (view.left / view.width)), -1 - (2.f * (view.top / view.height))});
        m_projectionTransform.scale({2.f / view.width, 2.f / view.height});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::setRenderPass(
        VkRenderPass renderPass,
        VkFormat colorFormat,
        std::uint32_t maxFramesInFlight,
        std::uint32_t subpass,
        VkSampleCountFlagBits samples)
    {
        setMaxFramesInFlight(maxFramesInFlight);
        if ((renderPass == m_renderPass) && (colorFormat == m_colorFormat) && (subpass == m_subpass) && (samples == m_samples))
            return;

        retirePipeline();
        m_renderPass = renderPass;
        m_subpass = subpass;
        m_colorFormat = colorFormat;
        m_depthFormat = VK_FORMAT_UNDEFINED;
        m_stencilFormat = VK_FORMAT_UNDEFINED;
        m_samples = samples;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::setDynamicRenderingFormats(
        VkFormat colorFormat,
        std::uint32_t maxFramesInFlight,
        VkFormat depthFormat,
        VkFormat stencilFormat,
        VkSampleCountFlagBits samples)
    {
        setMaxFramesInFlight(maxFramesInFlight);
        if ((m_renderPass == VK_NULL_HANDLE) && (colorFormat == m_colorFormat) && (depthFormat == m_depthFormat)
            && (stencilFormat == m_stencilFormat) && (samples == m_samples))
            return;

        retirePipeline();
        m_renderPass = VK_NULL_HANDLE;
        m_subpass = 0;
        m_colorFormat = colorFormat;
        m_depthFormat = depthFormat;
        m_stencilFormat = stencilFormat;
        m_samples = samples;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendRenderTargetVulkan::drawGui(const std::shared_ptr<RootContainer>& root, VkCommandBuffer cmdBuffer)
    {
        const auto& fn = m_renderer->getFunctions();

        if ((cmdBuffer == VK_NULL_HANDLE) || (m_targetSize.x <= 0) || (m_targetSize.y <= 0) || (m_viewRect.width <= 0)
            || (m_viewRect.height <= 0) || (m_viewport.width <= 0) || (m_viewport.height <= 0))
            return false;

        TGUI_ASSERT((m_renderPass != VK_NULL_HANDLE) || (m_colorFormat != VK_FORMAT_UNDEFINED),
                    "setRenderPass or setDynamicRenderingFormats has to be called before drawing with the Vulkan backend");

        if (m_pipeline == VK_NULL_HANDLE)
        {
            if ((m_renderPass == VK_NULL_HANDLE) && (m_colorFormat == VK_FORMAT_UNDEFINED))
                return false;

            m_pipeline = m_renderer->createPipeline(m_renderPass, m_subpass, m_colorFormat, m_depthFormat, m_stencilFormat, m_samples);
            if (m_pipeline == VK_NULL_HANDLE)
                return false;
        }

        // The frame that previously used these resources was drawn maxFramesInFlight frames ago,
        // so the GPU is no longer using them and they can be reused.
        m_currentFrame = (m_currentFrame + 1) % m_frames.size();
        FrameResources& frame = m_frames[m_currentFrame];
        frame.drawCommands.clear();
        for (VkPipeline pipeline : frame.retiredPipelines)
            fn.vkDestroyPipeline(m_renderer->getVulkanContext().device, pipeline, nullptr);
        frame.retiredPipelines.clear();

        // Free the staging buffers of textures that finished uploading
        m_renderer->releaseFinishedUploads();

        // Let all widgets add their vertices and draw commands
        m_pixelsPerPoint = {m_viewport.width / m_viewRect.width, m_viewport.height / m_viewRect.height};
        m_clipRect = createClipRect({{0, 0}, m_targetSize}, m_targetSize);
        m_vertices.clear();
        m_indices.clear();

        root->draw(*this, {});

        if (frame.drawCommands.empty() || m_vertices.empty() || m_indices.empty())
            return true; // Nothing to do

        // Copy the vertices and indices to a buffer that is accessible by both the CPU and the GPU.
        // The buffer is only recreated when it is too small, in which case we allocate more memory than needed.
        const VkDevice device = m_renderer->getVulkanContext().device;
        const VkDeviceSize verticesSize = m_vertices.size() * sizeof(Vertex);
        const VkDeviceSize indicesSize = m_indices.size() * sizeof(unsigned int);
        if (frame.bufferSize < verticesSize + indicesSize)
        {
            fn.vkDestroyBuffer(device, frame.buffer, nullptr);
            fn.vkFreeMemory(device, frame.memory, nullptr);
            frame.buffer = VK_NULL_HANDLE;
            frame.memory = VK_NULL_HANDLE;
            frame.bufferSize = 0;
            frame.mappedMemory = nullptr;

            const VkDeviceSize newBufferSize = (verticesSize + indicesSize) * 3 / 2;
            if (!m_renderer->createBuffer(newBufferSize,
                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                          frame.buffer,
                                          frame.memory))
                return false;

            m_renderer->setObjectName(VK_OBJECT_TYPE_BUFFER, frame.buffer, "TGUI vertex and index buffer");
            m_renderer->setObjectName(VK_OBJECT_TYPE_DEVICE_MEMORY, frame.memory, "TGUI vertex and index buffer memory");

            if (fn.vkMapMemory(device, frame.memory, 0, VK_WHOLE_SIZE, 0, &frame.mappedMemory) != VK_SUCCESS)
            {
                frame.mappedMemory = nullptr;
                return false;
            }

            frame.bufferSize = newBufferSize;
        }

        static_assert(sizeof(unsigned int) == 4, "Indices are stored as 32bit values");
        static_assert(sizeof(Vertex) % 4 == 0, "Index buffer offset must be a multiple of the index size");
        std::memcpy(frame.mappedMemory, m_vertices.data(), static_cast<std::size_t>(verticesSize));
        std::memcpy(static_cast<std::uint8_t*>(frame.mappedMemory) + verticesSize,
                    m_indices.data(),
                    static_cast<std::size_t>(indicesSize));
        m_vertices.clear();
        m_indices.clear();

        // Record the commands to draw everything
        const VkRect2D defaultClipRect = createClipRect({{0, 0}, m_targetSize}, m_targetSize);
        const VkDeviceSize vertexBufferOffset = 0;
        fn.vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        fn.vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &frame.buffer, &vertexBufferOffset);
        fn.vkCmdBindIndexBuffer(cmdBuffer, frame.buffer, verticesSize, VK_INDEX_TYPE_UINT32);
        fn.vkCmdSetScissor(cmdBuffer, 0, 1, &defaultClipRect);

        const VkViewport viewport = {m_viewport.left, m_viewport.top, m_viewport.width, m_viewport.height, 0, 1};
        fn.vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

        const VkPipelineLayout pipelineLayout = m_renderer->getPipelineLayout();
        std::uint32_t indexOffset = 0;
        VkDescriptorSet lastTextureDescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSet lastSamplerDescriptorSet = VK_NULL_HANDLE;
        VkRect2D lastClipRect = defaultClipRect;
        for (const DrawCommand& drawCommand : frame.drawCommands)
        {
            if (drawCommand.imageResources->descriptorSet != lastTextureDescriptorSet)
            {
                lastTextureDescriptorSet = drawCommand.imageResources->descriptorSet;
                fn.vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &lastTextureDescriptorSet, 0, nullptr);
            }

            const VkDescriptorSet samplerDescriptorSet = m_renderer->getSamplerDescriptorSet(drawCommand.smooth);
            if (samplerDescriptorSet != lastSamplerDescriptorSet)
            {
                lastSamplerDescriptorSet = samplerDescriptorSet;
                fn.vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &lastSamplerDescriptorSet, 0, nullptr);
            }

            if (!compareClipRects(lastClipRect, drawCommand.clipRect))
            {
                fn.vkCmdSetScissor(cmdBuffer, 0, 1, &drawCommand.clipRect);
                lastClipRect = drawCommand.clipRect;
            }

            fn.vkCmdDrawIndexed(cmdBuffer, static_cast<std::uint32_t>(drawCommand.indexCount), 1, indexOffset, 0, 0);
            indexOffset += static_cast<std::uint32_t>(drawCommand.indexCount);
        }

        if (!compareClipRects(lastClipRect, defaultClipRect))
            fn.vkCmdSetScissor(cmdBuffer, 0, 1, &defaultClipRect);

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::drawVertexArray(
        const RenderStates& states,
        const Vertex* vertices,
        std::size_t vertexCount,
        const unsigned int* indices,
        std::size_t indexCount,
        const std::shared_ptr<BackendTexture>& texture)
    {
        if ((vertexCount == 0) || (m_clipRect.extent.width == 0) || (m_clipRect.extent.height == 0))
            return;

        std::shared_ptr<BackendTextureVulkan::ImageResources> imageResources;
        bool smooth = false;
        if (texture)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureVulkan>(texture),
                        "BackendRenderTargetVulkan requires textures of type BackendTextureVulkan");
            imageResources = std::static_pointer_cast<BackendTextureVulkan>(texture)->getImageResources();
            if (!imageResources)
                return; // The texture failed to load

            smooth = texture->isSmooth();
        }
        else
            imageResources = m_emptyTexture->getImageResources();

        const std::size_t exisingVerticesCount = m_vertices.size();
        const std::size_t exisingIndicesCount = m_indices.size();
        const Transform finalTransform = m_projectionTransform * states.transform;

        m_vertices.insert(m_vertices.end(), vertices, vertices + vertexCount);
        std::for_each(m_vertices.begin() + static_cast<std::ptrdiff_t>(exisingVerticesCount),
                      m_vertices.end(),
                      [&finalTransform](Vertex& vertex) { vertex.position = finalTransform.transformPoint(vertex.position); });

        if (indices)
        {
            m_indices.insert(m_indices.end(), indices, indices + indexCount);
            std::for_each(m_indices.begin() + static_cast<std::ptrdiff_t>(exisingIndicesCount),
                          m_indices.end(),
                          [exisingVerticesCount](unsigned int& index) { index += static_cast<unsigned int>(exisingVerticesCount); });
        }
        else
        {
            m_indices.resize(m_indices.size() + vertexCount);
            std::iota(m_indices.begin() + static_cast<std::ptrdiff_t>(exisingIndicesCount),
                      m_indices.end(),
                      static_cast<unsigned int>(exisingVerticesCount));
            indexCount = vertexCount;
        }

        // Combine draw commands when the texture and clipping didn't change.
        // The draw commands keep the image alive until the GPU is no longer using it.
        auto& drawCommands = m_frames[m_currentFrame].drawCommands;
        if (!drawCommands.empty() && (drawCommands.back().imageResources == imageResources)
            && (drawCommands.back().smooth == smooth) && compareClipRects(drawCommands.back().clipRect, m_clipRect))
            drawCommands.back().indexCount += indexCount;
        else
            drawCommands.push_back({indexCount, std::move(imageResources), smooth, m_clipRect});
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::updateClipping(FloatRect clipRect, FloatRect clipViewport)
    {
        if ((clipViewport.width > 0) && (clipViewport.height > 0) && (clipRect.width > 0) && (clipRect.height > 0))
        {
            m_pixelsPerPoint = {clipViewport.width / clipRect.width, clipViewport.height / clipRect.height};
            m_clipRect = createClipRect(clipViewport, m_targetSize);
        }
        else // Clip the entire window
        {
            m_pixelsPerPoint = {1, 1};
            m_clipRect = {};
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::retirePipeline()
    {
        if (m_pipeline == VK_NULL_HANDLE)
            return;

        // The last drawn frames might still be using the pipeline. It will be destroyed when the resources of the
        // most recent frame are reused, at which point the GPU is guaranteed to have finished all those frames.
        m_frames[m_currentFrame].retiredPipelines.push_back(m_pipeline);
        m_pipeline = VK_NULL_HANDLE;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::setMaxFramesInFlight(std::uint32_t maxFramesInFlight)
    {
        TGUI_ASSERT(maxFramesInFlight > 0, "maxFramesInFlight passed to BackendRenderTargetVulkan must be at least 1");
        maxFramesInFlight = std::max(1u, maxFramesInFlight);
        if (maxFramesInFlight == m_frames.size())
            return;

        // We don't know which frames are still being executed, so wait until the GPU is no longer using their resources
        if (!m_frames.empty())
        {
            m_renderer->getFunctions().vkQueueWaitIdle(m_renderer->getVulkanContext().queue);
            destroyFrameResources();
        }

        m_frames.clear();
        m_frames.resize(maxFramesInFlight);
        m_currentFrame = 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetVulkan::destroyFrameResources()
    {
        const auto& fn = m_renderer->getFunctions();
        const VkDevice device = m_renderer->getVulkanContext().device;
        for (const auto& frame : m_frames)
        {
            for (VkPipeline pipeline : frame.retiredPipelines)
                fn.vkDestroyPipeline(device, pipeline, nullptr);

            fn.vkDestroyBuffer(device, frame.buffer, nullptr);
            fn.vkFreeMemory(device, frame.memory, nullptr);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
