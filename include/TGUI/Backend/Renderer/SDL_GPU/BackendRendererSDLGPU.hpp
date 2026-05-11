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

#ifndef TGUI_BACKEND_RENDERER_SDL_GPU_HPP
#define TGUI_BACKEND_RENDERER_SDL_GPU_HPP

#include <TGUI/Backend/Renderer/BackendRenderer.hpp>
#include <TGUI/Backend/Renderer/SDL_GPU/BackendRenderTargetSDLGPU.hpp>
#include <TGUI/Backend/Renderer/SDL_GPU/BackendTextureSDLGPU.hpp>
#include <TGUI/Backend/Renderer/SDL_GPU/CanvasSDLGPU.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SDL_Renderer;

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Backend renderer that uses SDL3's GPU API
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRendererSDLGPU : public BackendRenderer
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Initializes the backend renderer
        ///
        /// @param device  Device used for rendering
        /// @param swapchainTextureFormat  Texture format of the swapchain
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendRendererSDLGPU(SDL_GPUDevice* device, SDL_GPUTextureFormat swapchainTextureFormat);

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
        /// @brief Returns the texture format of the swapchain
        ///
        /// @return Texture format of the swapchain that was passed to the constructor of this object
        ///
        /// @since TGUI 1.13
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] SDL_GPUTextureFormat getSwapchainTextureFormat() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a pointer to the device object that was used to create this backend renderer
        ///
        /// @return SDL_GPUDevice pointer that was passed to the constructor of this object
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] SDL_GPUDevice* getInternalDevice() const;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        SDL_GPUDevice* m_device;
        SDL_GPUTextureFormat m_swapchainTextureFormat;
    };
} // namespace tgui

#endif // TGUI_BACKEND_RENDERER_SDL_GPU_HPP
