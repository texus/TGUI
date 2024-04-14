/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Renderer/Raylib/BackendTextureRaylib.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureRaylib::~BackendTextureRaylib()
    {
        if (m_texture.id)
            UnloadTexture(m_texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureRaylib::loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth)
    {
        const bool reuseTexture = (m_texture.id && (size.x == m_imageSize.x) && (size.y == m_imageSize.y) && (smooth == m_isSmooth));

        BackendTexture::loadTextureOnly(size, pixels, smooth);

        if (reuseTexture)
        {
            UpdateTexture(m_texture, pixels);
        }
        else
        {
            if (m_texture.id)
            {
                UnloadTexture(m_texture);
                m_texture.id = 0;
            }

            static_assert(sizeof(Image) == sizeof(void*) + 4*sizeof(int), "Assuming layout of Image class (c++20 aggregate initialization would solve this)");
            Image image = {
                const_cast<std::uint8_t*>(pixels),
                static_cast<int>(size.x),
                static_cast<int>(size.y),
                1,
                PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
            };

            m_texture = LoadTextureFromImage(image);
            if (m_texture.id)
                SetTextureFilter(m_texture, smooth ? TEXTURE_FILTER_BILINEAR : TEXTURE_FILTER_POINT);
        }

        return (m_texture.id > 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureRaylib::setSmooth(bool smooth)
    {
        if (m_isSmooth == smooth)
            return;

        BackendTexture::setSmooth(smooth);

        if (m_texture.id)
            SetTextureFilter(m_texture, smooth ? TEXTURE_FILTER_BILINEAR : TEXTURE_FILTER_POINT);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture2D& BackendTextureRaylib::getInternalTexture() const
    {
        return m_texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureRaylib::replaceInternalTexture(const Texture2D& texture)
    {
        if (m_texture.id)
        {
            UnloadTexture(m_texture);
            m_texture.id = 0;
        }

        m_texture = texture;
        m_pixels = nullptr;
        m_imageSize = {static_cast<unsigned int>(texture.width), static_cast<unsigned int>(texture.height)};
        m_isSmooth = false; // Raylib has no way to query the TextureFilter, assuming it is TEXTURE_FILTER_POINT
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
