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

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

#if TGUI_BUILD_CXX20_MODULE
    #include <TGUI/extlibs/IncludeSDL.hpp>

import tgui;
import tgui.backend.sdl_vulkan;
#else
    #include <TGUI/TGUI.hpp>

    #include <TGUI/Backend/SDL-Vulkan.hpp>
#endif

bool runExample(tgui::BackendGui& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying the Vulkan device
void run_application(SDL_Window* window,
                     VkInstance instance,
                     VkPhysicalDevice physicalDevice,
                     std::uint32_t queueFamilyIndex,
                     VkDevice device,
                     VkQueue queue)
{
    tgui::Gui gui(window, {instance, physicalDevice, queueFamilyIndex, device, queue});
    if (!runExample(gui))
        return;

    // To use your own main loop, call gui.setRenderPass(renderPass, swapchainFormat, maxFramesInFlight) once and then call
    // gui.draw(cmdBuffer) inside your render pass every frame. The maxFramesInFlight parameter must be at least the amount
    // of frames that your render loop allows the GPU to execute at the same time.
    gui.mainLoop();
}

// Note that no error checking is performed on SDL and Vulkan initialization in this example code
int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("TGUI example (SDL-Vulkan)", 800, 600, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    // Create the instance with the extensions that SDL needs to create a surface for the window
    Uint32 sdlExtensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);
    std::vector<const char*> instanceExtensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifdef __APPLE__
    // MoltenVK is only found when enabling the portability enumeration extension
    instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    instanceInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
    instanceInfo.enabledExtensionCount = static_cast<std::uint32_t>(instanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();

    VkInstance instance;
    vkCreateInstance(&instanceInfo, nullptr, &instance);

    // Find a GPU with a queue family that supports both graphics and presenting to the window
    std::uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    std::uint32_t queueFamilyIndex = 0;
    for (VkPhysicalDevice availablePhysicalDevice : physicalDevices)
    {
        std::uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(availablePhysicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(availablePhysicalDevice, &queueFamilyCount, queueFamilies.data());
        for (std::uint32_t i = 0; (i < queueFamilyCount) && !physicalDevice; ++i)
        {
            if ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                && SDL_Vulkan_GetPresentationSupport(instance, availablePhysicalDevice, i))
            {
                physicalDevice = availablePhysicalDevice;
                queueFamilyIndex = i;
            }
        }
    }

    // Create the device with the swapchain extension, which TGUI needs when you call gui.mainLoop()
    const float queuePriority = 1;
    VkDeviceQueueCreateInfo queueInfo = {};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = queueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
#ifdef __APPLE__
    deviceExtensions.push_back("VK_KHR_portability_subset");
#endif

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueInfo;
    deviceCreateInfo.enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    VkDevice device;
    vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);

    VkQueue queue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);

    run_application(window, instance, physicalDevice, queueFamilyIndex, device, queue);

    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
