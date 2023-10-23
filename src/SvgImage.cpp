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


#include <TGUI/Global.hpp>
#include <TGUI/SvgImage.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <TGUI/Backend/Renderer/BackendTexture.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/extlibs/IncludeNanoSVG.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        struct SvgImageData
        {
            NSVGimage* svg = nullptr;
            NSVGrasterizer* rasterizer = nullptr;
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::SvgImage() :
        m_data{std::make_unique<priv::SvgImageData>()}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::SvgImage(const String& filename) :
        m_data{std::make_unique<priv::SvgImageData>()}
    {
        m_data->svg = nsvgParseFromFile(filename.toStdString().c_str(), "px", 96);
        if (!m_data->svg)
        {
            TGUI_PRINT_WARNING("Failed to load svg: " << filename);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SvgImage::~SvgImage()
    {
        if (m_data->rasterizer)
            nsvgDeleteRasterizer(m_data->rasterizer);
        if (m_data->svg)
            nsvgDelete(m_data->svg);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SvgImage::isSet() const
    {
        return (m_data->svg != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f SvgImage::getSize() const
    {
        if (m_data->svg)
            return {static_cast<float>(m_data->svg->width), static_cast<float>(m_data->svg->height)};
        else
            return {0, 0};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SvgImage::rasterize(BackendTexture& texture, Vector2u size)
    {
        if (!m_data->svg)
            return;

        if (!m_data->rasterizer)
            m_data->rasterizer = nsvgCreateRasterizer();

        const float fontScale = getBackend()->getFontScale();
        size.x = static_cast<unsigned int>(size.x * fontScale);
        size.y = static_cast<unsigned int>(size.y * fontScale);

        const float scaleX = size.x / static_cast<float>(m_data->svg->width);
        const float scaleY = size.y / static_cast<float>(m_data->svg->height);

        auto pixels = MakeUniqueForOverwrite<unsigned char[]>(size.x * size.y * 4);
        nsvgRasterizeXY(m_data->rasterizer, m_data->svg, 0, 0, scaleX, scaleY,
                        pixels.get(), static_cast<int>(size.x), static_cast<int>(size.y), static_cast<int>(size.x * 4));

        texture.load(size, std::move(pixels), true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
