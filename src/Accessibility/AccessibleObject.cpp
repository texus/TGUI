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

#include <TGUI/Accessibility/AccessibleObject.hpp>
#include <TGUI/Accessibility/AccessibilityTree.hpp>
#include <TGUI/Accessibility/AccessibleWidgetHandler.hpp>
#include <TGUI/Backend/Window/BackendGui.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/SubwidgetContainer.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>

#include <algorithm>
#include <cmath>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::AccessibleObject(AccessibilityTree& tree, const Widget::Ptr& widget, String part) :
            m_tree{tree},
            m_widget{widget},
            m_part{std::move(part)}
        {
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr AccessibleObject::getWidget() const
        {
            return m_widget.lock();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::isAlive() const
        {
            const auto widget = getWidget();
            if (!widget)
                return false;

            // A widget that was removed from the gui no longer belongs to the tree, even when the application keeps it alive
            const Widget* rootContainer = m_tree.getGui().getContainer().get();
            const Widget* ancestor = widget.get();
            while (ancestor && (ancestor != rootContainer))
                ancestor = ancestor->getParent();
            if (!ancestor)
                return false;

            // Parts (e.g. list items) can be removed while the widget still exists
            if (m_part.empty())
                return true;

            AccessibleState state;
            return AccessibleWidgetHandler::getHandler(*widget).getState(*widget, m_part, state);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleState AccessibleObject::getState() const
        {
            AccessibleState state;
            if (const auto widget = getWidget())
                AccessibleWidgetHandler::getHandler(*widget).getState(*widget, m_part, state);

            return state;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleRole AccessibleObject::getRole() const
        {
            // A role that was explicitly set on the widget has priority over the role that is based on the widget type
            const auto widget = getWidget();
            if (widget && m_part.empty() && (widget->getAccessibleRole() != AccessibleRole::Default))
                return widget->getAccessibleRole();

            return getState().role;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleObject::getName() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            // The accessible name that was set on the widget has priority over e.g. the text on a button
            if (m_part.empty() && !widget->getAccessibleName().empty())
                return widget->getAccessibleName();

            const String name = getState().name;
            if (!name.empty() || !m_part.empty())
                return name;

            // Widgets that don't display a name of their own are named by the label in front of them, or by their widget name
            if (const auto label = getLabel())
                return label->getName();
            if (!widget->getWidgetName().starts_with(U"#TGUI_INTERNAL$"))
                return widget->getWidgetName();

            return name;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getLabel() const
        {
            const auto widget = getWidget();
            if (!widget || !m_part.empty() || !widget->getAccessibleName().empty() || !getState().name.empty())
                return nullptr;

            const Container* parent = widget->getParent();
            if (!parent)
                return nullptr;

            // Only the visible widget directly in front of this widget can be its label
            const auto& siblings = parent->getWidgets();
            const auto it = std::find(siblings.begin(), siblings.end(), widget);
            for (auto sibling = std::make_reverse_iterator(it); sibling != siblings.rend(); ++sibling)
            {
                if (!(*sibling)->isVisible())
                    continue;

                const auto label = std::dynamic_pointer_cast<Label>(*sibling);
                if (!label || label->getText().empty())
                    return nullptr;

                return m_tree.findWidgetObject(label);
            }

            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleObject::getDescription() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            if (m_part.empty() && !widget->getAccessibleDescription().empty())
                return widget->getAccessibleDescription();

            return getState().description;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleObject::getHelpText() const
        {
            const auto widget = getWidget();
            if (!widget || !m_part.empty())
                return {};

            // The help text is the text of the tool tip, if the tool tip is a label
            const auto label = std::dynamic_pointer_cast<Label>(widget->getToolTip());
            if (!label)
                return {};

            return label->getText();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleObject::getAutomationId() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            if (m_part.empty() || widget->getWidgetName().empty())
                return widget->getWidgetName();

            return widget->getWidgetName() + U"." + m_part;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleObject::getClassName() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            return widget->getWidgetType();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleObject::getBounds() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            const Vector2f widgetPosition = getAbsoluteWidgetPosition();
            if (m_part.empty())
                return {widgetPosition + widget->getWidgetOffset(), widget->getFullSize()};

            const FloatRect partBounds = AccessibleWidgetHandler::getHandler(*widget).getPartBounds(*widget, m_part);
            return {widgetPosition + partBounds.getPosition(), partBounds.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getParent() const
        {
            return m_parent.lock();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<AccessibleObject::Ptr> AccessibleObject::getChildren()
        {
            std::vector<Ptr> children;
            const auto widget = getWidget();
            if (!widget)
                return children;

            // The widgets inside a container are the first children of the container
            if (m_part.empty())
            {
                if (widget->isContainer())
                    addWidgetChildren(static_cast<const Container&>(*widget), children);
                else if (const auto subwidgetContainer = std::dynamic_pointer_cast<SubwidgetContainer>(widget))
                    addWidgetChildren(*subwidgetContainer->getContainer(), children);
            }

            const auto self = shared_from_this();
            for (const auto& childPart : AccessibleWidgetHandler::getHandler(*widget).getChildParts(*widget, m_part))
                children.push_back(m_tree.getPartObject(widget, childPart, self));

            return children;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getObjectAtPosition(Vector2f pos)
        {
            if (!isAlive() || isOffscreen() || !getBounds().contains(pos))
                return nullptr;

            // Children are ordered from back to front, so the last child that contains the point is the one on top
            const auto children = getChildren();
            for (auto it = children.rbegin(); it != children.rend(); ++it)
            {
                if (auto object = (*it)->getObjectAtPosition(pos))
                    return object;
            }

            return shared_from_this();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::isEnabled() const
        {
            const auto widget = getWidget();
            if (!widget)
                return false;

            // A widget inside a disabled container doesn't receive any events, so it is disabled as well
            if (m_part.empty() ? !widget->isEnabled() : !getState().enabled)
                return false;

            const auto parent = getParent();
            return !parent || parent->isEnabled();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::isOffscreen() const
        {
            const auto widget = getWidget();
            if (!widget)
                return true;

            if (m_part.empty() ? !widget->isVisible() : getState().offscreen)
                return true;

            const auto parent = getParent();
            return parent && parent->isOffscreen();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::isFocusable() const
        {
            const auto widget = getWidget();
            if (!widget)
                return false;

            if (!m_part.empty())
                return getState().focusable;

            // Widgets that only display something can technically be focused, but users can never give them focus
            const AccessibleRole role = getRole();
            if ((role == AccessibleRole::Text) || (role == AccessibleRole::Image) || (role == AccessibleRole::Separator)
                || (role == AccessibleRole::ProgressBar))
            {
                return false;
            }

            return widget->isFocusable() && widget->canGainFocus();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::setFocus()
        {
            const auto widget = getWidget();
            if (!widget || !isFocusable())
                return false;

            // Parts receive focus by selecting them, as the focus of the widget always follows its selection
            if (!m_part.empty())
            {
                const auto parent = getParent();
                if (parent && !parent->setFocus())
                    return false;

                return AccessibleWidgetHandler::getHandler(*widget).performAction(*widget, m_part, AccessibleAction::Select);
            }

            // Make certain that the parents are focused as well, setFocused only focuses the widget within its parent
            const auto parent = getParent();
            if (parent && parent->getWidget() && !parent->getWidget()->isFocused())
                parent->setFocus();

            widget->setFocused(true);
            return widget->isFocused();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::performAction(AccessibleAction action)
        {
            const auto widget = getWidget();
            if (!widget)
                return false;

            // Disabled widgets can't be used, but they can still be scrolled into view to show them to the user
            if ((action != AccessibleAction::ScrollIntoView) && !isEnabled())
                return false;

            // Scrolling a widget into view is handled here, as it is the same for every widget inside a scrollable panel
            if ((action == AccessibleAction::ScrollIntoView) && m_part.empty())
            {
                bool scrollablePanelFound = false;
                const Widget* child = widget.get();
                for (Container* parent = widget->getParent(); parent != nullptr; child = parent, parent = parent->getParent())
                {
                    auto* panel = dynamic_cast<ScrollablePanel*>(parent);
                    if (!panel)
                        continue;

                    scrollablePanelFound = true;
                    const Vector2f contentOffset = panel->getContentOffset();
                    Vector2f visibleSize = panel->getInnerSize();
                    if (panel->getVerticalScrollbar()->isShown())
                        visibleSize.x -= panel->getVerticalScrollbar()->getWidth();
                    if (panel->getHorizontalScrollbar()->isShown())
                        visibleSize.y -= panel->getHorizontalScrollbar()->getWidth();

                    const Vector2f childPos = child->getPosition();
                    const Vector2f childSize = child->getFullSize();
                    Vector2f newOffset = contentOffset;
                    if (childPos.x < contentOffset.x)
                        newOffset.x = childPos.x;
                    else if (childPos.x + childSize.x > contentOffset.x + visibleSize.x)
                        newOffset.x = std::min(childPos.x, childPos.x + childSize.x - visibleSize.x);

                    if (childPos.y < contentOffset.y)
                        newOffset.y = childPos.y;
                    else if (childPos.y + childSize.y > contentOffset.y + visibleSize.y)
                        newOffset.y = std::min(childPos.y, childPos.y + childSize.y - visibleSize.y);

                    panel->getHorizontalScrollbar()->setValue(static_cast<unsigned int>(std::max(0.f, std::round(newOffset.x))));
                    panel->getVerticalScrollbar()->setValue(static_cast<unsigned int>(std::max(0.f, std::round(newOffset.y))));
                }

                return scrollablePanelFound;
            }

            return AccessibleWidgetHandler::getHandler(*widget).performAction(*widget, m_part, action);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::setValue(const String& value)
        {
            const auto widget = getWidget();
            return widget && isEnabled() && AccessibleWidgetHandler::getHandler(*widget).setValue(*widget, m_part, value);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::setRangeValue(double value)
        {
            const auto widget = getWidget();
            return widget && isEnabled() && AccessibleWidgetHandler::getHandler(*widget).setRangeValue(*widget, m_part, value);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getSelectionContainer() const
        {
            for (auto ancestor = getParent(); ancestor; ancestor = ancestor->getParent())
            {
                if (ancestor->getState().selectionContainer)
                    return ancestor;
            }

            // Items without a selection container are grouped by their parent (e.g. radio buttons)
            return getParent();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<AccessibleObject::Ptr> AccessibleObject::getSelectedChildren()
        {
            // Selected items can be nested (e.g. in a tree view), so all descendants are searched
            std::vector<Ptr> selectedObjects;
            for (const auto& child : getChildren())
                child->addSelectedDescendants(selectedObjects);

            return selectedObjects;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getTableCell(std::size_t row, std::size_t column)
        {
            // The rows of a table are its children with the TableRow role, the cells are the children of the rows
            std::size_t rowIndex = 0;
            for (const auto& child : getChildren())
            {
                if (child->getRole() != AccessibleRole::TableRow)
                    continue;

                if (rowIndex++ != row)
                    continue;

                const auto cells = child->getChildren();
                return (column < cells.size()) ? cells[column] : nullptr;
            }

            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<AccessibleObject::Ptr> AccessibleObject::getTableColumnHeaders()
        {
            for (const auto& child : getChildren())
            {
                if (child->getRole() == AccessibleRole::Header)
                    return child->getChildren();
            }

            return {};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getContainingTable() const
        {
            for (auto ancestor = getParent(); ancestor; ancestor = ancestor->getParent())
            {
                if (ancestor->getRole() == AccessibleRole::Table)
                    return ancestor;
            }

            return nullptr;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleObject::Ptr AccessibleObject::getFocusedDescendant()
        {
            const auto widget = getWidget();
            if (!widget || !m_part.empty())
                return nullptr;

            const String focusedPart = AccessibleWidgetHandler::getHandler(*widget).getFocusedPart(*widget);
            if (focusedPart.empty())
                return nullptr;

            // The part might be nested inside other parts, so we search it through the children to give it the right parent
            std::vector<Ptr> objectsToSearch = getChildren();
            while (!objectsToSearch.empty())
            {
                const auto object = objectsToSearch.back();
                objectsToSearch.pop_back();
                if (object->getWidget() != widget)
                    continue;

                if (object->getPart() == focusedPart)
                    return object;

                // Only descend into parts that contain the focused part (keys of nested parts start with the key of their parent)
                if (focusedPart.starts_with(object->getPart() + U"/"))
                {
                    const auto children = object->getChildren();
                    objectsToSearch.insert(objectsToSearch.end(), children.begin(), children.end());
                }
            }

            return m_tree.getPartObject(widget, focusedPart, shared_from_this());
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String AccessibleObject::getText() const
        {
            const AccessibleState state = getState();
            if (!state.hasText || !state.value)
                return {};

            return *state.value;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::pair<std::size_t, std::size_t> AccessibleObject::getTextSelection() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {0, 0};

            return AccessibleWidgetHandler::getHandler(*widget).getTextSelection(*widget);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleObject::setTextSelection(std::size_t start, std::size_t end)
        {
            const auto widget = getWidget();
            return widget && AccessibleWidgetHandler::getHandler(*widget).setTextSelection(*widget, start, end);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::pair<std::size_t, std::size_t>> AccessibleObject::getTextLines() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            auto lines = AccessibleWidgetHandler::getHandler(*widget).getTextLines(*widget);
            if (lines.empty())
                lines.emplace_back(0, getText().length());

            return lines;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleObject::getCharacterBounds(std::size_t index) const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            const FloatRect bounds = AccessibleWidgetHandler::getHandler(*widget).getCharacterBounds(*widget, index);
            return {getAbsoluteWidgetPosition() + bounds.getPosition(), bounds.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t AccessibleObject::getTextIndexAtPosition(Vector2f pos) const
        {
            const auto widget = getWidget();
            if (!widget)
                return 0;

            return AccessibleWidgetHandler::getHandler(*widget).getTextIndexAtPosition(*widget, pos - getAbsoluteWidgetPosition());
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleObject::getVisibleTextArea() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            const FloatRect area = AccessibleWidgetHandler::getHandler(*widget).getTextArea(*widget);
            return {getAbsoluteWidgetPosition() + area.getPosition(), area.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleObject::scrollTextToIndex(std::size_t index)
        {
            if (const auto widget = getWidget())
                AccessibleWidgetHandler::getHandler(*widget).scrollTextToIndex(*widget, index);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleTextAttributes AccessibleObject::getTextAttributes() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            return AccessibleWidgetHandler::getHandler(*widget).getTextAttributes(*widget);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleObject::invalidate()
        {
            m_widget.reset();
            m_parent.reset();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Vector2f AccessibleObject::getAbsoluteWidgetPosition() const
        {
            const auto widget = getWidget();
            if (!widget)
                return {};

            const Vector2f position = widget->getAbsolutePosition();

            // Widgets inside a SubwidgetContainer have a parent that isn't connected to the gui, so getAbsolutePosition only
            // returns the position relative to the SubwidgetContainer. In that case we add the position of that container.
            const Widget* topWidget = widget.get();
            while (topWidget->getParent())
                topWidget = topWidget->getParent();

            if (dynamic_cast<const RootContainer*>(topWidget))
                return position;

            for (auto ancestor = getParent(); ancestor; ancestor = ancestor->getParent())
            {
                const auto subwidgetContainer = std::dynamic_pointer_cast<SubwidgetContainer>(ancestor->getWidget());
                if (subwidgetContainer && ancestor->getPart().empty() && (subwidgetContainer->getContainer() == topWidget))
                    return position + ancestor->getAbsoluteWidgetPosition();
            }

            return position;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleObject::addWidgetChildren(const Container& container, std::vector<Ptr>& children)
        {
            const auto self = shared_from_this();
            for (const auto& childWidget : container.getWidgets())
            {
                // Hidden widgets are not part of the accessibility tree
                if (!childWidget->isVisible())
                    continue;

                // The list of an open combo box and the open menus are shown by their own widget
                if (AccessibilityTree::isInternalPopupWidget(*childWidget))
                    continue;

                if (auto child = m_tree.getWidgetObject(childWidget, self))
                    children.push_back(std::move(child));
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleObject::addSelectedDescendants(std::vector<Ptr>& selectedObjects)
        {
            const AccessibleState state = getState();
            if (state.selectable && state.selected)
                selectedObjects.push_back(shared_from_this());

            // Selection containers inside this object manage their own selection
            if (state.selectionContainer)
                return;

            for (const auto& child : getChildren())
                child->addSelectedDescendants(selectedObjects);
        }
    } // namespace priv
} // namespace tgui
