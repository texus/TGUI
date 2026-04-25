////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_WIDGET_LOAD_RESOURCES_HPP
#define TGUI_WIDGET_LOAD_RESOURCES_HPP

#include <TGUI/Renderers/WidgetRenderer.hpp>
#include <TGUI/String.hpp>

#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Theme;

    /// Fallback renderers from Theme.\<Alias\> sections in the form file
    using ThemeFallbackMap = std::map<String, std::map<String, std::shared_ptr<RendererData>>>;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @internal
    /// @brief Resources used when loading widgets from a form file
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API WidgetLoadResources
    {
        std::map<String, std::shared_ptr<RendererData>> renderers;
        const std::map<String, std::shared_ptr<Theme>>* runtimeThemesByAlias = nullptr;
        const ThemeFallbackMap* themeFallbacks = nullptr;
    };
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_WIDGET_LOAD_RESOURCES_HPP
