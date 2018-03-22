/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Renderers/ButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, TextColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BackgroundColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorDown, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ButtonRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, Texture)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDown)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(ButtonRenderer, TextureFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleHover, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleDown, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleDisabled, {})
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ButtonRenderer, TextStyleFocused, {})
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
