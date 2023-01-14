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


#include <TGUI/Renderers/ComboBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ComboBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ComboBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, DefaultTextColor, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowBackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, ArrowColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(ComboBoxRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureBackgroundDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrow)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(ComboBoxRenderer, TextureArrowDisabled)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ComboBoxRenderer, TextStyle, TextStyle::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(ComboBoxRenderer, DefaultTextStyle, {})

    TGUI_RENDERER_PROPERTY_RENDERER(ComboBoxRenderer, ListBox, "ListBox")
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
