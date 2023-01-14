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


#include <TGUI/Renderers/ListViewRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(ListViewRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(ListViewRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, SelectedTextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, HeaderBackgroundColor, Color(230, 230, 230))
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, HeaderTextColor, {})
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, BorderColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, SeparatorColor, Color(200, 200, 200))
    TGUI_RENDERER_PROPERTY_COLOR(ListViewRenderer, GridLinesColor, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(ListViewRenderer, TextureHeaderBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(ListViewRenderer, TextureBackground)

    TGUI_RENDERER_PROPERTY_RENDERER(ListViewRenderer, Scrollbar, "Scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(ListViewRenderer, ScrollbarWidth, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
