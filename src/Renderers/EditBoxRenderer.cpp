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


#include <TGUI/Renderers/EditBoxRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Padding)

    TGUI_RENDERER_PROPERTY_NUMBER(EditBoxRenderer, CaretWidth, 1)

    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, DefaultTextColor, Color(160, 160, 160))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColorFocused, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColorFocused, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, Texture)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureDisabled)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureFocused)

    TGUI_RENDERER_PROPERTY_TEXT_STYLE(EditBoxRenderer, TextStyle, TextStyle::Regular)
    TGUI_RENDERER_PROPERTY_TEXT_STYLE(EditBoxRenderer, DefaultTextStyle, TextStyle::Italic)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
