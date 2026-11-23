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

#include <TGUI/Accessibility/AccessibleWidgetHandler.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleChildWindowHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            const auto& childWindow = static_cast<const ChildWindow&>(widget);
            state.role = AccessibleRole::Window;
            state.name = childWindow.getTitle();
            state.closable = (childWindow.getTitleButtons() & ChildWindow::TitleButton::Close) != 0;
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleChildWindowHandler::performAction(Widget& widget, const String& part, AccessibleAction action) const
        {
            if (!part.empty() || (action != AccessibleAction::Close))
                return false;

            static_cast<ChildWindow&>(widget).close();
            return true;
        }
    } // namespace priv
} // namespace tgui
