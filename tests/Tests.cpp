/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tests.hpp"

#include <TGUI/Config.hpp>

#include <cmath>
#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/extlibs/IncludeWindows.hpp>
#endif

#if TGUI_HAS_BACKEND_SDL_GPU || TGUI_HAS_RENDERER_BACKEND_VULKAN
    #include <TGUI/extlibs/IncludeStbImageWrite.hpp>
#endif

#if TGUI_HAS_BACKEND_SDL_GPU
    #include <TGUI/Backend/SDL-GPU.hpp>
#endif

#if TGUI_HAS_RENDERER_BACKEND_VULKAN
    #include <TGUI/Backend/Renderer/Vulkan/BackendRenderTargetVulkan.hpp>
    #include <TGUI/Backend/Renderer/Vulkan/BackendRendererVulkan.hpp>
#endif

namespace
{
    template <typename T>
    void testClickableWidgetSignalsImpl(const T& widget)
    {
        testWidgetSignals(widget);

        unsigned int mousePressedCount = 0;
        unsigned int mouseReleasedCount = 0;
        unsigned int clickedCount = 0;
        unsigned int rightMousePressedCount = 0;
        unsigned int rightMouseReleasedCount = 0;
        unsigned int rightClickedCount = 0;

        widget->setPosition({40, 30});
        widget->setSize({150, 100});

        widget->onMousePress([&](tgui::Vector2f pos) { mouseCallback(mousePressedCount, pos); });
        widget->onMouseRelease([&](tgui::Vector2f pos) { mouseCallback(mouseReleasedCount, pos); });
        widget->onClick([&](tgui::Vector2f pos) { mouseCallback(clickedCount, pos); });
        widget->onRightMousePress([&](tgui::Vector2f pos) { mouseCallback(rightMousePressedCount, pos); });
        widget->onRightMouseRelease([&](tgui::Vector2f pos) { mouseCallback(rightMouseReleasedCount, pos); });
        widget->onRightClick([&](tgui::Vector2f pos) { mouseCallback(rightClickedCount, pos); });

        SECTION("isMouseOnWidget")
        {
            REQUIRE(!widget->isMouseOnWidget({39, 29}));
            REQUIRE(widget->isMouseOnWidget({40, 30}));
            REQUIRE(widget->isMouseOnWidget({115, 80}));
            REQUIRE(widget->isMouseOnWidget({189, 129}));
            REQUIRE(!widget->isMouseOnWidget({190, 130}));

            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 0);
            REQUIRE(clickedCount == 0);
            REQUIRE(rightMousePressedCount == 0);
            REQUIRE(rightMouseReleasedCount == 0);
            REQUIRE(rightClickedCount == 0);
        }

        auto parent = tgui::Panel::create({300, 200});
        parent->setPosition({60, 55});
        parent->add(widget);

        SECTION("left mouse click")
        {
            parent->leftMouseReleased({175, 135});

            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);

            SECTION("mouse press")
            {
                parent->leftMousePressed({175, 135});

                REQUIRE(mousePressedCount == 1);
                REQUIRE(mouseReleasedCount == 1);
                REQUIRE(clickedCount == 0);
            }

            parent->leftMouseReleased({175, 135});
            parent->leftMouseButtonNoLongerDown();

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 1);
            REQUIRE(rightMousePressedCount == 0);
            REQUIRE(rightMouseReleasedCount == 0);
            REQUIRE(rightClickedCount == 0);
        }

        SECTION("right mouse click")
        {
            parent->rightMouseReleased({175, 135});

            REQUIRE(rightMousePressedCount == 0);
            REQUIRE(rightMouseReleasedCount == 1);
            REQUIRE(rightClickedCount == 0);

            SECTION("mouse press")
            {
                parent->rightMousePressed({175, 135});

                REQUIRE(rightMousePressedCount == 1);
                REQUIRE(rightMouseReleasedCount == 1);
                REQUIRE(rightClickedCount == 0);
            }

            parent->rightMouseReleased({175, 135});
            parent->rightMouseButtonNoLongerDown();

            REQUIRE(rightMousePressedCount == 1);
            REQUIRE(rightMouseReleasedCount == 2);
            REQUIRE(rightClickedCount == 1);
            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 0);
            REQUIRE(clickedCount == 0);
        }
    }
} // anonymous namespace

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool compareVector2f(tgui::Vector2f left, tgui::Vector2f right)
{
    const float epsilonX = std::max(0.000001f, std::max(std::abs(left.x), std::abs(right.x)) / 1000000.0f);
    const float epsilonY = std::max(0.000001f, std::max(std::abs(left.y), std::abs(right.y)) / 1000000.0f);
    return (std::fabs(left.x - right.x) < epsilonX) && (std::fabs(left.y - right.y) < epsilonY);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

tgui::String getClipboardContents()
{
#ifdef TGUI_SYSTEM_WINDOWS
    // Wait a moment before accessing the clipboard.
    // The data wasn't always there yet when requesting it immediately after changing it when using the SDL backend on Windows.
    // The problem might be with setting instead of getting though: setting two times quickly resulted in access denied error.
    // There were still occational failures when only waiting 1ms.
    Sleep(5);
#endif

    return tgui::getBackend()->getClipboard();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mouseCallback(unsigned int& count, tgui::Vector2f pos)
{
    count++;
    REQUIRE(pos == tgui::Vector2f(75, 50));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void genericCallback(unsigned int& count)
{
    count++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testWidgetSignals(const tgui::Widget::Ptr& widget)
{
    // TODO: Test other signals than MouseEntered and MouseLeft

    SECTION("mouse move")
    {
        unsigned int mouseEnteredCount = 0;
        unsigned int mouseLeftCount = 0;

        widget->onMouseEnter([&] { genericCallback(mouseEnteredCount); });
        widget->onMouseLeave([&] { genericCallback(mouseLeftCount); });

        auto parent = tgui::Panel::create({300, 200});
        parent->setPosition({30, 25});
        parent->add(widget);

        widget->setPosition({40, 30});
        widget->setSize({150, 100});

        parent->mouseMoved({40, 40});
        REQUIRE(mouseEnteredCount == 0);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved({70, 55});
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved({219, 154});
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved({220, 155});
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 1);

        parent->remove(widget);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testClickableWidgetSignals(const tgui::ClickableWidget::Ptr& widget)
{
    testClickableWidgetSignalsImpl(widget);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testClickableWidgetSignals(const tgui::Panel::Ptr& widget)
{
    testClickableWidgetSignalsImpl(widget);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testScrollbarAccess(tgui::ScrollbarAccessor* scrollbar)
{
    scrollbar->setValue(10);
    REQUIRE(scrollbar->getValue() == 10);

    scrollbar->setScrollAmount(25);
    REQUIRE(scrollbar->getScrollAmount() == 25);

    scrollbar->setPolicy(tgui::Scrollbar::Policy::Always);
    REQUIRE(scrollbar->getPolicy() == tgui::Scrollbar::Policy::Always);

    REQUIRE(scrollbar->getMaximum() > scrollbar->getViewportSize());
    REQUIRE(scrollbar->getMaxValue() == scrollbar->getMaximum() - scrollbar->getViewportSize());

    REQUIRE(scrollbar->isShown());
    REQUIRE(scrollbar->getWidth() > 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testWidgetRenderer(tgui::WidgetRenderer* renderer)
{
    SECTION("WidgetRenderer")
    {
        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
            REQUIRE_NOTHROW(renderer->setProperty("Font", "resources/DejaVuSans.ttf"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
            REQUIRE_NOTHROW(renderer->setProperty("Font", tgui::Font{"resources/DejaVuSans.ttf"}));
        }

        SECTION("functions")
        {
            renderer->setOpacity(0.8f);
            renderer->setFont({"resources/DejaVuSans.ttf"});
        }

        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
        REQUIRE(renderer->getProperty("Font").getFont() != nullptr);

        REQUIRE(renderer->getOpacity() == 0.8f);
        REQUIRE(renderer->getFont().getId() == "resources/DejaVuSans.ttf");

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void testDraw(tgui::BackendGui& gui, const char* filename, tgui::Vector2u imageSize, void* targetPtr)
{
    (void)filename;
    (void)imageSize;
    (void)targetPtr;

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer()) && targetPtr)
    {
        auto* target = reinterpret_cast<sf::RenderTexture*>(targetPtr);
        target->clear({25, 130, 10});
        gui.draw();
        target->display();
        (void)target->getTexture().copyToImage().saveToFile(filename);
    #ifdef TGUI_ENABLE_DRAW_TESTS
        compareImageFiles(filename, "expected/" + tgui::String(filename));
    #endif
        return;
    }
#endif // TGUI_HAS_BACKEND_SFML_GRAPHICS

#if TGUI_HAS_BACKEND_SDL_GPU
    if (std::dynamic_pointer_cast<tgui::BackendRendererSDLGPU>(tgui::getBackend()->getRenderer()))
    {
        if (dynamic_cast<tgui::SDL_GPU::Gui*>(&gui) == nullptr)
        {
            assert(false);
            return;
        }

        tgui::RelFloatRect viewport = gui.getViewport();
        tgui::RelFloatRect view = gui.getView();
        viewport.updateParentSize({static_cast<float>(imageSize.x), static_cast<float>(imageSize.y)});
        view.updateParentSize({viewport.getWidth(), viewport.getHeight()});
        gui.getBackendRenderTarget()->setView(view.getRect(),
                                              viewport.getRect(),
                                              {static_cast<float>(imageSize.x), static_cast<float>(imageSize.y)});
        gui.getContainer()->setSize(tgui::Vector2f{view.getWidth(), view.getHeight()});

        const auto backendRenderer = std::static_pointer_cast<tgui::BackendRendererSDLGPU>(tgui::getBackend()->getRenderer());
        SDL_GPUDevice* device = backendRenderer->getInternalDevice();

        // The texture needs to be rendered with the same format as the gui's pipeline. We expect either BGRA or RGBA.
        const SDL_GPUTextureFormat textureFormat = backendRenderer->getSwapchainTextureFormat();
        if ((textureFormat != SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM) && (textureFormat != SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM))
        {
            assert(false);
            return;
        }
        SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
        if (!cmdBuffer)
        {
            assert(false);
            return;
        }

        SDL_GPUTextureCreateInfo textureCreateInfo = {};
        textureCreateInfo.type = SDL_GPU_TEXTURETYPE_2D;
        textureCreateInfo.format = textureFormat;
        textureCreateInfo.width = imageSize.x;
        textureCreateInfo.height = imageSize.y;
        textureCreateInfo.layer_count_or_depth = 1;
        textureCreateInfo.num_levels = 1;
        textureCreateInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
        SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &textureCreateInfo);
        if (!texture)
        {
            assert(false);
            return;
        }

        dynamic_cast<tgui::SDL_GPU::Gui&>(gui).prepareDraw(cmdBuffer);

        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = texture;
        colorTargetInfo.clear_color = {25.f / 255.f, 130.f / 255.f, 10.f / 255.f, 1.f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, nullptr);
        dynamic_cast<tgui::SDL_GPU::Gui&>(gui).draw(renderPass);
        SDL_EndGPURenderPass(renderPass);

        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {};
        transferBufferCreateInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
        transferBufferCreateInfo.size = textureCreateInfo.width * textureCreateInfo.height * 4;
        SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferBufferCreateInfo);

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmdBuffer);

        SDL_GPUTextureRegion textureRegion = {};
        textureRegion.texture = texture;
        textureRegion.w = textureCreateInfo.width;
        textureRegion.h = textureCreateInfo.height;
        textureRegion.d = 1;

        SDL_GPUTextureTransferInfo textureTransferInfo = {};
        textureTransferInfo.transfer_buffer = transferBuffer;
        textureTransferInfo.offset = 0;
        SDL_DownloadFromGPUTexture(copyPass, &textureRegion, &textureTransferInfo);
        SDL_EndGPUCopyPass(copyPass);

        SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmdBuffer);
        SDL_WaitForGPUFences(device, true, &fence, 1);
        SDL_ReleaseGPUFence(device, fence);

        const auto* mappedPixels = static_cast<const std::uint8_t*>(SDL_MapGPUTransferBuffer(device, transferBuffer, false));
        std::vector<std::uint8_t> pixelData(mappedPixels, mappedPixels + transferBufferCreateInfo.size);
        SDL_UnmapGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
        SDL_ReleaseGPUTexture(device, texture);

        // Swap red and blue pixels when rendering to a BGRA texture (pixel data is RGBA)
        if (textureFormat == SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM)
        {
            for (std::size_t i = 0; i < pixelData.size(); i += 4)
                std::swap(pixelData[i], pixelData[i + 2]);
        }

        const auto writeFunc = [](void* filenamePtr, void* pngData, int dataLength)
        {
            tgui::writeFile(*static_cast<decltype(filename)*>(filenamePtr),
                            tgui::CharStringView(static_cast<const char*>(pngData), static_cast<std::size_t>(dataLength)));
        };
        if (!stbi_write_png_to_func(writeFunc,
                                    &filename,
                                    static_cast<int>(textureCreateInfo.width),
                                    static_cast<int>(textureCreateInfo.height),
                                    4,
                                    pixelData.data(),
                                    static_cast<int>(textureCreateInfo.width * 4)))
        {
            assert(false);
            return;
        }

    #ifdef TGUI_ENABLE_DRAW_TESTS
            // Comparing the output files is not possible because the expected images were rendered with SFML (and thus might differ slightly).
            // However this line can be uncommented if the files in the expected folder were replaced with ones generated with the SDL_GPU backend.
            //compareImageFiles(filename, "expected/" + tgui::String(filename));
    #endif
        return;
    }
#endif // TGUI_HAS_BACKEND_SDL_GPU

#if TGUI_HAS_RENDERER_BACKEND_VULKAN
    if (std::dynamic_pointer_cast<tgui::BackendRendererVulkan>(tgui::getBackend()->getRenderer()))
    {
        const auto renderTarget = std::dynamic_pointer_cast<tgui::BackendRenderTargetVulkan>(gui.getBackendRenderTarget());
        if (renderTarget == nullptr)
        {
            assert(false);
            return;
        }

        tgui::RelFloatRect viewport = gui.getViewport();
        tgui::RelFloatRect view = gui.getView();
        viewport.updateParentSize({static_cast<float>(imageSize.x), static_cast<float>(imageSize.y)});
        view.updateParentSize({viewport.getWidth(), viewport.getHeight()});
        renderTarget->setView(view.getRect(), viewport.getRect(), {static_cast<float>(imageSize.x), static_cast<float>(imageSize.y)});
        gui.getContainer()->setSize(tgui::Vector2f{view.getWidth(), view.getHeight()});

        const auto renderer = std::static_pointer_cast<tgui::BackendRendererVulkan>(tgui::getBackend()->getRenderer());
        const tgui::VulkanContext& vulkanContext = renderer->getVulkanContext();
        const VkDevice device = vulkanContext.device;

        // Create the image to render to
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent = {imageSize.x, imageSize.y, 1};
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImage image = VK_NULL_HANDLE;
        REQUIRE(vkCreateImage(device, &imageInfo, nullptr, &image) == VK_SUCCESS);

        VkMemoryRequirements memoryRequirements;
        vkGetImageMemoryRequirements(device, image, &memoryRequirements);
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = renderer->findMemoryType(memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (allocInfo.memoryTypeIndex >= VK_MAX_MEMORY_TYPES)
            allocInfo.memoryTypeIndex = renderer->findMemoryType(memoryRequirements.memoryTypeBits, 0);
        REQUIRE(allocInfo.memoryTypeIndex < VK_MAX_MEMORY_TYPES);
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        REQUIRE(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) == VK_SUCCESS);
        REQUIRE(vkBindImageMemory(device, image, imageMemory, 0) == VK_SUCCESS);

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
        VkImageView imageView = VK_NULL_HANDLE;
        REQUIRE(vkCreateImageView(device, &viewInfo, nullptr, &imageView) == VK_SUCCESS);

        // Create a render pass that leaves the image ready to be copied to a buffer
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = imageInfo.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        const VkAttachmentReference colorAttachmentRef = {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = 0;
        dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dependency.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        VkRenderPass renderPass = VK_NULL_HANDLE;
        REQUIRE(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) == VK_SUCCESS);

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = &imageView;
        framebufferInfo.width = imageSize.x;
        framebufferInfo.height = imageSize.y;
        framebufferInfo.layers = 1;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        REQUIRE(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) == VK_SUCCESS);

        // Cached memory is preferred, because reading from uncached memory on the CPU is very slow.
        // Not every device has host-visible memory that is cached, so coherent memory is used as fallback.
        const VkDeviceSize readbackSize = static_cast<VkDeviceSize>(imageSize.x) * imageSize.y * 4;
        VkBuffer readbackBuffer = VK_NULL_HANDLE;
        VkDeviceMemory readbackMemory = VK_NULL_HANDLE;
        if (!renderer->createBuffer(readbackSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                                    readbackBuffer,
                                    readbackMemory))
        {
            REQUIRE(renderer->createBuffer(readbackSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           readbackBuffer,
                                           readbackMemory));
        }

        // Draw the gui and copy the result to the buffer
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        commandPoolInfo.queueFamilyIndex = vulkanContext.queueFamilyIndex;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        REQUIRE(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) == VK_SUCCESS);

        VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
        cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdBufferAllocInfo.commandPool = commandPool;
        cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cmdBufferAllocInfo.commandBufferCount = 1;
        VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
        REQUIRE(vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, &cmdBuffer) == VK_SUCCESS);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        REQUIRE(vkBeginCommandBuffer(cmdBuffer, &beginInfo) == VK_SUCCESS);

        VkClearValue clearValue = {};
        clearValue.color = {{25.f / 255.f, 130.f / 255.f, 10.f / 255.f, 1.f}};
        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = renderPass;
        renderPassBeginInfo.framebuffer = framebuffer;
        renderPassBeginInfo.renderArea.extent = {imageSize.x, imageSize.y};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        const std::uint32_t maxFramesInFlight = 1; // We wait for the queue to be idle after each draw
        renderTarget->setRenderPass(renderPass, imageInfo.format, maxFramesInFlight);
        renderTarget->drawGui(gui.getContainer(), cmdBuffer);

        vkCmdEndRenderPass(cmdBuffer);

        VkBufferImageCopy region = {};
        region.imageSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
        region.imageExtent = {imageSize.x, imageSize.y, 1};
        vkCmdCopyImageToBuffer(cmdBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, readbackBuffer, 1, &region);

        REQUIRE(vkEndCommandBuffer(cmdBuffer) == VK_SUCCESS);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        REQUIRE(vkQueueSubmit(vulkanContext.queue, 1, &submitInfo, VK_NULL_HANDLE) == VK_SUCCESS);
        REQUIRE(vkQueueWaitIdle(vulkanContext.queue) == VK_SUCCESS);

        // Save the pixels to a file. The pixels are copied first, so that the image encoder doesn't read from the mapped
        // memory, which would be very slow if the fallback to uncached memory was used.
        // The cached memory isn't necessarily coherent, so the mapped range is invalidated before reading from it.
        void* mappedMemory = nullptr;
        REQUIRE(vkMapMemory(device, readbackMemory, 0, VK_WHOLE_SIZE, 0, &mappedMemory) == VK_SUCCESS);
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = readbackMemory;
        mappedRange.offset = 0;
        mappedRange.size = VK_WHOLE_SIZE;
        REQUIRE(vkInvalidateMappedMemoryRanges(device, 1, &mappedRange) == VK_SUCCESS);
        std::vector<std::uint8_t> pixelData(static_cast<std::size_t>(readbackSize));
        std::memcpy(pixelData.data(), mappedMemory, pixelData.size());
        vkUnmapMemory(device, readbackMemory);

        const auto writeFunc = [](void* filenamePtr, void* pngData, int dataLength)
        {
            tgui::writeFile(*static_cast<decltype(filename)*>(filenamePtr),
                            tgui::CharStringView(static_cast<const char*>(pngData), static_cast<std::size_t>(dataLength)));
        };
        REQUIRE(stbi_write_png_to_func(writeFunc,
                                       &filename,
                                       static_cast<int>(imageSize.x),
                                       static_cast<int>(imageSize.y),
                                       4,
                                       pixelData.data(),
                                       static_cast<int>(imageSize.x * 4)));

        vkDestroyCommandPool(device, commandPool, nullptr);
        vkDestroyBuffer(device, readbackBuffer, nullptr);
        vkFreeMemory(device, readbackMemory, nullptr);
        vkDestroyFramebuffer(device, framebuffer, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
        vkDestroyImageView(device, imageView, nullptr);
        vkDestroyImage(device, image, nullptr);
        vkFreeMemory(device, imageMemory, nullptr);

    #ifdef TGUI_ENABLE_DRAW_TESTS
            // Comparing the output files is not possible because the expected images were rendered with SFML (and thus might differ slightly).
            // However this line can be uncommented if the files in the expected folder were replaced with ones generated with a Vulkan backend.
            //compareImageFiles(filename, "expected/" + tgui::String(filename));
    #endif
        return;
    }
#endif // TGUI_HAS_RENDERER_BACKEND_VULKAN

    gui.draw();
}
