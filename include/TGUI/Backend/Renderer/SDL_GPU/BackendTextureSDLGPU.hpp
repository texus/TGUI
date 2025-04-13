/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_BACKEND_TEXTURE_SDL_GPU_HPP
#define TGUI_BACKEND_TEXTURE_SDL_GPU_HPP

#include <TGUI/Config.hpp>
#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Renderer/BackendTexture.hpp>
    #include <TGUI/extlibs/IncludeSDL.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Texture implementation that uses SDL3's GPU API
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendTextureSDLGPU : public BackendTexture
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param device      The device to use when creating textures
        /// @param usageFlags  Texture usage flags (SDL_GPU_TEXTUREUSAGE_SAMPLER is needed to use in a pixel shader,
        ///                    SDL_GPU_TEXTUREUSAGE_COLOR_TARGET would be used if you want to use it as a render target).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendTextureSDLGPU(SDL_GPUDevice* device, std::uint32_t usageFlags = SDL_GPU_TEXTUREUSAGE_SAMPLER);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendTextureSDLGPU() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from an array of 32-bits RGBA pixels, but don't take ownership of the pixels
        ///
        /// @param size   Width and height of the image to create
        /// @param pixels Pointer to array of size.x*size.y*4 bytes with RGBA pixels, or nullptr to create an empty texture
        /// @param smooth Should the smooth filter be enabled or not?
        ///
        /// @warning Unlike the load function, loadTextureOnly won't store the pixels and isTransparentPixel thus won't work
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns a pointer to the internal SDL_GPUTexture
        /// @return Pointer to internal texture
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD SDL_GPUTexture* getInternalTexture() const
        {
            return m_texture;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Replaces the internal texture by a different one
        ///
        /// @param texture    Texture to copy
        /// @param imageSize  Dimensions of the new texture
        ///
        /// This will remove the pixel data stored by this object and reset its size based on the given imageSize parameter.
        ///
        /// @warning Ownership of the texture is transferred by calling this function. The SDL_ReleaseGPUTexture function will be
        ///          called when this object is destroyed and it should NOT be called by the caller of replaceInternalTexture.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void replaceInternalTexture(SDL_GPUTexture* texture, Vector2u imageSize);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        SDL_GPUDevice* m_device;
        SDL_GPUTexture* m_texture = nullptr;
        std::uint32_t m_usageFlags;
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_TEXTURE_SDL_GPU_HPP
