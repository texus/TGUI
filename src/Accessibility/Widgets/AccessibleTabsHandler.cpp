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
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TabsBase.hpp>

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    // Returns the indices of the tabs that are visible, hidden tabs aren't exposed to assistive technology
    [[nodiscard]] std::vector<std::size_t> getVisibleTabs(const tgui::TabsBase& tabs)
    {
        std::vector<std::size_t> visibleTabs;
        for (std::size_t i = 0; i < tabs.getTabsCount(); ++i)
        {
            if (tabs.getTabVisible(i))
                visibleTabs.push_back(i);
        }
        return visibleTabs;
    }

    // Converts the key of a part back into the index of the tab, returns -1 if the part isn't a visible tab
    [[nodiscard]] int getTabIndex(const tgui::TabsBase& tabs, const tgui::String& part)
    {
        if (part.empty())
            return -1;

        const int index = part.toInt(-1);
        if ((index < 0) || (static_cast<std::size_t>(index) >= tabs.getTabsCount()) || !tabs.getTabVisible(static_cast<std::size_t>(index)))
            return -1;

        return index;
    }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleTabsHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            const auto& tabs = static_cast<const TabsBase&>(widget);
            if (part.empty())
            {
                state.role = AccessibleRole::TabList;
                state.selectionContainer = true;
                state.orientation = dynamic_cast<const Tabs*>(&tabs) ? Orientation::Horizontal : Orientation::Vertical;
                return true;
            }

            const int index = getTabIndex(tabs, part);
            if (index < 0)
                return false;

            const auto visibleTabs = getVisibleTabs(tabs);
            const auto it = std::find(visibleTabs.begin(), visibleTabs.end(), static_cast<std::size_t>(index));
            state.role = AccessibleRole::TabItem;
            state.name = tabs.getText(static_cast<std::size_t>(index));
            state.focusable = true;
            state.enabled = tabs.isEnabled() && tabs.getTabEnabled(static_cast<std::size_t>(index));
            state.selectable = true;
            state.selected = (tabs.getSelectedIndex() == index);
            state.positionInSet = static_cast<int>(it - visibleTabs.begin()) + 1;
            state.sizeOfSet = static_cast<int>(visibleTabs.size());
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<String> AccessibleTabsHandler::getChildParts(const Widget& widget, const String& part) const
        {
            if (!part.empty())
                return {};

            std::vector<String> parts;
            for (const std::size_t index : getVisibleTabs(static_cast<const TabsBase&>(widget)))
                parts.push_back(String::fromNumber(index));

            return parts;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleTabsHandler::getPartBounds(const Widget& widget, const String& part) const
        {
            // The size of each tab isn't available, so the tabs are assumed to divide the widget equally
            const auto& tabs = static_cast<const TabsBase&>(widget);
            const int index = getTabIndex(tabs, part);
            const auto visibleTabs = getVisibleTabs(tabs);
            const auto it = std::find(visibleTabs.begin(), visibleTabs.end(), static_cast<std::size_t>(index));
            if ((index < 0) || (it == visibleTabs.end()))
                return AccessibleWidgetHandler::getPartBounds(widget, part);

            const float position = static_cast<float>(it - visibleTabs.begin());
            const Vector2f size = tabs.getSize();
            const float count = static_cast<float>(visibleTabs.size());
            if (dynamic_cast<const Tabs*>(&tabs))
                return {position * size.x / count, 0, size.x / count, size.y};
            else
                return {0, position * size.y / count, size.x, size.y / count};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleTabsHandler::getFocusedPart(const Widget& widget) const
        {
            // The arrow keys change the selected tab, so the focus follows the selection
            const int selectedIndex = static_cast<const TabsBase&>(widget).getSelectedIndex();
            return (selectedIndex >= 0) ? String::fromNumber(selectedIndex) : String{};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleTabsHandler::performAction(Widget& widget, const String& part, AccessibleAction action) const
        {
            auto& tabs = static_cast<TabsBase&>(widget);
            const int index = getTabIndex(tabs, part);
            if ((index < 0) || ((action != AccessibleAction::Select) && (action != AccessibleAction::AddToSelection)))
                return false;

            return tabs.select(static_cast<std::size_t>(index));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleTabsHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            auto& tabs = static_cast<TabsBase&>(widget);
            tabs.onTabSelect.setAccessibilityHandler(
                [&tree, &tabs]
                {
                    const int selectedIndex = tabs.getSelectedIndex();
                    if (selectedIndex >= 0)
                    {
                        const String part = String::fromNumber(selectedIndex);
                        tree.queueEvent(tabs, AccessibleEvent::PropertyChanged, AccessibleProperty::Selected, part);
                        tree.queueEvent(tabs, AccessibleEvent::ElementSelected, {}, part);
                    }

                    tree.queueEvent(tabs, AccessibleEvent::FocusChanged, {}, {});
                });
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleTabsHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);
            disconnectSignal(static_cast<TabsBase&>(widget).onTabSelect);
        }
    } // namespace priv
} // namespace tgui
