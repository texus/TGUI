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
#include <TGUI/Widgets/TextArea.hpp>

#include <algorithm>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    // Returns the font that is used to display the text of the text area
    [[nodiscard]] tgui::Font getTextAreaFont(const tgui::TextArea& textArea)
    {
        const tgui::Font rendererFont = textArea.getSharedRenderer()->getFont();
        return rendererFont ? rendererFont : textArea.getInheritedFont();
    }

    // Returns the height of a single line in the text area
    [[nodiscard]] float getTextAreaLineHeight(const tgui::TextArea& textArea)
    {
        const tgui::Font font = getTextAreaFont(textArea);
        return font ? tgui::Text::getLineHeight(font, textArea.getTextSize()) : static_cast<float>(textArea.getTextSize());
    }
} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleTextAreaHandler::getState(const Widget& widget, const String& part, AccessibleState& state) const
        {
            if (!part.empty())
                return false;

            const auto& textArea = static_cast<const TextArea&>(widget);
            state.role = AccessibleRole::MultiLineEdit;
            state.value = textArea.getText();
            state.hasText = true;
            state.readOnly = textArea.isReadOnly();
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleTextAreaHandler::setValue(Widget& widget, const String& part, const String& value) const
        {
            auto& textArea = static_cast<TextArea&>(widget);
            if (!part.empty() || textArea.isReadOnly())
                return false;

            textArea.setText(value);
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::pair<std::size_t, std::size_t> AccessibleTextAreaHandler::getTextSelection(const Widget& widget) const
        {
            const auto& textArea = static_cast<const TextArea&>(widget);
            return std::minmax(textArea.getSelectionStart(), textArea.getSelectionEnd());
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool AccessibleTextAreaHandler::setTextSelection(Widget& widget, std::size_t start, std::size_t end) const
        {
            static_cast<TextArea&>(widget).setSelectedText(start, end);
            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<std::pair<std::size_t, std::size_t>> AccessibleTextAreaHandler::getTextLines(const Widget& widget) const
        {
            // Each line ends where the next one starts, which includes the newline character at the end of the line
            const auto& textArea = static_cast<const TextArea&>(widget);
            const std::size_t lineCount = textArea.getLinesCount();
            std::vector<std::pair<std::size_t, std::size_t>> lines;
            lines.reserve(lineCount);
            for (std::size_t i = 0; i < lineCount; ++i)
                lines.emplace_back(textArea.getLineStartIndex(i), textArea.getLineStartIndex(i + 1));

            return lines;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleTextAreaHandler::getCharacterBounds(const Widget& widget, std::size_t index) const
        {
            const auto& textArea = static_cast<const TextArea&>(widget);
            const String text = textArea.getText();
            index = std::min(index, text.length());

            // The character at the end of a line (or at the end of the text) has no width
            const Vector2f pos = textArea.getCharacterPosition(index);
            float right = pos.x;
            if ((index < text.length()) && (text[index] != U'\n'))
            {
                const Vector2f nextPos = textArea.getCharacterPosition(index + 1);
                if (nextPos.y == pos.y)
                    right = nextPos.x;
            }

            return {pos, {right - pos.x, getTextAreaLineHeight(textArea)}};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::size_t AccessibleTextAreaHandler::getTextIndexAtPosition(const Widget& widget, Vector2f pos) const
        {
            const auto& textArea = static_cast<const TextArea&>(widget);
            const auto lines = getTextLines(widget);
            if (lines.empty())
                return 0;

            // Find the line that contains the position, and then the character on that line
            const float lineHeight = getTextAreaLineHeight(textArea);
            for (const auto& line : lines)
            {
                if (pos.y >= textArea.getCharacterPosition(line.first).y + lineHeight)
                    continue;

                for (std::size_t i = line.first; i + 1 < line.second; ++i)
                {
                    const float left = textArea.getCharacterPosition(i).x;
                    const float right = textArea.getCharacterPosition(i + 1).x;
                    if (pos.x < (left + right) / 2.f)
                        return i;
                }

                return (line.second > line.first) && (line.second < textArea.getText().length()) ? line.second - 1 : line.second;
            }

            return lines.back().second;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        FloatRect AccessibleTextAreaHandler::getTextArea(const Widget& widget) const
        {
            return {{}, widget.getSize()};
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleTextAreaHandler::scrollTextToIndex(Widget& widget, std::size_t index) const
        {
            // Scroll vertically so that the line with the character is visible
            auto& textArea = static_cast<TextArea&>(widget);
            const float lineHeight = getTextAreaLineHeight(textArea);
            const float charTop = textArea.getCharacterPosition(index).y;
            const float scrollValue = static_cast<float>(textArea.getVerticalScrollbar()->getValue());
            if (charTop < 0)
                textArea.getVerticalScrollbar()->setValue(static_cast<unsigned int>(std::max(0.f, scrollValue + charTop)));
            else if (charTop + lineHeight > textArea.getSize().y)
                textArea.getVerticalScrollbar()->setValue(static_cast<unsigned int>(scrollValue + charTop + lineHeight - textArea.getSize().y));
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        AccessibleTextAttributes AccessibleTextAreaHandler::getTextAttributes(const Widget& widget) const
        {
            const auto& textArea = static_cast<const TextArea&>(widget);
            const auto* renderer = textArea.getSharedRenderer();

            AccessibleTextAttributes attributes;
            attributes.fontName = getTextAreaFont(textArea).getId();
            attributes.fontSize = static_cast<float>(textArea.getTextSize());
            attributes.foregroundColor = renderer->getTextColor();
            attributes.backgroundColor = renderer->getBackgroundColor();
            return attributes;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleTextAreaHandler::connectSignals(Widget& widget, AccessibilityTree& tree) const
        {
            AccessibleWidgetHandler::connectSignals(widget, tree);

            auto& textArea = static_cast<TextArea&>(widget);
            textArea.onTextChange.setAccessibilityHandler(
                [&tree, &textArea]
                {
                    tree.queueEvent(textArea, AccessibleEvent::TextChanged, {}, {});
                    tree.queueEvent(textArea, AccessibleEvent::PropertyChanged, AccessibleProperty::Value, {});
                });
            connectSignal(textArea.onCaretPositionChange, textArea, tree, AccessibleEvent::TextSelectionChanged);
            connectSignal(textArea.onSelectionChange, textArea, tree, AccessibleEvent::TextSelectionChanged);
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void AccessibleTextAreaHandler::disconnectSignals(Widget& widget) const
        {
            AccessibleWidgetHandler::disconnectSignals(widget);

            auto& textArea = static_cast<TextArea&>(widget);
            disconnectSignal(textArea.onTextChange);
            disconnectSignal(textArea.onCaretPositionChange);
            disconnectSignal(textArea.onSelectionChange);
        }
    } // namespace priv
} // namespace tgui
