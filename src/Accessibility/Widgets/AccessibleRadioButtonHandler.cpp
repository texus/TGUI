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
#include <TGUI/Container.hpp>
#include <TGUI/Widgets/RadioButton.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleRadioButtonHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            const auto& radioButton = static_cast<const RadioButton&>(widget);
            state.role = AccessibleRole::RadioButton;
            state.name = radioButton.getText();
            state.selectable = true;
            state.selected = radioButton.isChecked();

            // Only one radio button inside the same parent can be checked, so the visible radio buttons in the parent form
            // the group. Check boxes inherit from RadioButton, so the widget type is compared to leave them out.
            if (const Container* parent = radioButton.getParent())
            {
                for (const auto& sibling : parent->getWidgets())
                {
                    if (!sibling->isVisible() || (sibling->getWidgetType() != radioButton.getWidgetType()))
                        continue;

                    ++state.sizeOfSet;
                    if (sibling.get() == &radioButton)
                        state.positionInSet = state.sizeOfSet;
                }
            }

            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleRadioButtonHandler::performAction(Widget& widget, const String& part, AccessibleAction action) const
        {
            if (!part.empty() || ((action != AccessibleAction::Select) && (action != AccessibleAction::AddToSelection)))
                return false;

            static_cast<RadioButton&>(widget).setChecked(true);
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleRadioButtonHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            auto& radioButton = static_cast<RadioButton&>(widget);
            radioButton.onChange.setAccessibilityHandler(
                [&tree, &radioButton]
                {
                    tree.queueEvent(radioButton, AccessibleEvent::PropertyChanged, AccessibleProperty::Selected, {});
                    if (radioButton.isChecked())
                        tree.queueEvent(radioButton, AccessibleEvent::ElementSelected, {}, {});
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleRadioButtonHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);
            disconnectSignal(static_cast<RadioButton&>(widget).onChange);
        }
    } // namespace priv
} // namespace tgui
