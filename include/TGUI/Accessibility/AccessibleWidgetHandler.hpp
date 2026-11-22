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

#ifndef TGUI_ACCESSIBLE_WIDGET_HANDLER_HPP
#define TGUI_ACCESSIBLE_WIDGET_HANDLER_HPP

#include <TGUI/Accessibility/AccessibilityTypes.hpp>
#include <TGUI/Widget.hpp>

#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        class AccessibilityTree;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Describes a type of widget to assistive technology.
        //
        // There is one handler per widget type, which only accesses the widget via its public interface. Handlers don't
        // store any state, the widget is passed to every function. Parts identify things inside a widget that aren't widgets
        // themselves (e.g. list box items). They are only meaningful to the handler that created them, an empty part
        // always refers to the widget itself.
        //
        // This base class handles the properties that every widget has, and is used for widgets without a specific handler.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleWidgetHandler
        {
        public:
            // Virtual destructor
            virtual ~AccessibleWidgetHandler() = default;

            // Returns the handler that should be used for the widget, based on its type
            [[nodiscard]] static const AccessibleWidgetHandler& getHandler(const Widget& widget);

            // Fills in the state of the widget or of one of its parts. Returns false when the part doesn't exist.
            virtual bool getState(const Widget& widget, const String& part, AccessibleState& state) const;

            // Returns the parts that are children of the given part, or of the widget itself when the part is empty
            [[nodiscard]] virtual std::vector<String> getChildParts(const Widget& widget, const String& part) const;

            // Returns the bounding rectangle of a part, relative to the top-left of the widget
            [[nodiscard]] virtual FloatRect getPartBounds(const Widget& widget, const String& part) const;

            // Returns the part that has keyboard focus while the widget is focused, or an empty string if the widget has it
            [[nodiscard]] virtual String getFocusedPart(const Widget& widget) const;

            // Performs an action on the widget or on one of its parts. Returns whether the action was performed.
            virtual bool performAction(Widget& widget, const String& part, AccessibleAction action) const;

            // Changes the textual value of the widget or part. Returns whether the value was changed.
            virtual bool setValue(Widget& widget, const String& part, const String& value) const;

            // Changes the numeric value of the widget or part. Returns whether the value was changed.
            virtual bool setRangeValue(Widget& widget, const String& part, double value) const;

            // Returns the start and end of the selected text. The start is never larger than the end.
            [[nodiscard]] virtual std::pair<std::size_t, std::size_t> getTextSelection(const Widget& widget) const;

            // Selects text in the widget. Returns whether the selection was changed.
            virtual bool setTextSelection(Widget& widget, std::size_t start, std::size_t end) const;

            // Returns the start and end of each line as it is displayed on the screen.
            // The end of a line includes the newline character that ends it.
            [[nodiscard]] virtual std::vector<std::pair<std::size_t, std::size_t>> getTextLines(const Widget& widget) const;

            // Returns the bounding rectangle of a character, relative to the top-left of the widget
            [[nodiscard]] virtual FloatRect getCharacterBounds(const Widget& widget, std::size_t index) const;

            // Returns the index of the character closest to a position relative to the top-left of the widget
            [[nodiscard]] virtual std::size_t getTextIndexAtPosition(const Widget& widget, Vector2f pos) const;

            // Returns the area in which the text is visible, relative to the top-left of the widget
            [[nodiscard]] virtual FloatRect getTextArea(const Widget& widget) const;

            // Scrolls the text so that the character at the given index becomes visible
            virtual void scrollTextToIndex(Widget& widget, std::size_t index) const;

            // Returns the font and colors of the text
            [[nodiscard]] virtual AccessibleTextAttributes getTextAttributes(const Widget& widget) const;

            // Connects the accessibility handlers of the signals that inform the tree about changes to the widget
            virtual void connectSignals(Widget& widget, AccessibilityTree& tree) const;

            // Removes the accessibility handlers that were set in connectSignals
            virtual void disconnectSignals(Widget& widget) const;

        protected:
            // Sets the accessibility handler of the signal to queue an event about the widget (or one of its parts) in the tree
            static void connectSignal(Signal& signal,
                                      Widget& widget,
                                      AccessibilityTree& tree,
                                      AccessibleEvent event,
                                      AccessibleProperty property = {});

            // Removes the accessibility handler of the signal
            static void disconnectSignal(Signal& signal);
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for containers without a meaning of their own (e.g. Panel, Group and the layouts)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleContainerHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for the root container of the gui, which represents the window
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleRootContainerHandler : public AccessibleContainerHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for child windows (and the widgets that inherit from it, such as MessageBox)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleChildWindowHandler : public AccessibleContainerHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool performAction(Widget& widget, const String& part, AccessibleAction action) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for labels
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleLabelHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for buttons (and the widgets that inherit from it, such as BitmapButton)
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleButtonHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool performAction(Widget& widget, const String& part, AccessibleAction action) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for edit boxes
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleEditBoxHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool setValue(Widget& widget, const String& part, const String& value) const override;
            [[nodiscard]] std::pair<std::size_t, std::size_t> getTextSelection(const Widget& widget) const override;
            bool setTextSelection(Widget& widget, std::size_t start, std::size_t end) const override;
            [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> getTextLines(const Widget& widget) const override;
            [[nodiscard]] FloatRect getCharacterBounds(const Widget& widget, std::size_t index) const override;
            [[nodiscard]] std::size_t getTextIndexAtPosition(const Widget& widget, Vector2f pos) const override;
            [[nodiscard]] FloatRect getTextArea(const Widget& widget) const override;
            void scrollTextToIndex(Widget& widget, std::size_t index) const override;
            [[nodiscard]] AccessibleTextAttributes getTextAttributes(const Widget& widget) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for text areas
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleTextAreaHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool setValue(Widget& widget, const String& part, const String& value) const override;
            [[nodiscard]] std::pair<std::size_t, std::size_t> getTextSelection(const Widget& widget) const override;
            bool setTextSelection(Widget& widget, std::size_t start, std::size_t end) const override;
            [[nodiscard]] std::vector<std::pair<std::size_t, std::size_t>> getTextLines(const Widget& widget) const override;
            [[nodiscard]] FloatRect getCharacterBounds(const Widget& widget, std::size_t index) const override;
            [[nodiscard]] std::size_t getTextIndexAtPosition(const Widget& widget, Vector2f pos) const override;
            [[nodiscard]] FloatRect getTextArea(const Widget& widget) const override;
            void scrollTextToIndex(Widget& widget, std::size_t index) const override;
            [[nodiscard]] AccessibleTextAttributes getTextAttributes(const Widget& widget) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for toggle buttons
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleToggleButtonHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool performAction(Widget& widget, const String& part, AccessibleAction action) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for check boxes
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleCheckBoxHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool performAction(Widget& widget, const String& part, AccessibleAction action) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for radio buttons. The radio buttons inside the same parent form a group.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleRadioButtonHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool performAction(Widget& widget, const String& part, AccessibleAction action) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for widgets with a numeric value: Slider, Knob, SpinButton, ProgressBar and Scrollbar
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleRangeHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            bool setRangeValue(Widget& widget, const String& part, double value) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for pictures
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessiblePictureHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for separator lines
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleSeparatorLineHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handler for Tabs and VerticalTabs. The tabs are parts with the tab index as key.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class AccessibleTabsHandler : public AccessibleWidgetHandler
        {
        public:
            bool getState(const Widget& widget, const String& part, AccessibleState& state) const override;
            [[nodiscard]] std::vector<String> getChildParts(const Widget& widget, const String& part) const override;
            [[nodiscard]] FloatRect getPartBounds(const Widget& widget, const String& part) const override;
            [[nodiscard]] String getFocusedPart(const Widget& widget) const override;
            bool performAction(Widget& widget, const String& part, AccessibleAction action) const override;
            void connectSignals(Widget& widget, AccessibilityTree& tree) const override;
            void disconnectSignals(Widget& widget) const override;
        };
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_ACCESSIBLE_WIDGET_HANDLER_HPP
