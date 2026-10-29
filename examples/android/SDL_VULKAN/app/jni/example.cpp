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

#include <TGUI/TGUI.hpp>

#include <TGUI/Backend/SDL-Vulkan.hpp>

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_vulkan.h>

#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>

// The background image will rotate with the screen
void setBackground(tgui::BackendGui& gui, tgui::Vector2f screenSize)
{
    gui.get("Landscape")->setSize(screenSize.x, screenSize.y);
    gui.get("Portrait")->setSize(screenSize.x, screenSize.y);

    if (screenSize.x > screenSize.y)
    {
        gui.get("Landscape")->setVisible(true);
        gui.get("Portrait")->setVisible(false);
    }
    else
    {
        gui.get("Landscape")->setVisible(false);
        gui.get("Portrait")->setVisible(true);
    }
}

bool runExample(tgui::BackendGui& gui)
{
    auto picLandscape = tgui::Picture::create("Background-Landscape.png");
    picLandscape->setSize({"100%", "100%"});
    gui.add(picLandscape, "Landscape");

    auto picPortrait = tgui::Picture::create("Background-Portrait.png");
    picPortrait->setSize({"100%", "100%"});
    gui.add(picPortrait, "Portrait");

    // Clicking on this edit box will open the keyboard and allow you to type in it
    auto editBox = tgui::EditBox::create();
    editBox->setPosition(50, 150);
    editBox->setSize(400, 40);
    editBox->setDefaultText("Enter text here...");
    gui.add(editBox);

    // Pressing the button will change the text in the edit box
    auto button = tgui::Button::create("Hello");
    button->setPosition(50, 50);
    button->setSize(200, 50);
    button->onPress([=] { editBox->setText("World"); });
    gui.add(button);

    // Set the background and let is change depending on the screen orientation.
    // As long as we don't manually set a view, the gui view will match the window size.
    setBackground(gui, gui.getView().getSize());

    gui.onViewChange([guiPtr = &gui](tgui::FloatRect view) { setBackground(*guiPtr, view.getSize()); });

    return true;
}

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

    gui.mainLoop();
}

// Note that no error checking is performed on SDL and Vulkan initialization in this example code
int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("TGUI window with SDL",
                                          800,
                                          600, // ignored because of SDL_WINDOW_FULLSCREEN flag
                                          SDL_WINDOW_VULKAN | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);

    // Create the instance with the extensions that SDL needs to create a surface for the window
    Uint32 sdlExtensionCount = 0;
    const char* const* sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = sdlExtensionCount;
    instanceInfo.ppEnabledExtensionNames = sdlExtensions;

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

    const char* swapchainExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueInfo;
    deviceCreateInfo.enabledExtensionCount = 1;
    deviceCreateInfo.ppEnabledExtensionNames = &swapchainExtension;

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
