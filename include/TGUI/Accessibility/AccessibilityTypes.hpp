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

#ifndef TGUI_ACCESSIBILITY_TYPES_HPP
#define TGUI_ACCESSIBILITY_TYPES_HPP

#include <TGUI/AccessibleRole.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Layout.hpp>
#include <TGUI/Optional.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Vector2.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // State of a widget that can be toggled
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class AccessibleToggleState
        {
            Off,
            On,
            Indeterminate
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // State of a widget that can be expanded or collapsed
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class AccessibleExpandState
        {
            Collapsed,
            Expanded,
            PartiallyExpanded,
            LeafNode
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Information about a widget that has a numeric value within a range (e.g. a slider)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct AccessibleRangeInfo
        {
            double value = 0;
            double minimum = 0;
            double maximum = 0;
            double smallChange = 0;
            double largeChange = 0;
            bool readOnly = false;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Properties of which a change is reported to assistive technology
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class AccessibleProperty
        {
            Name,
            Description,
            HelpText,
            Value,
            RangeValue,
            ToggleState,
            ExpandState,
            Selected,
            Enabled,
            Offscreen,
            ReadOnly,
            Bounds
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Events that widgets report to the accessibility backend when their state changes
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class AccessibleEvent
        {
            FocusChanged,         // The widget (or a part of it) received keyboard focus
            PropertyChanged,      // A property of the widget or part changed
            ChildrenChanged,      // Children were added to or removed from the widget
            TextChanged,          // The text inside the widget changed
            TextSelectionChanged, // The caret or selection inside the widget changed
            ElementSelected,      // The part became the selected item of the widget
            WindowOpened,         // The widget is a window that was shown
            WindowClosed,         // The widget is a window that was hidden
            MenuOpened,           // A menu of the widget was opened
            MenuClosed,           // A menu of the widget was closed
            ObjectRemoved         // The accessible object no longer exists (only reported by the tree itself)
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Attributes of the text in a widget
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct AccessibleTextAttributes
        {
            String fontName;
            float fontSize = 0;
            bool bold = false;
            bool italic = false;
            bool underlined = false;
            bool strikeThrough = false;
            Color foregroundColor;
            Color backgroundColor;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Actions that assistive technology can perform on a widget or on a part of a widget
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        enum class AccessibleAction
        {
            Invoke,              // Perform the default action (e.g. press a button)
            Toggle,              // Change the toggle state to the next state
            Expand,              // Show the children (e.g. open a combo box or a tree node)
            Collapse,            // Hide the children
            Select,              // Select the item and deselect all other items
            AddToSelection,      // Add the item to the selection
            RemoveFromSelection, // Remove the item from the selection
            ScrollIntoView,      // Scroll the parent so that the item becomes visible
            Close                // Close the window
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Describes a widget, or a part of a widget, as it is presented to assistive technology.
        // Handlers fill in this structure in AccessibleWidgetHandler::getState. Values that aren't filled in mean that the widget
        // doesn't support that property (e.g. a widget without toggle state can't be toggled).
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct AccessibleState
        {
            AccessibleRole role = AccessibleRole::Custom;      // What kind of control it is
            String name;                                       // Name used when the widget has no accessible name
            String description;                                // Description used when the widget has no accessible description
            Optional<String> value;                            // Textual value (e.g. text of an edit box)
            Optional<AccessibleRangeInfo> rangeInfo;           // Numeric value (e.g. value of a slider)
            Optional<AccessibleToggleState> toggleState;       // Toggle state for widgets that can be toggled
            Optional<AccessibleExpandState> expandState;       // Expand state for widgets that can be expanded
            Optional<Orientation> orientation;                 // Direction of e.g. a slider or scrollbar
            Optional<Vector2<std::size_t>> tableSize;          // Amount of rows and columns when the widget is a table
            Optional<Vector2<std::size_t>> cellPosition;       // Row and column when the part is a cell in a table
            int positionInSet = 0;                             // 1-based position between siblings, 0 if unknown
            int sizeOfSet = 0;                                 // Amount of siblings including itself, 0 if unknown
            int level = 0;                                     // 1-based depth in a hierarchy, 0 if unknown
            bool focusable = false;                            // Can the part receive keyboard focus? (ignored for widgets)
            bool enabled = true;                               // Can the user interact with the part? (ignored for widgets)
            bool offscreen = false;                            // Is the part hidden or scrolled out of view?
            bool readOnly = false;                             // Can the value be changed?
            bool password = false;                             // Is the value a password that shouldn't be read?
            bool modal = false;                                // Does the window block interaction with the rest of the gui?
            bool hasText = false;                              // Does the widget support the text functions in Widget?
            bool invokable = false;                            // Does the Invoke action do something?
            bool closable = false;                             // Does the Close action do something?
            bool scrollable = false;                           // Does the ScrollIntoView action do something?
            bool selectable = false;                           // Can the part be selected inside its selection container?
            bool selected = false;                             // Is the part currently selected?
            bool selectionContainer = false;                   // Does it contain items that can be selected?
            bool multipleSelection = false;                    // Can more than one item be selected at the same time?
            bool selectionRequired = false;                    // Must an item always be selected?
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBILITY_TYPES_HPP
