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
#include <TGUI/Widgets/EditBox.hpp>

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    // Returns the font that is used to display the text of the edit box
    [[nodiscard]] tgui::Font getEditBoxFont(const tgui::EditBox& editBox)
    {
        const tgui::Font rendererFont = editBox.getSharedRenderer()->getFont();
        return rendererFont ? rendererFont : editBox.getInheritedFont();
    }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleEditBoxHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            // When the edit box contains a password, only the password characters are exposed
            const auto& editBox = static_cast<const EditBox&>(widget);
            const String& text = editBox.getText();
            const char32_t passwordChar = editBox.getPasswordCharacter();
            state.role = AccessibleRole::Edit;
            state.value = (passwordChar != U'\0') ? String(text.length(), passwordChar) : text;
            state.hasText = true;
            state.readOnly = editBox.isReadOnly();
            state.password = (passwordChar != U'\0');
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleEditBoxHandler::setValue(Widget& widget, const String& part, const String& value) const
        {
            auto& editBox = static_cast<EditBox&>(widget);
            if (!part.empty() || editBox.isReadOnly())
                return false;

            editBox.setText(value);
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::pair<std::size_t, std::size_t> AccessibleEditBoxHandler::getTextSelection(const Widget& widget) const
        {
            const auto& editBox = static_cast<const EditBox&>(widget);
            return std::minmax(editBox.getSelectionStart(), editBox.getSelectionEnd());
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleEditBoxHandler::setTextSelection(Widget& widget, std::size_t start, std::size_t end) const
        {
            static_cast<EditBox&>(widget).selectText(start, end - start);
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::pair<std::size_t, std::size_t>> AccessibleEditBoxHandler::getTextLines(const Widget& widget) const
        {
            return {{0, static_cast<const EditBox&>(widget).getText().length()}};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleEditBoxHandler::getCharacterBounds(const Widget& widget, std::size_t index) const
        {
            const auto& editBox = static_cast<const EditBox&>(widget);
            const std::size_t textLength = editBox.getText().length();
            index = std::min(index, textLength);

            const Vector2f pos = editBox.getCharacterPosition(index);
            const float right = (index < textLength) ? editBox.getCharacterPosition(index + 1).x : pos.x;

            const Font font = getEditBoxFont(editBox);
            const float height = font ? Text::getLineHeight(font, editBox.getTextSize()) : editBox.getSize().y;
            return {pos, {right - pos.x, height}};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t AccessibleEditBoxHandler::getTextIndexAtPosition(const Widget& widget, Vector2f pos) const
        {
            // Find the character of which the middle is closest to the position
            const auto& editBox = static_cast<const EditBox&>(widget);
            const std::size_t textLength = editBox.getText().length();
            for (std::size_t i = 0; i < textLength; ++i)
            {
                const float left = editBox.getCharacterPosition(i).x;
                const float right = editBox.getCharacterPosition(i + 1).x;
                if (pos.x < (left + right) / 2.f)
                    return i;
            }

            return textLength;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleEditBoxHandler::getTextArea(const Widget& widget) const
        {
            return {{}, widget.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleEditBoxHandler::scrollTextToIndex(Widget&, std::size_t) const
        {
            // An edit box only scrolls to keep the caret visible, there is no way to scroll without moving the caret
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleTextAttributes AccessibleEditBoxHandler::getTextAttributes(const Widget& widget) const
        {
            const auto& editBox = static_cast<const EditBox&>(widget);
            const auto* renderer = editBox.getSharedRenderer();

            AccessibleTextAttributes attributes;
            attributes.fontName = getEditBoxFont(editBox).getId();
            attributes.fontSize = static_cast<float>(editBox.getTextSize());
            attributes.foregroundColor = renderer->getTextColor();
            attributes.backgroundColor = renderer->getBackgroundColor();
            return attributes;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleEditBoxHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            auto& editBox = static_cast<EditBox&>(widget);
            editBox.onTextChange.setAccessibilityHandler(
                [&tree, &editBox]
                {
                    tree.queueEvent(editBox, AccessibleEvent::TextChanged, {}, {});
                    tree.queueEvent(editBox, AccessibleEvent::PropertyChanged, AccessibleProperty::Value, {});
                });
            connectSignal(editBox.onCaretPositionChange, editBox, tree, AccessibleEvent::TextSelectionChanged);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleEditBoxHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);

            auto& editBox = static_cast<EditBox&>(widget);
            disconnectSignal(editBox.onTextChange);
            disconnectSignal(editBox.onCaretPositionChange);
        }
    } // namespace priv
} // namespace tgui
