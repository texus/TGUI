/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/BackendTexture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    bool BackendTextureBase::load(Vector2u size, std::unique_ptr<std::uint8_t[]> pixels)
    {
        m_pixels = std::move(pixels);
        m_imageSize = size;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u BackendTextureBase::getSize() const
    {
        return m_imageSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureBase::setSmooth(bool smooth)
    {
        m_isSmooth = smooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureBase::isSmooth() const
    {
        return m_isSmooth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureBase::isTransparentPixel(Vector2u pixel) const
    {
        if (!m_pixels)
            return false;

        TGUI_ASSERT((pixel.x < m_imageSize.x) && (pixel.y < m_imageSize.y), "Pixel out of range in BackendTextureBase::isTransparentPixel");

        const auto pixelOffset = (pixel.y * m_imageSize.x + pixel.x) * 4;
        return (m_pixels[pixelOffset + 3] == 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
