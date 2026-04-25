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

#ifndef TGUI_FORM_LOAD_OPTIONS_HPP
#define TGUI_FORM_LOAD_OPTIONS_HPP

#include <TGUI/Loading/Theme.hpp>
#include <TGUI/String.hpp>

#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Options for loadWidgetsFromFile / loadWidgetsFromStream
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct TGUI_API FormLoadOptions
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Remove existing widgets first if there are any
        ///
        /// If true (default), removeAllWidgets() will be called at the start of the load function.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool replaceExistingWidgets = true;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Runtime themes keyed by alias name used in the form file
        ///
        /// When a widget line sets Renderer to \@Alias or \@Alias.Section, the renderer is taken from
        /// themesByAlias[Alias] (Section defaults to the widget type if omitted). If the alias is missing from this map,
        /// loading falls back to a matching Theme.Alias block in the same form file when present.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::map<String, Theme::Ptr> themesByAlias;
    };
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_FORM_LOAD_OPTIONS_HPP
