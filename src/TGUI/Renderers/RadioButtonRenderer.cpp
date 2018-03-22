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


#include <TGUI/Renderers/RadioButtonRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(RadioButtonRenderer, Borders)

    TGUI_RENDERER_PROPERTY_NUMBER(RadioButtonRenderer, TextDistanceRatio, 0.2f)

    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorChecked, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorCheckedHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, TextColorCheckedDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorChecked, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorCheckedHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BackgroundColorCheckedDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorChecked, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorCheckedHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorCheckedDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, BorderColorCheckedFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, CheckColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, CheckColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(RadioButtonRenderer, CheckColorDisabled, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUnchecked)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureUncheckedFocused)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureChecked)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(RadioButtonRenderer, TextureCheckedFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(RadioButtonRenderer, TextStyle, sf::Text::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(RadioButtonRenderer, TextStyleChecked, {})
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
