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
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/SeparatorLine.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/TabsBase.hpp>
#include <TGUI/Widgets/TextArea.hpp>
#include <TGUI/Widgets/ToggleButton.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        const AccessibleWidgetHandler& AccessibleWidgetHandler::getHandler(const Widget& widget)
        {
            static const AccessibleWidgetHandler widgetHandler;
            static const AccessibleContainerHandler containerHandler;
            static const AccessibleRootContainerHandler rootContainerHandler;
            static const AccessibleChildWindowHandler childWindowHandler;
            static const AccessibleLabelHandler labelHandler;
            static const AccessibleButtonHandler buttonHandler;
            static const AccessibleEditBoxHandler editBoxHandler;
            static const AccessibleTextAreaHandler textAreaHandler;
            static const AccessibleToggleButtonHandler toggleButtonHandler;
            static const AccessibleCheckBoxHandler checkBoxHandler;
            static const AccessibleRadioButtonHandler radioButtonHandler;
            static const AccessibleRangeHandler rangeHandler;
            static const AccessiblePictureHandler pictureHandler;
            static const AccessibleSeparatorLineHandler separatorLineHandler;
            static const AccessibleTabsHandler tabsHandler;

            // Derived classes have to be checked before their base classes (e.g. CheckBox inherits from RadioButton)
            if (dynamic_cast<const Button*>(&widget))
                return buttonHandler;
            if (dynamic_cast<const ToggleButton*>(&widget))
                return toggleButtonHandler;
            if (dynamic_cast<const CheckBox*>(&widget))
                return checkBoxHandler;
            if (dynamic_cast<const RadioButton*>(&widget))
                return radioButtonHandler;
            if (dynamic_cast<const Slider*>(&widget) || dynamic_cast<const Knob*>(&widget) || dynamic_cast<const SpinButton*>(&widget)
                || dynamic_cast<const ProgressBar*>(&widget) || dynamic_cast<const Scrollbar*>(&widget))
                return rangeHandler;
            if (dynamic_cast<const Picture*>(&widget))
                return pictureHandler;
            if (dynamic_cast<const SeparatorLine*>(&widget))
                return separatorLineHandler;
            if (dynamic_cast<const TabsBase*>(&widget))
                return tabsHandler;
            if (dynamic_cast<const EditBox*>(&widget))
                return editBoxHandler;
            if (dynamic_cast<const TextArea*>(&widget))
                return textAreaHandler;
            if (dynamic_cast<const Label*>(&widget))
                return labelHandler;
            if (dynamic_cast<const ChildWindow*>(&widget))
                return childWindowHandler;
            if (dynamic_cast<const RootContainer*>(&widget))
                return rootContainerHandler;
            if (widget.isContainer())
                return containerHandler;

            return widgetHandler;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleWidgetHandler::getState(const Widget&, const String& part, AccessibleState& state) const
        {
            // Widgets don't have any parts unless a derived handler adds them
            if (!part.empty())
                return false;

            state.role = AccessibleRole::Custom;
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<String> AccessibleWidgetHandler::getChildParts(const Widget&, const String&) const
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleWidgetHandler::getPartBounds(const Widget& widget, const String&) const
        {
            return {{}, widget.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleWidgetHandler::getFocusedPart(const Widget&) const
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleWidgetHandler::performAction(Widget&, const String&, AccessibleAction) const
        {
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleWidgetHandler::setValue(Widget&, const String&, const String&) const
        {
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleWidgetHandler::setRangeValue(Widget&, const String&, double) const
        {
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::pair<std::size_t, std::size_t> AccessibleWidgetHandler::getTextSelection(const Widget&) const
        {
            return {0, 0};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleWidgetHandler::setTextSelection(Widget&, std::size_t, std::size_t) const
        {
            return false;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::pair<std::size_t, std::size_t>> AccessibleWidgetHandler::getTextLines(const Widget&) const
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleWidgetHandler::getCharacterBounds(const Widget&, std::size_t) const
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t AccessibleWidgetHandler::getTextIndexAtPosition(const Widget&, Vector2f) const
        {
            return 0;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleWidgetHandler::getTextArea(const Widget& widget) const
        {
            return {{}, widget.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleWidgetHandler::scrollTextToIndex(Widget&, std::size_t) const
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleTextAttributes AccessibleWidgetHandler::getTextAttributes(const Widget&) const
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleWidgetHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            connectSignal(widget.onFocus, widget, tree, AccessibleEvent::FocusChanged);
            connectSignal(widget.onUnfocus, widget, tree, AccessibleEvent::FocusChanged);
            connectSignal(widget.onVisibilityChange, widget, tree, AccessibleEvent::PropertyChanged, AccessibleProperty::Offscreen);
            connectSignal(widget.onEnabledChange, widget, tree, AccessibleEvent::PropertyChanged, AccessibleProperty::Enabled);
            connectSignal(widget.onAccessibleTextChange, widget, tree, AccessibleEvent::PropertyChanged, AccessibleProperty::Name);
            connectSignal(widget.onChildrenChange, widget, tree, AccessibleEvent::ChildrenChanged);

            // The role, name and description are all changed via the same signal
            widget.onAccessibilityChange.setAccessibilityHandler(
                [&tree, &widget]
                {
                    tree.queueEvent(widget, AccessibleEvent::PropertyChanged, AccessibleProperty::Name, {});
                    tree.queueEvent(widget, AccessibleEvent::PropertyChanged, AccessibleProperty::Description, {});
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleWidgetHandler::disconnectSignals(Widget& widget) const
        {
            disconnectSignal(widget.onFocus);
            disconnectSignal(widget.onUnfocus);
            disconnectSignal(widget.onVisibilityChange);
            disconnectSignal(widget.onEnabledChange);
            disconnectSignal(widget.onAccessibleTextChange);
            disconnectSignal(widget.onChildrenChange);
            disconnectSignal(widget.onAccessibilityChange);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleWidgetHandler::connectSignal(Signal& signal,
                                                    Widget& widget,
                                                    AccessibilityTree& tree,
                                                    AccessibleEvent event,
                                                    AccessibleProperty property)
        {
            // The signal is a member of the widget, so the widget always exists when the handler is called.
            // The tree removes the handlers before it is destroyed.
            signal.setAccessibilityHandler([&tree, &widget, event, property] { tree.queueEvent(widget, event, property, {}); });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleWidgetHandler::disconnectSignal(Signal& signal)
        {
            signal.setAccessibilityHandler(nullptr);
        }
    } // namespace priv
} // namespace tgui
