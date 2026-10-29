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
#include <TGUI/Backend/Renderer/Vulkan/CanvasVulkan.hpp>

#include <TGUI/Loading/WidgetFactory.hpp>

#include <algorithm>
#include <array>
#include <type_traits>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // To edit the shaders, change the GLSL source files in src/Backend/Renderer/Vulkan/shaders and run compile_shaders.sh in
    // that folder to compile the shaders and generate the c++ code found below.

    static const std::uint32_t vertexShaderSPV[] =
        {0x07230203, 0x00010000, 0x000d000b, 0x00000023, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47,
         0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x000b000f, 0x00000000, 0x00000004, 0x6e69616d,
         0x00000000, 0x00000009, 0x0000000b, 0x0000000f, 0x00000011, 0x00000018, 0x0000001b, 0x00040047, 0x00000009, 0x0000001e,
         0x00000000, 0x00040047, 0x0000000b, 0x0000001e, 0x00000001, 0x00040047, 0x0000000f, 0x0000001e, 0x00000001, 0x00040047,
         0x00000011, 0x0000001e, 0x00000002, 0x00030047, 0x00000016, 0x00000002, 0x00050048, 0x00000016, 0x00000000, 0x0000000b,
         0x00000000, 0x00050048, 0x00000016, 0x00000001, 0x0000000b, 0x00000001, 0x00050048, 0x00000016, 0x00000002, 0x0000000b,
         0x00000003, 0x00050048, 0x00000016, 0x00000003, 0x0000000b, 0x00000004, 0x00040047, 0x0000001b, 0x0000001e, 0x00000000,
         0x00020013, 0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007,
         0x00000006, 0x00000004, 0x00040020, 0x00000008, 0x00000003, 0x00000007, 0x0004003b, 0x00000008, 0x00000009, 0x00000003,
         0x00040020, 0x0000000a, 0x00000001, 0x00000007, 0x0004003b, 0x0000000a, 0x0000000b, 0x00000001, 0x00040017, 0x0000000d,
         0x00000006, 0x00000002, 0x00040020, 0x0000000e, 0x00000003, 0x0000000d, 0x0004003b, 0x0000000e, 0x0000000f, 0x00000003,
         0x00040020, 0x00000010, 0x00000001, 0x0000000d, 0x0004003b, 0x00000010, 0x00000011, 0x00000001, 0x00040015, 0x00000013,
         0x00000020, 0x00000000, 0x0004002b, 0x00000013, 0x00000014, 0x00000001, 0x0004001c, 0x00000015, 0x00000006, 0x00000014,
         0x0006001e, 0x00000016, 0x00000007, 0x00000006, 0x00000015, 0x00000015, 0x00040020, 0x00000017, 0x00000003, 0x00000016,
         0x0004003b, 0x00000017, 0x00000018, 0x00000003, 0x00040015, 0x00000019, 0x00000020, 0x00000001, 0x0004002b, 0x00000019,
         0x0000001a, 0x00000000, 0x0004003b, 0x00000010, 0x0000001b, 0x00000001, 0x0004002b, 0x00000006, 0x0000001d, 0x00000000,
         0x0004002b, 0x00000006, 0x0000001e, 0x3f800000, 0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8,
         0x00000005, 0x0004003d, 0x00000007, 0x0000000c, 0x0000000b, 0x0003003e, 0x00000009, 0x0000000c, 0x0004003d, 0x0000000d,
         0x00000012, 0x00000011, 0x0003003e, 0x0000000f, 0x00000012, 0x0004003d, 0x0000000d, 0x0000001c, 0x0000001b, 0x00050051,
         0x00000006, 0x0000001f, 0x0000001c, 0x00000000, 0x00050051, 0x00000006, 0x00000020, 0x0000001c, 0x00000001, 0x00070050,
         0x00000007, 0x00000021, 0x0000001f, 0x00000020, 0x0000001d, 0x0000001e, 0x00050041, 0x00000008, 0x00000022, 0x00000018,
         0x0000001a, 0x0003003e, 0x00000022, 0x00000021, 0x000100fd, 0x00010038};

    static const std::uint32_t fragmentShaderSPV[] =
        {0x07230203, 0x00010000, 0x000d000b, 0x00000050, 0x00000000, 0x00020011, 0x00000001, 0x0006000b, 0x00000001, 0x4c534c47,
         0x6474732e, 0x3035342e, 0x00000000, 0x0003000e, 0x00000000, 0x00000001, 0x0008000f, 0x00000004, 0x00000004, 0x6e69616d,
         0x00000000, 0x00000009, 0x00000016, 0x0000001a, 0x00030010, 0x00000004, 0x00000007, 0x00040047, 0x00000009, 0x0000001e,
         0x00000000, 0x00040047, 0x0000000c, 0x00000021, 0x00000000, 0x00040047, 0x0000000c, 0x00000022, 0x00000000, 0x00040047,
         0x00000010, 0x00000021, 0x00000000, 0x00040047, 0x00000010, 0x00000022, 0x00000001, 0x00040047, 0x00000016, 0x0000001e,
         0x00000001, 0x00040047, 0x0000001a, 0x0000001e, 0x00000000, 0x00040047, 0x0000001e, 0x00000001, 0x00000000, 0x00020013,
         0x00000002, 0x00030021, 0x00000003, 0x00000002, 0x00030016, 0x00000006, 0x00000020, 0x00040017, 0x00000007, 0x00000006,
         0x00000004, 0x00040020, 0x00000008, 0x00000003, 0x00000007, 0x0004003b, 0x00000008, 0x00000009, 0x00000003, 0x00090019,
         0x0000000a, 0x00000006, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x00040020, 0x0000000b,
         0x00000000, 0x0000000a, 0x0004003b, 0x0000000b, 0x0000000c, 0x00000000, 0x0002001a, 0x0000000e, 0x00040020, 0x0000000f,
         0x00000000, 0x0000000e, 0x0004003b, 0x0000000f, 0x00000010, 0x00000000, 0x0003001b, 0x00000012, 0x0000000a, 0x00040017,
         0x00000014, 0x00000006, 0x00000002, 0x00040020, 0x00000015, 0x00000001, 0x00000014, 0x0004003b, 0x00000015, 0x00000016,
         0x00000001, 0x00040020, 0x00000019, 0x00000001, 0x00000007, 0x0004003b, 0x00000019, 0x0000001a, 0x00000001, 0x00020014,
         0x0000001d, 0x00030031, 0x0000001d, 0x0000001e, 0x00040017, 0x00000021, 0x00000006, 0x00000003, 0x0004002b, 0x00000006,
         0x0000002c, 0x3d6147ae, 0x0004002b, 0x00000006, 0x00000032, 0x4019999a, 0x0006002c, 0x00000021, 0x00000033, 0x00000032,
         0x00000032, 0x00000032, 0x0004002b, 0x00000006, 0x00000039, 0x3d25aee6, 0x0006002c, 0x00000021, 0x0000003a, 0x00000039,
         0x00000039, 0x00000039, 0x00040017, 0x0000003b, 0x0000001d, 0x00000003, 0x00040015, 0x0000003e, 0x00000020, 0x00000000,
         0x0004002b, 0x0000003e, 0x0000003f, 0x00000000, 0x00040020, 0x00000040, 0x00000003, 0x00000006, 0x0004002b, 0x0000003e,
         0x00000043, 0x00000001, 0x0004002b, 0x0000003e, 0x00000046, 0x00000002, 0x0006002c, 0x00000021, 0x0000004a, 0x0000002c,
         0x0000002c, 0x0000002c, 0x0004002b, 0x00000006, 0x0000004c, 0x3d9e8391, 0x0006002c, 0x00000021, 0x0000004d, 0x0000004c,
         0x0000004c, 0x0000004c, 0x0004002b, 0x00000006, 0x0000004e, 0x3f72a76f, 0x0006002c, 0x00000021, 0x0000004f, 0x0000004e,
         0x0000004e, 0x0000004e, 0x00050036, 0x00000002, 0x00000004, 0x00000000, 0x00000003, 0x000200f8, 0x00000005, 0x0004003d,
         0x0000000a, 0x0000000d, 0x0000000c, 0x0004003d, 0x0000000e, 0x00000011, 0x00000010, 0x00050056, 0x00000012, 0x00000013,
         0x0000000d, 0x00000011, 0x0004003d, 0x00000014, 0x00000017, 0x00000016, 0x00050057, 0x00000007, 0x00000018, 0x00000013,
         0x00000017, 0x0004003d, 0x00000007, 0x0000001b, 0x0000001a, 0x00050085, 0x00000007, 0x0000001c, 0x00000018, 0x0000001b,
         0x0003003e, 0x00000009, 0x0000001c, 0x000300f7, 0x00000020, 0x00000000, 0x000400fa, 0x0000001e, 0x0000001f, 0x00000020,
         0x000200f8, 0x0000001f, 0x0004003d, 0x00000007, 0x00000024, 0x00000009, 0x0008004f, 0x00000021, 0x00000025, 0x00000024,
         0x00000024, 0x00000000, 0x00000001, 0x00000002, 0x00050085, 0x00000021, 0x00000028, 0x00000025, 0x0000004d, 0x0004003d,
         0x00000007, 0x0000002a, 0x00000009, 0x0008004f, 0x00000021, 0x0000002b, 0x0000002a, 0x0000002a, 0x00000000, 0x00000001,
         0x00000002, 0x00050081, 0x00000021, 0x0000002e, 0x0000002b, 0x0000004a, 0x00050085, 0x00000021, 0x00000031, 0x0000002e,
         0x0000004f, 0x0007000c, 0x00000021, 0x00000034, 0x00000001, 0x0000001a, 0x00000031, 0x00000033, 0x0004003d, 0x00000007,
         0x00000037, 0x00000009, 0x0008004f, 0x00000021, 0x00000038, 0x00000037, 0x00000037, 0x00000000, 0x00000001, 0x00000002,
         0x000500bc, 0x0000003b, 0x0000003c, 0x00000038, 0x0000003a, 0x000600a9, 0x00000021, 0x0000003d, 0x0000003c, 0x00000028,
         0x00000034, 0x00050041, 0x00000040, 0x00000041, 0x00000009, 0x0000003f, 0x00050051, 0x00000006, 0x00000042, 0x0000003d,
         0x00000000, 0x0003003e, 0x00000041, 0x00000042, 0x00050041, 0x00000040, 0x00000044, 0x00000009, 0x00000043, 0x00050051,
         0x00000006, 0x00000045, 0x0000003d, 0x00000001, 0x0003003e, 0x00000044, 0x00000045, 0x00050041, 0x00000040, 0x00000047,
         0x00000009, 0x00000046, 0x00050051, 0x00000006, 0x00000048, 0x0000003d, 0x00000002, 0x0003003e, 0x00000047, 0x00000048,
         0x000200f9, 0x00000020, 0x000200f8, 0x00000020, 0x000100fd, 0x00010038};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererVulkan::BackendRendererVulkan(const VulkanContext& vulkanContext) :
        m_vulkanContext{vulkanContext}
    {
        const Functions& fn = m_functions;

        TGUI_ASSERT(m_vulkanContext.instance && m_vulkanContext.physicalDevice && m_vulkanContext.device && m_vulkanContext.queue,
                    "instance, physicalDevice, device and queue passed to BackendRendererVulkan can't be VK_NULL_HANDLE");
        if (!m_vulkanContext.getInstanceProcAddr)
            throw Exception("Creating BackendRendererVulkan failed: getInstanceProcAddr in VulkanContext can't be a nullptr");

        // Loads a Vulkan function and throws when a required function isn't available.
        // Device-level functions are loaded via vkGetDeviceProcAddr so that calls don't need to go through the loader.
        const auto loadInstanceFunction = [this](auto& func, const char* name, bool required)
        {
            func = reinterpret_cast<std::remove_reference_t<decltype(func)>>(
                m_vulkanContext.getInstanceProcAddr(m_vulkanContext.instance, name));
            if (!func && required)
                throw Exception("Creating BackendRendererVulkan failed: failed to load " + String(name));
        };
        const auto loadDeviceFunction = [this](auto& func, const char* name, bool required)
        {
            func = reinterpret_cast<std::remove_reference_t<decltype(func)>>(
                m_functions.vkGetDeviceProcAddr(m_vulkanContext.device, name));
            if (!func && required)
                throw Exception("Creating BackendRendererVulkan failed: failed to load " + String(name));
        };

#define TGUI_LOAD_INSTANCE_FUNCTION(func) loadInstanceFunction(m_functions.func, #func, true)
#define TGUI_LOAD_OPTIONAL_INSTANCE_FUNCTION(func) loadInstanceFunction(m_functions.func, #func, false)
#define TGUI_LOAD_DEVICE_FUNCTION(func) loadDeviceFunction(m_functions.func, #func, true)
#define TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(func) loadDeviceFunction(m_functions.func, #func, false)

        TGUI_LOAD_INSTANCE_FUNCTION(vkGetDeviceProcAddr);
        TGUI_LOAD_INSTANCE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
        TGUI_LOAD_INSTANCE_FUNCTION(vkGetPhysicalDeviceProperties);

        // The surface, debug utils and swapchain functions are optional, they are nullptr when their extension isn't enabled
        TGUI_LOAD_OPTIONAL_INSTANCE_FUNCTION(vkDestroySurfaceKHR);
        TGUI_LOAD_OPTIONAL_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
        TGUI_LOAD_OPTIONAL_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
        TGUI_LOAD_OPTIONAL_INSTANCE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);

        TGUI_LOAD_DEVICE_FUNCTION(vkAllocateCommandBuffers);
        TGUI_LOAD_DEVICE_FUNCTION(vkAllocateDescriptorSets);
        TGUI_LOAD_DEVICE_FUNCTION(vkAllocateMemory);
        TGUI_LOAD_DEVICE_FUNCTION(vkBeginCommandBuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkBindBufferMemory);
        TGUI_LOAD_DEVICE_FUNCTION(vkBindImageMemory);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdBeginRenderPass);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdBindPipeline);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdClearColorImage);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdDrawIndexed);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdEndRenderPass);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdPipelineBarrier);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdSetScissor);
        TGUI_LOAD_DEVICE_FUNCTION(vkCmdSetViewport);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateBuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateCommandPool);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateDescriptorPool);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateFence);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateFramebuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateImage);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateImageView);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreatePipelineLayout);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateRenderPass);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateSampler);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateSemaphore);
        TGUI_LOAD_DEVICE_FUNCTION(vkCreateShaderModule);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyBuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyCommandPool);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyDescriptorPool);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyFence);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyFramebuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyImage);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyImageView);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyPipeline);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyPipelineLayout);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyRenderPass);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroySampler);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroySemaphore);
        TGUI_LOAD_DEVICE_FUNCTION(vkDestroyShaderModule);
        TGUI_LOAD_DEVICE_FUNCTION(vkEndCommandBuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkFreeCommandBuffers);
        TGUI_LOAD_DEVICE_FUNCTION(vkFreeDescriptorSets);
        TGUI_LOAD_DEVICE_FUNCTION(vkFreeMemory);
        TGUI_LOAD_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
        TGUI_LOAD_DEVICE_FUNCTION(vkGetFenceStatus);
        TGUI_LOAD_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
        TGUI_LOAD_DEVICE_FUNCTION(vkMapMemory);
        TGUI_LOAD_DEVICE_FUNCTION(vkQueueSubmit);
        TGUI_LOAD_DEVICE_FUNCTION(vkQueueWaitIdle);
        TGUI_LOAD_DEVICE_FUNCTION(vkResetCommandBuffer);
        TGUI_LOAD_DEVICE_FUNCTION(vkResetFences);
        TGUI_LOAD_DEVICE_FUNCTION(vkUnmapMemory);
        TGUI_LOAD_DEVICE_FUNCTION(vkUpdateDescriptorSets);
        TGUI_LOAD_DEVICE_FUNCTION(vkWaitForFences);

        // Some loaders (e.g. on Android) return a function from vkGetInstanceProcAddr even when VK_EXT_debug_utils isn't
        // enabled, while vkGetDeviceProcAddr does return a nullptr in that case.
        TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(vkSetDebugUtilsObjectNameEXT);

        TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(vkAcquireNextImageKHR);
        TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(vkCreateSwapchainKHR);
        TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(vkDestroySwapchainKHR);
        TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
        TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION(vkQueuePresentKHR);

#undef TGUI_LOAD_INSTANCE_FUNCTION
#undef TGUI_LOAD_OPTIONAL_INSTANCE_FUNCTION
#undef TGUI_LOAD_DEVICE_FUNCTION
#undef TGUI_LOAD_OPTIONAL_DEVICE_FUNCTION

        if (!WidgetFactory::getConstructFunction(U"CanvasVulkan"))
            WidgetFactory::setConstructFunction(U"CanvasVulkan", std::make_shared<CanvasVulkan>);

        fn.vkGetPhysicalDeviceMemoryProperties(m_vulkanContext.physicalDevice, &m_memoryProperties);

        VkPhysicalDeviceProperties deviceProperties;
        fn.vkGetPhysicalDeviceProperties(m_vulkanContext.physicalDevice, &deviceProperties);
        m_maxTextureSize = deviceProperties.limits.maxImageDimension2D;

        try
        {
            createVulkanObjects();
        }
        catch (const Exception&)
        {
            destroyVulkanObjects();
            throw;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererVulkan::~BackendRendererVulkan()
    {
        const Functions& fn = m_functions;
        const VkDevice device = m_vulkanContext.device;

        // The staging buffers can only be destroyed after the GPU finished the uploads that use them
        waitForUpload(m_lastUploadId);
        for (const auto& upload : m_uploads)
            fn.vkDestroyFence(device, upload.fence, nullptr);

        for (const auto& descriptorPool : m_textureDescriptorPools)
            fn.vkDestroyDescriptorPool(device, descriptorPool.pool, nullptr);

        destroyVulkanObjects();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendRendererVulkan::createTexture()
    {
        return std::make_shared<BackendTextureVulkan>(shared_from_this());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendRendererVulkan::getMaximumTextureSize()
    {
        return m_maxTextureSize;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const VulkanContext& BackendRendererVulkan::getVulkanContext() const
    {
        return m_vulkanContext;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const BackendRendererVulkan::Functions& BackendRendererVulkan::getFunctions() const
    {
        return m_functions;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendRendererVulkan::isSrgbFormat(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_R8_SRGB:
            case VK_FORMAT_R8G8_SRGB:
            case VK_FORMAT_R8G8B8_SRGB:
            case VK_FORMAT_B8G8R8_SRGB:
            case VK_FORMAT_R8G8B8A8_SRGB:
            case VK_FORMAT_B8G8R8A8_SRGB:
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
                return true;
            default:
                return false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkPipeline BackendRendererVulkan::createPipeline(
        VkRenderPass renderPass,
        std::uint32_t subpass,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkFormat stencilFormat,
        VkSampleCountFlagBits samples) const
    {
        const Functions& fn = m_functions;

        std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages = {};
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = m_vertexShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = m_fragmentShaderModule;
        shaderStages[1].pName = "main";

        // When the GPU converts the output to sRGB, the fragment shader first has to convert the colors to linear
        const VkBool32 convertToLinear = isSrgbFormat(colorFormat) ? VK_TRUE : VK_FALSE;
        VkSpecializationMapEntry specializationEntry = {};
        specializationEntry.constantID = 0;
        specializationEntry.size = sizeof(VkBool32);

        VkSpecializationInfo specializationInfo = {};
        specializationInfo.mapEntryCount = 1;
        specializationInfo.pMapEntries = &specializationEntry;
        specializationInfo.dataSize = sizeof(VkBool32);
        specializationInfo.pData = &convertToLinear;
        shaderStages[1].pSpecializationInfo = &specializationInfo;

        // Position is stored as x,y in the first 2 floats
        // Color is stored as r,g,b,a in the next 4 bytes
        // Texture coordinate is stored as u,v in the last 2 floats
        static_assert(sizeof(Vertex) == 8 + 4 + 8, "Size of tgui::Vertex has to match the Vulkan backend shader input");

        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].format = VK_FORMAT_R8G8B8A8_UNORM;
        attributeDescriptions[1].offset = sizeof(float) * 2;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = sizeof(float) * 2 + sizeof(std::uint8_t) * 4;

        VkPipelineVertexInputStateCreateInfo vertexInputState = {};
        vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputState.vertexBindingDescriptionCount = 1;
        vertexInputState.pVertexBindingDescriptions = &bindingDescription;
        vertexInputState.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributeDescriptions.size());
        vertexInputState.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
        inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        // Viewport and scissor are set dynamically
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizationState = {};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_NONE;
        rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationState.lineWidth = 1;

        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = samples;

        // The depth and stencil tests are disabled, but the state is required if the render pass has a depth attachment
        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        VkPipelineColorBlendAttachmentState blendAttachmentState = {};
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
                                              | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &blendAttachmentState;

        const std::array<VkDynamicState, 2> dynamicStates = {{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR}};
        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<std::uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputState;
        pipelineInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizationState;
        pipelineInfo.pMultisampleState = &multisampleState;
        pipelineInfo.pDepthStencilState = &depthStencilState;
        pipelineInfo.pColorBlendState = &colorBlendState;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = subpass;

#ifdef VK_KHR_dynamic_rendering
        VkPipelineRenderingCreateInfoKHR renderingInfo = {};
        if (renderPass == VK_NULL_HANDLE)
        {
            renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachmentFormats = &colorFormat;
            renderingInfo.depthAttachmentFormat = depthFormat;
            renderingInfo.stencilAttachmentFormat = stencilFormat;
            pipelineInfo.pNext = &renderingInfo;
        }
#else
        (void)colorFormat;
        (void)depthFormat;
        (void)stencilFormat;
        TGUI_ASSERT(renderPass != VK_NULL_HANDLE,
                    "Dynamic rendering requires Vulkan headers that support VK_KHR_dynamic_rendering");
#endif

        VkPipeline pipeline = VK_NULL_HANDLE;
        if (fn.vkCreateGraphicsPipelines(m_vulkanContext.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
            return VK_NULL_HANDLE;

        setObjectName(VK_OBJECT_TYPE_PIPELINE, pipeline, "TGUI pipeline");
        return pipeline;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkPipelineLayout BackendRendererVulkan::getPipelineLayout() const
    {
        return m_pipelineLayout;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkDescriptorSet BackendRendererVulkan::getSamplerDescriptorSet(bool smooth) const
    {
        return smooth ? m_samplerDescriptorSetLinear : m_samplerDescriptorSetNearest;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VkDescriptorSet BackendRendererVulkan::allocateTextureDescriptorSet(VkImageView imageView, VkDescriptorPool& pool)
    {
        const Functions& fn = m_functions;

        // Descriptor pools are created on demand. We keep track of the amount of sets in each pool ourselves, because
        // Vulkan 1.0 doesn't guarantee that vkAllocateDescriptorSets returns an error when the pool is full.
        constexpr std::uint32_t setsPerPool = 64;
        auto poolIt = std::find_if(m_textureDescriptorPools.begin(),
                                   m_textureDescriptorPools.end(),
                                   [](const DescriptorPool& descriptorPool) { return descriptorPool.allocatedSets < setsPerPool; });
        if (poolIt == m_textureDescriptorPools.end())
        {
            VkDescriptorPoolSize poolSize = {};
            poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            poolSize.descriptorCount = setsPerPool;

            VkDescriptorPoolCreateInfo poolInfo = {};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = setsPerPool;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &poolSize;

            VkDescriptorPool newPool = VK_NULL_HANDLE;
            if (fn.vkCreateDescriptorPool(m_vulkanContext.device, &poolInfo, nullptr, &newPool) != VK_SUCCESS)
                return VK_NULL_HANDLE;

            setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, newPool, "TGUI texture descriptor pool");

            m_textureDescriptorPools.push_back({newPool, 0});
            poolIt = std::prev(m_textureDescriptorPools.end());
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = poolIt->pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &m_textureDescriptorSetLayout;

        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
        if (fn.vkAllocateDescriptorSets(m_vulkanContext.device, &allocInfo, &descriptorSet) != VK_SUCCESS)
            return VK_NULL_HANDLE;

        setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, descriptorSet, "TGUI texture descriptor set");

        ++poolIt->allocatedSets;
        pool = poolIt->pool;

        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageView = imageView;
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        descriptorWrite.pImageInfo = &imageInfo;
        fn.vkUpdateDescriptorSets(m_vulkanContext.device, 1, &descriptorWrite, 0, nullptr);

        return descriptorSet;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRendererVulkan::freeTextureDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorPool pool)
    {
        const Functions& fn = m_functions;

        auto poolIt = std::find_if(m_textureDescriptorPools.begin(),
                                   m_textureDescriptorPools.end(),
                                   [pool](const DescriptorPool& descriptorPool) { return descriptorPool.pool == pool; });
        TGUI_ASSERT(poolIt != m_textureDescriptorPools.end(),
                    "Descriptor set has to belong to one of the texture descriptor pools");
        if (poolIt == m_textureDescriptorPools.end())
            return;

        fn.vkFreeDescriptorSets(m_vulkanContext.device, pool, 1, &descriptorSet);
        --poolIt->allocatedSets;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendRendererVulkan::createBuffer(VkDeviceSize size,
                                             VkBufferUsageFlags usage,
                                             VkMemoryPropertyFlags memoryProperties,
                                             VkBuffer& buffer,
                                             VkDeviceMemory& memory) const
    {
        const Functions& fn = m_functions;

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (fn.vkCreateBuffer(m_vulkanContext.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
            return false;

        VkMemoryRequirements memoryRequirements;
        fn.vkGetBufferMemoryRequirements(m_vulkanContext.device, buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, memoryProperties);
        if ((allocInfo.memoryTypeIndex >= m_memoryProperties.memoryTypeCount)
            || (fn.vkAllocateMemory(m_vulkanContext.device, &allocInfo, nullptr, &memory) != VK_SUCCESS))
        {
            fn.vkDestroyBuffer(m_vulkanContext.device, buffer, nullptr);
            buffer = VK_NULL_HANDLE;
            return false;
        }

        if (fn.vkBindBufferMemory(m_vulkanContext.device, buffer, memory, 0) != VK_SUCCESS)
        {
            fn.vkDestroyBuffer(m_vulkanContext.device, buffer, nullptr);
            fn.vkFreeMemory(m_vulkanContext.device, memory, nullptr);
            buffer = VK_NULL_HANDLE;
            memory = VK_NULL_HANDLE;
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint32_t BackendRendererVulkan::findMemoryType(std::uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) const
    {
        for (std::uint32_t i = 0; i < m_memoryProperties.memoryTypeCount; ++i)
        {
            if ((memoryTypeBits & (1u << i)) && ((m_memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
                return i;
        }

        return VK_MAX_MEMORY_TYPES;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::uint64_t BackendRendererVulkan::submitUploadCommands(const std::function<void(VkCommandBuffer)>& recordFunc,
                                                              VkBuffer stagingBuffer,
                                                              VkDeviceMemory stagingMemory)
    {
        const Functions& fn = m_functions;
        const VkDevice device = m_vulkanContext.device;

        // Limit the amount of uploads that can be pending, to limit the memory that is used by the staging buffers
        constexpr std::size_t maxPendingUploads = 16;

        releaseFinishedUploads();

        // Find an upload slot that isn't in use. If too many uploads are still executing then we wait for the oldest one.
        auto uploadIt = std::find_if(m_uploads.begin(), m_uploads.end(), [](const Upload& upload) { return upload.id == 0; });
        if ((uploadIt == m_uploads.end()) && (m_uploads.size() >= maxPendingUploads))
        {
            uploadIt = std::min_element(m_uploads.begin(),
                                        m_uploads.end(),
                                        [](const Upload& left, const Upload& right) { return left.id < right.id; });
            waitForUpload(uploadIt->id);
        }

        if (uploadIt == m_uploads.end())
        {
            Upload newUpload;

            VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
            cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufferAllocInfo.commandPool = m_commandPool;
            cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufferAllocInfo.commandBufferCount = 1;
            VkFenceCreateInfo fenceInfo = {};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            if ((fn.vkAllocateCommandBuffers(device, &cmdBufferAllocInfo, &newUpload.cmdBuffer) != VK_SUCCESS)
                || (fn.vkCreateFence(device, &fenceInfo, nullptr, &newUpload.fence) != VK_SUCCESS))
            {
                if (newUpload.cmdBuffer != VK_NULL_HANDLE)
                    fn.vkFreeCommandBuffers(device, m_commandPool, 1, &newUpload.cmdBuffer);

                fn.vkDestroyBuffer(device, stagingBuffer, nullptr);
                fn.vkFreeMemory(device, stagingMemory, nullptr);
                return 0;
            }

            setObjectName(VK_OBJECT_TYPE_COMMAND_BUFFER, newUpload.cmdBuffer, "TGUI upload command buffer");
            setObjectName(VK_OBJECT_TYPE_FENCE, newUpload.fence, "TGUI upload fence");

            m_uploads.push_back(newUpload);
            uploadIt = std::prev(m_uploads.end());
        }

        Upload& upload = *uploadIt;
        upload.stagingBuffer = stagingBuffer;
        upload.stagingMemory = stagingMemory;

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if ((fn.vkResetCommandBuffer(upload.cmdBuffer, 0) != VK_SUCCESS)
            || (fn.vkBeginCommandBuffer(upload.cmdBuffer, &beginInfo) != VK_SUCCESS))
        {
            finishUpload(upload);
            return 0;
        }

        recordFunc(upload.cmdBuffer);

        if (fn.vkEndCommandBuffer(upload.cmdBuffer) != VK_SUCCESS)
        {
            finishUpload(upload);
            return 0;
        }

        // Later work on the queue is synchronized with the upload via the pipeline barriers recorded in the command
        // buffer, so we don't have to wait for the upload to finish. The fence tells us when the staging buffer can be freed.
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &upload.cmdBuffer;
        if (fn.vkQueueSubmit(m_vulkanContext.queue, 1, &submitInfo, upload.fence) != VK_SUCCESS)
        {
            finishUpload(upload);
            return 0;
        }

        upload.id = ++m_lastUploadId;
        return upload.id;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRendererVulkan::waitForUpload(std::uint64_t uploadId)
    {
        const Functions& fn = m_functions;

        // A fence only guarantees that the work of its own submission finished, so we also wait for older uploads
        for (Upload& upload : m_uploads)
        {
            if ((upload.id == 0) || (upload.id > uploadId))
                continue;

            fn.vkWaitForFences(m_vulkanContext.device, 1, &upload.fence, VK_TRUE, UINT64_MAX);
            finishUpload(upload);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRendererVulkan::releaseFinishedUploads()
    {
        const Functions& fn = m_functions;

        for (Upload& upload : m_uploads)
        {
            if ((upload.id != 0) && (fn.vkGetFenceStatus(m_vulkanContext.device, upload.fence) == VK_SUCCESS))
                finishUpload(upload);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRendererVulkan::finishUpload(Upload& upload)
    {
        const Functions& fn = m_functions;

        // The fence was only signaled if the upload was submitted
        const VkDevice device = m_vulkanContext.device;
        if (upload.id != 0)
            fn.vkResetFences(device, 1, &upload.fence);

        fn.vkDestroyBuffer(device, upload.stagingBuffer, nullptr);
        fn.vkFreeMemory(device, upload.stagingMemory, nullptr);
        upload.stagingBuffer = VK_NULL_HANDLE;
        upload.stagingMemory = VK_NULL_HANDLE;
        upload.id = 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRendererVulkan::createVulkanObjects()
    {
        const Functions& fn = m_functions;

        const VkDevice device = m_vulkanContext.device;

        // Create the shader modules
        VkShaderModuleCreateInfo shaderModuleInfo = {};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = sizeof(vertexShaderSPV);
        shaderModuleInfo.pCode = vertexShaderSPV;
        if (fn.vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &m_vertexShaderModule) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create vertex shader module");

        shaderModuleInfo.codeSize = sizeof(fragmentShaderSPV);
        shaderModuleInfo.pCode = fragmentShaderSPV;
        if (fn.vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &m_fragmentShaderModule) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create fragment shader module");

        setObjectName(VK_OBJECT_TYPE_SHADER_MODULE, m_vertexShaderModule, "TGUI vertex shader");
        setObjectName(VK_OBJECT_TYPE_SHADER_MODULE, m_fragmentShaderModule, "TGUI fragment shader");

        // Create the pipeline layout. Textures and samplers are stored in different descriptor sets so that the
        // descriptor set of a texture doesn't need to change when switching between smooth and non-smooth filtering.
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = 0;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo setLayoutInfo = {};
        setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        setLayoutInfo.bindingCount = 1;
        setLayoutInfo.pBindings = &layoutBinding;
        if (fn.vkCreateDescriptorSetLayout(device, &setLayoutInfo, nullptr, &m_textureDescriptorSetLayout) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create descriptor set layout");

        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        if (fn.vkCreateDescriptorSetLayout(device, &setLayoutInfo, nullptr, &m_samplerDescriptorSetLayout) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create descriptor set layout");

        const std::array<VkDescriptorSetLayout, 2> setLayouts = {{m_textureDescriptorSetLayout, m_samplerDescriptorSetLayout}};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<std::uint32_t>(setLayouts.size());
        pipelineLayoutInfo.pSetLayouts = setLayouts.data();
        if (fn.vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create pipeline layout");

        setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, m_textureDescriptorSetLayout, "TGUI texture descriptor set layout");
        setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, m_samplerDescriptorSetLayout, "TGUI sampler descriptor set layout");
        setObjectName(VK_OBJECT_TYPE_PIPELINE_LAYOUT, m_pipelineLayout, "TGUI pipeline layout");

        // Create the samplers and the descriptor sets that contain them
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        if (fn.vkCreateSampler(device, &samplerInfo, nullptr, &m_samplerNearest) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create sampler");

        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        if (fn.vkCreateSampler(device, &samplerInfo, nullptr, &m_samplerLinear) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create linear sampler");

        setObjectName(VK_OBJECT_TYPE_SAMPLER, m_samplerNearest, "TGUI nearest sampler");
        setObjectName(VK_OBJECT_TYPE_SAMPLER, m_samplerLinear, "TGUI linear sampler");

        VkDescriptorPoolSize samplerPoolSize = {};
        samplerPoolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
        samplerPoolSize.descriptorCount = 2;

        VkDescriptorPoolCreateInfo samplerPoolInfo = {};
        samplerPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        samplerPoolInfo.maxSets = 2;
        samplerPoolInfo.poolSizeCount = 1;
        samplerPoolInfo.pPoolSizes = &samplerPoolSize;
        if (fn.vkCreateDescriptorPool(device, &samplerPoolInfo, nullptr, &m_samplerDescriptorPool) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create descriptor pool");

        const std::array<VkDescriptorSetLayout, 2> samplerSetLayouts = {{m_samplerDescriptorSetLayout, m_samplerDescriptorSetLayout}};
        std::array<VkDescriptorSet, 2> samplerSets = {};
        VkDescriptorSetAllocateInfo samplerSetsAllocInfo = {};
        samplerSetsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        samplerSetsAllocInfo.descriptorPool = m_samplerDescriptorPool;
        samplerSetsAllocInfo.descriptorSetCount = static_cast<std::uint32_t>(samplerSetLayouts.size());
        samplerSetsAllocInfo.pSetLayouts = samplerSetLayouts.data();
        if (fn.vkAllocateDescriptorSets(device, &samplerSetsAllocInfo, samplerSets.data()) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to allocate descriptor sets");

        m_samplerDescriptorSetNearest = samplerSets[0];
        m_samplerDescriptorSetLinear = samplerSets[1];
        setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_POOL, m_samplerDescriptorPool, "TGUI sampler descriptor pool");
        setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, m_samplerDescriptorSetNearest, "TGUI nearest sampler descriptor set");
        setObjectName(VK_OBJECT_TYPE_DESCRIPTOR_SET, m_samplerDescriptorSetLinear, "TGUI linear sampler descriptor set");

        std::array<VkDescriptorImageInfo, 2> samplerImageInfos = {};
        samplerImageInfos[0].sampler = m_samplerNearest;
        samplerImageInfos[1].sampler = m_samplerLinear;

        std::array<VkWriteDescriptorSet, 2> samplerWrites = {};
        for (std::size_t i = 0; i < samplerWrites.size(); ++i)
        {
            samplerWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            samplerWrites[i].dstSet = samplerSets[i];
            samplerWrites[i].dstBinding = 0;
            samplerWrites[i].descriptorCount = 1;
            samplerWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
            samplerWrites[i].pImageInfo = &samplerImageInfos[i];
        }
        fn.vkUpdateDescriptorSets(device, static_cast<std::uint32_t>(samplerWrites.size()), samplerWrites.data(), 0, nullptr);

        // Create the command pool that is used to upload textures. The command buffers are allocated when needed.
        VkCommandPoolCreateInfo commandPoolInfo = {};
        commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        commandPoolInfo.queueFamilyIndex = m_vulkanContext.queueFamilyIndex;
        if (fn.vkCreateCommandPool(device, &commandPoolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
            throw Exception("Creating BackendRendererVulkan failed: failed to create command pool");

        setObjectName(VK_OBJECT_TYPE_COMMAND_POOL, m_commandPool, "TGUI upload command pool");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRendererVulkan::destroyVulkanObjects()
    {
        const Functions& fn = m_functions;

        // All destroy functions accept VK_NULL_HANDLE, so this function can be used when creation failed halfway
        const VkDevice device = m_vulkanContext.device;
        fn.vkDestroyCommandPool(device, m_commandPool, nullptr);
        fn.vkDestroyDescriptorPool(device, m_samplerDescriptorPool, nullptr);
        fn.vkDestroySampler(device, m_samplerLinear, nullptr);
        fn.vkDestroySampler(device, m_samplerNearest, nullptr);
        fn.vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
        fn.vkDestroyDescriptorSetLayout(device, m_samplerDescriptorSetLayout, nullptr);
        fn.vkDestroyDescriptorSetLayout(device, m_textureDescriptorSetLayout, nullptr);
        fn.vkDestroyShaderModule(device, m_fragmentShaderModule, nullptr);
        fn.vkDestroyShaderModule(device, m_vertexShaderModule, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
