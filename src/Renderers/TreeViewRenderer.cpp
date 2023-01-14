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


#include <TGUI/Renderers/TreeViewRenderer.hpp>
#include <TGUI/RendererDefines.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_RENDERER_PROPERTY_OUTLINE(TreeViewRenderer, Borders)
    TGUI_RENDERER_PROPERTY_OUTLINE(TreeViewRenderer, Padding)

    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, BackgroundColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedBackgroundColor, Color(0, 110, 255))
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, BackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedBackgroundColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, BorderColor, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, TextColor, Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, TextColorHover, {})
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedTextColor, Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(TreeViewRenderer, SelectedTextColorHover, {})

    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureBackground)
    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureBranchExpanded)
    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureBranchCollapsed)
    TGUI_RENDERER_PROPERTY_TEXTURE(TreeViewRenderer, TextureLeaf)

    TGUI_RENDERER_PROPERTY_RENDERER(TreeViewRenderer, Scrollbar, "Scrollbar")
    TGUI_RENDERER_PROPERTY_NUMBER(TreeViewRenderer, ScrollbarWidth, 0)
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
