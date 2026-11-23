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
#include <TGUI/Widgets/ToggleButton.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleToggleButtonHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            const auto& button = static_cast<const ToggleButton&>(widget);
            state.role = AccessibleRole::ToggleButton;
            state.name = button.getText();
            state.toggleState = button.isDown() ? AccessibleToggleState::On : AccessibleToggleState::Off;
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleToggleButtonHandler::performAction(Widget& widget, const String& part, AccessibleAction action) const
        {
            if (!part.empty() || (action != AccessibleAction::Toggle))
                return false;

            auto& button = static_cast<ToggleButton&>(widget);
            button.setDown(!button.isDown());
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleToggleButtonHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            auto& button = static_cast<ToggleButton&>(widget);
            connectSignal(button.onToggle, button, tree, AccessibleEvent::PropertyChanged, AccessibleProperty::ToggleState);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleToggleButtonHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);
            disconnectSignal(static_cast<ToggleButton&>(widget).onToggle);
        }
    } // namespace priv
} // namespace tgui
