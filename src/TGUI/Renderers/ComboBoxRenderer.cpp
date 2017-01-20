/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Renderers/ComboBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ComboBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ComboBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BackgroundColor, sf::Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, TextColor, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColor, sf::Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColor, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BorderColor, sf::Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowUp)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowUpHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowDownHover)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ComboBoxRenderer, TextStyle, sf::Text::Regular)

    TGUI_RENDERER_PROPERTY_RENDERER(ComboBoxRenderer, ListBox)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
