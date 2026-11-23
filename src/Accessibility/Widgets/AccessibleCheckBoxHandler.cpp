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
#include <TGUI/Accessibility/AccessibilityTree.hpp>
#include <TGUI/Widgets/CheckBox.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleCheckBoxHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            const auto& checkBox = static_cast<const CheckBox&>(widget);
            state.role = AccessibleRole::CheckBox;
            state.name = checkBox.getText();
            state.toggleState = checkBox.isChecked() ? AccessibleToggleState::On : AccessibleToggleState::Off;
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleCheckBoxHandler::performAction(Widget& widget, const String& part, AccessibleAction action) const
        {
            if (!part.empty() || (action != AccessibleAction::Toggle))
                return false;

            auto& checkBox = static_cast<CheckBox&>(widget);
            checkBox.setChecked(!checkBox.isChecked());
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleCheckBoxHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            auto& checkBox = static_cast<CheckBox&>(widget);
            connectSignal(checkBox.onChange, checkBox, tree, AccessibleEvent::PropertyChanged, AccessibleProperty::ToggleState);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleCheckBoxHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);
            disconnectSignal(static_cast<CheckBox&>(widget).onChange);
        }
    } // namespace priv
} // namespace tgui
