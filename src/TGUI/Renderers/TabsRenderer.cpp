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


#include <TGUI/Renderers/TabsRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(TabsRenderer, Borders)

    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BackgroundColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, TextColorDisabled, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, SelectedTextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TabsRenderer, BorderColor, Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureTab)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureTabHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureSelectedTab)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureSelectedTabHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(TabsRenderer, TextureDisabledTab)

    TGUI_RENDERER_PROPERTY_NUMBER(TabsRenderer, DistanceToSide, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
