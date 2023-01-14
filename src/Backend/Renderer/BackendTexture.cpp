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


#include <TGUI/Backend/Renderer/BackendTexture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTexture::load(Vector2u size, std::unique_ptr<std::uint8_t[]> pixels, bool smooth)
    {
        TGUI_ASSERT(pixels, "load in BackendTexture doesn't accept a nullptr for pixels, use loadTextureOnly instead");

        if (!loadTextureOnly(size, pixels.get(), smooth))
            return false;

        m_pixels = std::move(pixels);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTexture::loadTextureOnly(Vector2u size, const std::uint8_t*, bool smooth)
    {
        TGUI_ASSERT((size.x > 0) && (size.y > 0), "load and loadTextureOnly functions in BackendTexture needs a valid size");

        m_pixels = nullptr;
        m_imageSize = size;
        m_isSmooth = smooth;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u BackendTexture::getSize() const
    {
        return m_imageSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTexture::setSmooth(bool smooth)
    {
        m_isSmooth = smooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTexture::isSmooth() const
    {
        return m_isSmooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTexture::isTransparentPixel(Vector2u pixel) const
    {
        if (!m_pixels)
            return false;

        TGUI_ASSERT((pixel.x < m_imageSize.x) && (pixel.y < m_imageSize.y), "Pixel out of range in BackendTexture::isTransparentPixel");

        const auto pixelOffset = (pixel.y * m_imageSize.x + pixel.x) * 4;
        return (m_pixels[pixelOffset + 3] == 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::uint8_t* BackendTexture::getPixels() const
    {
        return m_pixels.get();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
