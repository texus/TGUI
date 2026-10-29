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

#ifndef TGUI_BACKEND_RENDERER_VULKAN_HPP
#define TGUI_BACKEND_RENDERER_VULKAN_HPP

#include <TGUI/Backend/Renderer/BackendRenderer.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendRenderTargetVulkan.hpp>
#include <TGUI/Backend/Renderer/Vulkan/BackendTextureVulkan.hpp>
#include <TGUI/Backend/Renderer/Vulkan/CanvasVulkan.hpp>

#include <functional>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Vulkan objects that TGUI needs to render
    ///
    /// The queue must support graphics operations. It also has to support presenting to the window surface if the
    /// gui.mainLoop() function is used. Command buffers in which the gui is drawn must be submitted to this queue,
    /// as texture uploads are submitted to it as well. The objects must remain alive for as long as TGUI uses them.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct VulkanContext
    {
        VkInstance instance = VK_NULL_HANDLE;             //!< Instance that was used to create the device
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; //!< Physical device on which the device was created
        std::uint32_t queueFamilyIndex = 0;               //!< Queue family of the queue
        VkDevice device = VK_NULL_HANDLE;                 //!< Logical device used for rendering
        VkQueue queue = VK_NULL_HANDLE;                   //!< Queue that is used for rendering

        /// Function used to load all Vulkan functions, it must belong to the loader that created the instance.
        /// The Gui classes from GLFW_VULKAN and SDL_VULKAN take it from GLFW or SDL when it is left empty.
        PFN_vkGetInstanceProcAddr getInstanceProcAddr = nullptr;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Backend renderer that uses Vulkan
    ///
    /// The renderer doesn't create any Vulkan instance or device itself, these have to be provided by the user.
    /// TGUI doesn't link to the Vulkan loader, all Vulkan functions are loaded via the provided vkGetInstanceProcAddr.
    /// All TGUI functions must be called from the same thread, as TGUI submits work to the provided queue when
    /// textures are loaded. These uploads are submitted without waiting for them to finish, so any work that accesses
    /// the images of TGUI textures has to be submitted to the same queue.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRendererVulkan : public BackendRenderer, public std::enable_shared_from_this<BackendRendererVulkan>
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Vulkan functions that are used by TGUI
        ///
        /// The surface and swapchain functions are nullptr when the VK_KHR_surface instance extension or
        /// VK_KHR_swapchain device extension weren't enabled. The debug utils functions are nullptr when the
        /// VK_EXT_debug_utils instance extension wasn't enabled.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct Functions
        {
            // Instance-level functions
            PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
            PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
            PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;

            // Instance-level functions from VK_KHR_surface
            PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
            PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
            PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
            PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;

            // Device-level functions
            PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = nullptr;
            PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = nullptr;
            PFN_vkAllocateMemory vkAllocateMemory = nullptr;
            PFN_vkBeginCommandBuffer vkBeginCommandBuffer = nullptr;
            PFN_vkBindBufferMemory vkBindBufferMemory = nullptr;
            PFN_vkBindImageMemory vkBindImageMemory = nullptr;
            PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = nullptr;
            PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = nullptr;
            PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer = nullptr;
            PFN_vkCmdBindPipeline vkCmdBindPipeline = nullptr;
            PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = nullptr;
            PFN_vkCmdClearColorImage vkCmdClearColorImage = nullptr;
            PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage = nullptr;
            PFN_vkCmdDrawIndexed vkCmdDrawIndexed = nullptr;
            PFN_vkCmdEndRenderPass vkCmdEndRenderPass = nullptr;
            PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = nullptr;
            PFN_vkCmdSetScissor vkCmdSetScissor = nullptr;
            PFN_vkCmdSetViewport vkCmdSetViewport = nullptr;
            PFN_vkCreateBuffer vkCreateBuffer = nullptr;
            PFN_vkCreateCommandPool vkCreateCommandPool = nullptr;
            PFN_vkCreateDescriptorPool vkCreateDescriptorPool = nullptr;
            PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = nullptr;
            PFN_vkCreateFence vkCreateFence = nullptr;
            PFN_vkCreateFramebuffer vkCreateFramebuffer = nullptr;
            PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = nullptr;
            PFN_vkCreateImage vkCreateImage = nullptr;
            PFN_vkCreateImageView vkCreateImageView = nullptr;
            PFN_vkCreatePipelineLayout vkCreatePipelineLayout = nullptr;
            PFN_vkCreateRenderPass vkCreateRenderPass = nullptr;
            PFN_vkCreateSampler vkCreateSampler = nullptr;
            PFN_vkCreateSemaphore vkCreateSemaphore = nullptr;
            PFN_vkCreateShaderModule vkCreateShaderModule = nullptr;
            PFN_vkDestroyBuffer vkDestroyBuffer = nullptr;
            PFN_vkDestroyCommandPool vkDestroyCommandPool = nullptr;
            PFN_vkDestroyDescriptorPool vkDestroyDescriptorPool = nullptr;
            PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout = nullptr;
            PFN_vkDestroyFence vkDestroyFence = nullptr;
            PFN_vkDestroyFramebuffer vkDestroyFramebuffer = nullptr;
            PFN_vkDestroyImage vkDestroyImage = nullptr;
            PFN_vkDestroyImageView vkDestroyImageView = nullptr;
            PFN_vkDestroyPipeline vkDestroyPipeline = nullptr;
            PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout = nullptr;
            PFN_vkDestroyRenderPass vkDestroyRenderPass = nullptr;
            PFN_vkDestroySampler vkDestroySampler = nullptr;
            PFN_vkDestroySemaphore vkDestroySemaphore = nullptr;
            PFN_vkDestroyShaderModule vkDestroyShaderModule = nullptr;
            PFN_vkEndCommandBuffer vkEndCommandBuffer = nullptr;
            PFN_vkFreeCommandBuffers vkFreeCommandBuffers = nullptr;
            PFN_vkFreeDescriptorSets vkFreeDescriptorSets = nullptr;
            PFN_vkFreeMemory vkFreeMemory = nullptr;
            PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = nullptr;
            PFN_vkGetFenceStatus vkGetFenceStatus = nullptr;
            PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = nullptr;
            PFN_vkMapMemory vkMapMemory = nullptr;
            PFN_vkQueueSubmit vkQueueSubmit = nullptr;
            PFN_vkQueueWaitIdle vkQueueWaitIdle = nullptr;
            PFN_vkResetCommandBuffer vkResetCommandBuffer = nullptr;
            PFN_vkResetFences vkResetFences = nullptr;
            PFN_vkUnmapMemory vkUnmapMemory = nullptr;
            PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = nullptr;
            PFN_vkWaitForFences vkWaitForFences = nullptr;

            // Device-level function from the VK_EXT_debug_utils instance extension
            PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;

            // Device-level functions from VK_KHR_swapchain
            PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = nullptr;
            PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = nullptr;
            PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR = nullptr;
            PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = nullptr;
            PFN_vkQueuePresentKHR vkQueuePresentKHR = nullptr;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Initializes the backend renderer
        ///
        /// @param vulkanContext  Vulkan objects that will be used for rendering
        ///
        /// @throw Exception when getInstanceProcAddr is empty, when a Vulkan function couldn't be loaded or when a Vulkan
        ///        object couldn't be created
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendRendererVulkan(const VulkanContext& vulkanContext);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendRendererVulkan() override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a new empty texture object
        /// @return Shared pointer to a new texture object
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] std::shared_ptr<BackendTexture> createTexture() override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the maximum allowed texture size
        ///
        /// @return Maximum width and height that you should try to use in a single texture
        ///
        /// This maximum size is defined by the graphics driver. Most likely this will return 8192 or 16384.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] unsigned int getMaximumTextureSize() override;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the Vulkan objects that were passed to the constructor of this object
        ///
        /// @return Vulkan objects that are used for rendering
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] const VulkanContext& getVulkanContext() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the Vulkan functions that were loaded by the renderer
        ///
        /// @return Function pointers that TGUI uses to call Vulkan functions
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] const Functions& getFunctions() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the GPU converts colors from linear to sRGB space when writing to an image of a format
        ///
        /// @param format  Format of the color attachment
        ///
        /// @return True for the 8-bit *_SRGB formats, false otherwise
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] static bool isSrgbFormat(VkFormat format);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Creates a graphics pipeline that can be used to draw the gui
        ///
        /// @param renderPass        Render pass in which the pipeline will be used, or VK_NULL_HANDLE for dynamic rendering
        /// @param subpass           Index of the subpass in which the pipeline will be used
        /// @param colorFormat       Format of the color attachment
        /// @param depthFormat       Format of the depth attachment (only used with dynamic rendering)
        /// @param stencilFormat     Format of the stencil attachment (only used with dynamic rendering)
        /// @param samples           Amount of samples in the color attachment
        ///
        /// @return Created pipeline or VK_NULL_HANDLE on failure. The caller is responsible for destroying it.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkPipeline createPipeline(
            VkRenderPass renderPass,
            std::uint32_t subpass,
            VkFormat colorFormat,
            VkFormat depthFormat,
            VkFormat stencilFormat,
            VkSampleCountFlagBits samples) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns the pipeline layout that is shared by all pipelines created with createPipeline
        ///
        /// Descriptor set 0 contains the texture (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE) and
        /// descriptor set 1 contains the sampler (VK_DESCRIPTOR_TYPE_SAMPLER).
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Returns the descriptor set to bind as set 1 in the pipeline layout
        ///
        /// @param smooth  Should the linear sampler be returned instead of the nearest-neighbor sampler?
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkDescriptorSet getSamplerDescriptorSet(bool smooth) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Allocates a descriptor set that can be bound as set 0 in the pipeline layout
        ///
        /// @param imageView  Image view to store in the descriptor set
        /// @param pool       Descriptor pool from which the set was allocated, needed to free the set later
        ///
        /// @return Descriptor set, or VK_NULL_HANDLE when allocation failed
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] VkDescriptorSet allocateTextureDescriptorSet(VkImageView imageView, VkDescriptorPool& pool);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Frees a descriptor set that was allocated with allocateTextureDescriptorSet
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void freeTextureDescriptorSet(VkDescriptorSet descriptorSet, VkDescriptorPool pool);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Creates a buffer and binds newly allocated memory to it
        ///
        /// @return True on success, false if the buffer couldn't be created
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] bool createBuffer(VkDeviceSize size,
                                        VkBufferUsageFlags usage,
                                        VkMemoryPropertyFlags memoryProperties,
                                        VkBuffer& buffer,
                                        VkDeviceMemory& memory) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Finds a memory type that matches the requirements
        ///
        /// @return Index of the memory type, or a value of at least VK_MAX_MEMORY_TYPES if no suitable memory type exists
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] std::uint32_t findMemoryType(std::uint32_t memoryTypeBits, VkMemoryPropertyFlags properties) const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Records commands in a command buffer and submits it to the queue, without waiting for it to finish
        ///
        /// @param recordFunc     Function that records the commands into the provided command buffer
        /// @param stagingBuffer  Buffer used by the commands (or VK_NULL_HANDLE). The renderer takes ownership of it and
        ///                       destroys it once the commands have finished executing, even when submitting fails.
        /// @param stagingMemory  Memory that is bound to the staging buffer, it is freed together with the buffer
        ///
        /// @return Id that can be passed to waitForUpload, or 0 if the commands couldn't be submitted
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] std::uint64_t submitUploadCommands(const std::function<void(VkCommandBuffer)>& recordFunc,
                                                         VkBuffer stagingBuffer,
                                                         VkDeviceMemory stagingMemory);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Waits until an upload, and all uploads that were submitted before it, have finished executing
        ///
        /// @param uploadId  Id that was returned by submitUploadCommands
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void waitForUpload(std::uint64_t uploadId);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Frees the staging buffers of uploads that have finished executing
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void releaseFinishedUploads();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Gives a name to a Vulkan object, which is shown in validation messages and debugging tools
        ///
        /// Nothing happens when the VK_EXT_debug_utils instance extension wasn't enabled.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename HandleType>
        void setObjectName(VkObjectType objectType, HandleType handle, const char* name) const
        {
            if (!m_functions.vkSetDebugUtilsObjectNameEXT || (handle == VK_NULL_HANDLE))
                return;

            VkDebugUtilsObjectNameInfoEXT nameInfo = {};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = objectType;
            nameInfo.objectHandle = reinterpret_cast<std::uint64_t>(handle);
            nameInfo.pObjectName = name;
            m_functions.vkSetDebugUtilsObjectNameEXT(m_vulkanContext.device, &nameInfo);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        struct Upload
        {
            VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
            VkFence fence = VK_NULL_HANDLE; // Unsignaled while the upload slot isn't in use
            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
            std::uint64_t id = 0; // 0 when the upload slot isn't in use
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destroys the staging buffer of an upload that finished executing and marks the upload slot as unused
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void finishUpload(Upload& upload);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Creates the Vulkan objects that are shared between all textures and render targets
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void createVulkanObjects();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destroys the objects that were created in createVulkanObjects
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void destroyVulkanObjects();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        struct DescriptorPool
        {
            VkDescriptorPool pool;
            std::uint32_t allocatedSets;
        };

        VulkanContext m_vulkanContext;
        Functions m_functions;
        VkPhysicalDeviceMemoryProperties m_memoryProperties = {};
        unsigned int m_maxTextureSize = 0;

        VkShaderModule m_vertexShaderModule = VK_NULL_HANDLE;
        VkShaderModule m_fragmentShaderModule = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_textureDescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_samplerDescriptorSetLayout = VK_NULL_HANDLE;
        VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

        VkSampler m_samplerNearest = VK_NULL_HANDLE;
        VkSampler m_samplerLinear = VK_NULL_HANDLE;
        VkDescriptorPool m_samplerDescriptorPool = VK_NULL_HANDLE;
        VkDescriptorSet m_samplerDescriptorSetNearest = VK_NULL_HANDLE;
        VkDescriptorSet m_samplerDescriptorSetLinear = VK_NULL_HANDLE;
        std::vector<DescriptorPool> m_textureDescriptorPools;

        VkCommandPool m_commandPool = VK_NULL_HANDLE;
        std::vector<Upload> m_uploads;
        std::uint64_t m_lastUploadId = 0;
    };
} // namespace tgui

#endif // TGUI_BACKEND_RENDERER_VULKAN_HPP
