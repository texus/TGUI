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

#ifndef TGUI_ACCESSIBLE_OBJECT_HPP
#define TGUI_ACCESSIBLE_OBJECT_HPP

#include <TGUI/Accessibility/AccessibilityTypes.hpp>
#include <TGUI/Widget.hpp>

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        class AccessibilityTree;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // A widget, or a part of a widget (e.g. an item in a list box), as it is exposed to assistive technology.
        //
        // The object doesn't contain any information itself, it asks the widget for everything via the accessibility
        // functions in the Widget class. The part identifies which part of the widget the object represents, it is an empty
        // string when the object represents the widget itself. The objects are created and cached by the AccessibilityTree,
        // which ensures that the same object is returned every time the same widget or part is requested.
        //
        // All coordinates are in the coordinate system of the gui view (the same coordinates as used for widget positions).
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API AccessibleObject : public std::enable_shared_from_this<AccessibleObject>
        {
        public:
            using Ptr = std::shared_ptr<AccessibleObject>; //!< Shared accessible object pointer

            // Constructor
            AccessibleObject(AccessibilityTree& tree, const Widget::Ptr& widget, String part);

            AccessibleObject(const AccessibleObject&) = delete;
            AccessibleObject& operator=(const AccessibleObject&) = delete;

            // Returns the widget that this object belongs to, or a nullptr if the widget no longer exists
            [[nodiscard]] Widget::Ptr getWidget() const;

            // Returns which part of the widget this object represents, or an empty string if it represents the widget itself
            [[nodiscard]] const String& getPart() const
            {
                return m_part;
            }

            // Returns the tree that owns this object
            [[nodiscard]] AccessibilityTree& getTree() const
            {
                return m_tree;
            }

            // Returns a number that uniquely identifies this object within its tree
            [[nodiscard]] unsigned int getUniqueId() const
            {
                return m_uniqueId;
            }

            // Returns whether the widget or part that this object represents still exists.
            // All functions in this class can still be called after this function returns false, they will just return
            // default values in that case.
            [[nodiscard]] bool isAlive() const;

            // Returns the state of the widget or part, as filled in by the widget
            [[nodiscard]] AccessibleState getState() const;

            // Returns the role of the object
            [[nodiscard]] AccessibleRole getRole() const;

            // Returns the name of the object (the accessible name of the widget, or e.g. the text on a button)
            [[nodiscard]] String getName() const;

            // Returns the label that names the widget, which is the label directly in front of a widget without a name of its
            // own (e.g. the text in front of a slider), like the static text in front of a control in a native Windows dialog.
            // Returns a nullptr if the name of the object doesn't come from a label.
            [[nodiscard]] Ptr getLabel() const;

            // Returns the long description of the object
            [[nodiscard]] String getDescription() const;

            // Returns a short help text, which is the text of the tool tip
            [[nodiscard]] String getHelpText() const;

            // Returns an identifier that remains the same between runs of the program (based on the widget name)
            [[nodiscard]] String getAutomationId() const;

            // Returns the name of the class of the object (the widget type)
            [[nodiscard]] String getClassName() const;

            // Returns the bounding rectangle of the object, in gui coordinates
            [[nodiscard]] FloatRect getBounds() const;

            // Returns the parent of this object, or a nullptr if this is the root object
            [[nodiscard]] Ptr getParent() const;

            // Returns the children of this object, in navigation order.
            // The children of a widget are the visible widgets inside it, followed by the parts of the widget.
            [[nodiscard]] std::vector<Ptr> getChildren();

            // Returns the deepest object at the given position, or a nullptr if the position is not on top of this object
            [[nodiscard]] Ptr getObjectAtPosition(Vector2f pos);

            // Returns whether the object is enabled
            [[nodiscard]] bool isEnabled() const;

            // Returns whether the object is hidden or scrolled out of the visible area
            [[nodiscard]] bool isOffscreen() const;

            // Returns whether the object can receive keyboard focus
            [[nodiscard]] bool isFocusable() const;

            // Gives keyboard focus to the object
            bool setFocus();

            // Performs an action on the object
            bool performAction(AccessibleAction action);

            // Changes the textual value of the object
            bool setValue(const String& value);

            // Changes the numeric value of the object
            bool setRangeValue(double value);

            // Returns the closest ancestor that manages the selection of this item
            [[nodiscard]] Ptr getSelectionContainer() const;

            // Returns the selected items inside this selection container
            [[nodiscard]] std::vector<Ptr> getSelectedChildren();

            // Returns the cell at the given row and column when this object is a table
            [[nodiscard]] Ptr getTableCell(std::size_t row, std::size_t column);

            // Returns the column headers when this object is a table
            [[nodiscard]] std::vector<Ptr> getTableColumnHeaders();

            // Returns the table that contains this cell
            [[nodiscard]] Ptr getContainingTable() const;

            // Returns the object that should receive the focus event when this object is the focused widget.
            // For example, a list box forwards the focus to its selected item.
            [[nodiscard]] Ptr getFocusedDescendant();

            // Returns the text of a widget that has text (which is its value), or an empty string otherwise
            [[nodiscard]] String getText() const;

            // Returns the start and end of the selected text, with the start always being smaller than or equal to the end
            [[nodiscard]] std::pair<std::size_t, std::size_t> getTextSelection() const;

            // Selects the text between start and end
            bool setTextSelection(std::size_t start, std::size_t end);

            // Returns the start and end of each line of the text as it is displayed on the screen
            [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> getTextLines() const;

            // Returns the bounding rectangle of a character in gui coordinates
            [[nodiscard]] FloatRect getCharacterBounds(std::size_t index) const;

            // Returns the index of the character closest to the given position in gui coordinates
            [[nodiscard]] std::size_t getTextIndexAtPosition(Vector2f pos) const;

            // Returns the area in which the text is visible, in gui coordinates
            [[nodiscard]] FloatRect getVisibleTextArea() const;

            // Scrolls the text so that the character at the given index becomes visible
            void scrollTextToIndex(std::size_t index);

            // Returns the attributes of the text
            [[nodiscard]] AccessibleTextAttributes getTextAttributes() const;

            // Called by the tree when the object is being removed from the cache
            void invalidate();

        private:
            // Returns the position of the widget relative to the gui, also for widgets inside a SubwidgetContainer
            [[nodiscard]] Vector2f getAbsoluteWidgetPosition() const;

            // Adds the objects for the visible widgets inside the container to the list of children
            void addWidgetChildren(const Container& container, std::vector<Ptr>& children);

            // Adds the selected parts of this object and of its descendants to the list
            void addSelectedDescendants(std::vector<Ptr>& selectedObjects);

        private:
            AccessibilityTree& m_tree;
            std::weak_ptr<Widget> m_widget;
            String m_part;
            std::weak_ptr<AccessibleObject> m_parent; // Set by the tree each time the object is requested
            unsigned int m_uniqueId = 0;

            friend class AccessibilityTree;
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBLE_OBJECT_HPP
