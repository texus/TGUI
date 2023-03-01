/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/extlibs/IncludeSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_Renderer/BackendTextureSDL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static SDL_ScaleMode GetCurrentSDLScaleMode()
    {
        const char *hint = SDL_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);

        if (!hint || SDL_strcasecmp(hint, "nearest") == 0) {
            return SDL_SCALEMODE_NEAREST;
        } else if (SDL_strcasecmp(hint, "linear") == 0) {
            return SDL_SCALEMODE_LINEAR;
        } else if (SDL_strcasecmp(hint, "best") == 0) {
            return SDL_SCALEMODE_BEST;
        } else {
            return static_cast<SDL_ScaleMode>(SDL_atoi(hint));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureSDL::BackendTextureSDL(SDL_Renderer* renderer) :
        m_renderer{renderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureSDL::~BackendTextureSDL()
    {
        if (m_texture)
            SDL_DestroyTexture(m_texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDL::loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth)
    {
        const bool reuseTexture = (m_texture && (size.x == m_imageSize.x) && (size.y == m_imageSize.y) && (smooth == m_isSmooth));

        BackendTexture::loadTextureOnly(size, pixels, smooth);

        if (!reuseTexture)
        {
            if (m_texture)
                SDL_DestroyTexture(m_texture);

            // Change the scale mode if it doesn't match our smoothing parameter
            const SDL_ScaleMode oldScaleMode = GetCurrentSDLScaleMode();
            const SDL_ScaleMode newScaleMode = smooth ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST;
            if (oldScaleMode != newScaleMode)
            {
                if (smooth)
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
                else
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
            }

            m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, static_cast<int>(size.x), static_cast<int>(size.y));

            // Restore the old scale mode if we changed it (since it is a global setting)
            if (oldScaleMode != newScaleMode)
            {
                switch (oldScaleMode)
                {
                case SDL_SCALEMODE_NEAREST:
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
                    break;
                case SDL_SCALEMODE_LINEAR:
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
                    break;
                case SDL_SCALEMODE_BEST:
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
                    break;
                }
            }

            // We need to set the blendmode if we want SDL to render our alpha channel correctly (otherwise all letters will be colored rectangles)
            if (m_texture)
                SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
        }

        // Copy the pixels to the texture
        if (m_texture && pixels)
            SDL_UpdateTexture(m_texture, nullptr, pixels, static_cast<int>(size.x * 4));

        return (m_texture != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSDL::setSmooth(bool smooth)
    {
        if (m_isSmooth == smooth)
            return;

        BackendTexture::setSmooth(smooth);

        if (m_texture)
            SDL_SetTextureScaleMode(m_texture, smooth ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SDL_Texture* BackendTextureSDL::getInternalTexture() const
    {
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSDL::replaceInternalTexture(SDL_Texture* texture)
    {
        if (m_texture)
            SDL_DestroyTexture(m_texture);

        m_texture = texture;
        m_pixels = nullptr;

        int width;
        int height;
        if (SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) == 0)
            m_imageSize = {static_cast<unsigned int>(width), static_cast<unsigned int>(height)};

#if (SDL_MAJOR_VERSION > 2) \
 || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION > 0)) \
 || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION == 0) && (SDL_PATCHLEVEL >= 12))
        SDL_ScaleMode scaleMode;
        if (SDL_GetTextureScaleMode(m_texture, &scaleMode) == 0)
            m_isSmooth = (scaleMode != SDL_SCALEMODE_NEAREST);
#else
        // We have no way of knowing whether the texture was created with smoothing enabled or not.
        // Except for some unrealistic edge cases it doesn't matter what we set the value to though.
        m_isSmooth = (GetCurrentSDLScaleMode() != SDL_SCALEMODE_NEAREST);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
