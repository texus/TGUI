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


#include <TGUI/SvgImage.hpp>
#include <TGUI/BackendTexture.hpp>

#define NANOSVG_IMPLEMENTATION
#include "TGUI/extlibs/nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "TGUI/extlibs/nanosvg/nanosvgrast.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::SvgImage(const String& filename)
    {
        m_svg = nsvgParseFromFile(filename.toStdString().c_str(), "px", 96);
        if (!m_svg)
        {
            TGUI_PRINT_WARNING("Failed to load svg: " << filename);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::~SvgImage()
    {
        if (m_rasterizer)
            nsvgDeleteRasterizer(m_rasterizer);
        if (m_svg)
            nsvgDelete(m_svg);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SvgImage::isSet() const
    {
        return (m_svg != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f SvgImage::getSize() const
    {
        if (m_svg)
            return {static_cast<float>(m_svg->width), static_cast<float>(m_svg->height)};
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SvgImage::rasterize(BackendTextureBase& texture, Vector2u size)
    {
        if (!m_svg)
            return;

        if (!m_rasterizer)
            m_rasterizer = nsvgCreateRasterizer();

        const float scaleX = size.x / static_cast<float>(m_svg->width);
        const float scaleY = size.y / static_cast<float>(m_svg->height);

        auto pixels = std::make_unique<unsigned char[]>(size.x * size.y * 4);
        nsvgRasterizeFull(m_rasterizer, m_svg, 0, 0, static_cast<double>(scaleX), static_cast<double>(scaleY), pixels.get(), size.x, size.y, size.x * 4);

        texture.load(size, std::move(pixels));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
