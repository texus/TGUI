/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/TextArea.hpp>
#include <TGUI/Keyboard.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char TextArea::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextArea::TextArea(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection1.setFont(m_fontCached);
        m_textSelection2.setFont(m_fontCached);
        m_textAfterSelection1.setFont(m_fontCached);
        m_textAfterSelection2.setFont(m_fontCached);
        m_defaultText.setFont(m_fontCached);

        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);
        m_horizontalScrollbar->setVisible(false);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<TextAreaRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setSize({Text::getLineHeight(m_fontCached, m_textSizeCached) * 18,
                     10 * m_fontCached.getLineSpacing(m_textSizeCached)
                     + std::max(m_fontCached.getFontHeight(m_textSizeCached), m_fontCached.getLineSpacing(m_textSizeCached)) - m_fontCached.getLineSpacing(m_textSizeCached)
                     + Text::getExtraVerticalPadding(m_textSizeCached)
                     + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextArea::Ptr TextArea::create()
    {
        return std::make_shared<TextArea>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextArea::Ptr TextArea::copy(const TextArea::ConstPtr& textArea)
    {
        if (textArea)
            return std::static_pointer_cast<TextArea>(textArea->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextAreaRenderer* TextArea::getSharedRenderer()
    {
        return aurora::downcast<TextAreaRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextAreaRenderer* TextArea::getSharedRenderer() const
    {
        return aurora::downcast<const TextAreaRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextAreaRenderer* TextArea::getRenderer()
    {
        return aurora::downcast<TextAreaRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        updateScrollbars();

        // The size of the text area has changed, update the text
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setText(String text)
    {
        // Don't allow the text to contain carriage returns
        text.replace('\r', U"");

        // Remove all the excess characters when a character limit is set
        if ((m_maxChars > 0) && (text.length() > m_maxChars))
            m_text = text.toUtf32().substr(0, m_maxChars);
        else
            m_text = text.toUtf32();

        rearrangeText(false);

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::addText(String text)
    {
        setText(m_text + std::move(text));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TextArea::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setDefaultText(const String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& TextArea::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setSelectedText(std::size_t selectionStartIndex, std::size_t selectionEndIndex)
    {
        const auto oldSelEnd = m_selEnd;
        setCaretPositionImpl(selectionEndIndex, true, false);    // Replace m_selEnd but do not emit onCaretPositionChange yet.
        setCaretPositionImpl(selectionStartIndex, false, false); // Retain m_selEnd.
        if (oldSelEnd != m_selEnd)
            onCaretPositionChange.emit(this);                    // Emit onCaretPositionChange now.
        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TextArea::getSelectedText() const
    {
        const std::size_t selStart = getSelectionStart();
        const std::size_t selEnd = getSelectionEnd();
        if (selStart <= selEnd)
            return m_text.substr(selStart, selEnd - selStart);
        else
            return m_text.substr(selEnd, selStart - selEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getSelectionStart() const
    {
        return getIndexOfSelectionPos(m_selStart);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getSelectionEnd() const
    {
        return getIndexOfSelectionPos(m_selEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::updateTextSize()
    {
        if (m_textSizeCached < 1)
            m_textSizeCached = 1;

        // Change the text size
        m_textBeforeSelection.setCharacterSize(m_textSizeCached);
        m_textSelection1.setCharacterSize(m_textSizeCached);
        m_textSelection2.setCharacterSize(m_textSizeCached);
        m_textAfterSelection1.setCharacterSize(m_textSizeCached);
        m_textAfterSelection2.setCharacterSize(m_textSizeCached);
        m_defaultText.setCharacterSize(m_textSizeCached);

        // Calculate the height of one line
        m_lineHeight = m_fontCached.getLineSpacing(m_textSizeCached);

        m_verticalScrollbar->setScrollAmount(static_cast<unsigned int>(m_lineHeight));
        m_horizontalScrollbar->setScrollAmount(m_textSizeCached);

        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setMaximumCharacters(std::size_t maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_text.length() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, String::npos);
            rearrangeText(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setTabString(String tabText)
    {
        // Do not allow evil cartridge returns.
        m_tabText = tabText.replace('\r', U"");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TextArea::getTabString() const
    {
        return m_tabText;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setVerticalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy TextArea::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setHorizontalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy TextArea::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        setCaretPositionImpl(charactersBeforeCaret, true, true); // Update m_selEnd and emit onCaretPositionChange.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getCaretPosition() const
    {
        return getSelectionEnd();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getCaretLine() const
    {
        const auto caret = getCaretPosition();
        if (caret == 0)
            return 1;
        else
            return m_text.substr(0, caret).count('\n') + 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getCaretColumn() const
    {
        const auto caret = getCaretPosition();
        if (caret == 0)
            return 1;
        auto lineStart = m_text.rfind('\n', caret - 1);
        if (lineStart == String::npos)
            return caret + 1;
        else
            return caret - lineStart;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextArea::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getLinesCount() const
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setFocused(bool focused)
    {
        const Vector2f caretPosition = {m_caretPosition.x + m_bordersCached.getLeft() + m_paddingCached.getLeft() - static_cast<float>(m_horizontalScrollbar->getValue()),
                                        m_caretPosition.y + m_bordersCached.getTop() + m_paddingCached.getTop() - static_cast<float>(m_verticalScrollbar->getValue())};

        // The SDL backend will positions the IME candidate window outside the rectangle, so if we pass the entire
        // size of the text area then it could appear far from the cursor. We want the window to be below the line
        // that our cursor is on. For some reason the IME window is only placed at the correct location after the
        // second character is typed, when the first character is typed the location is always at the top position
        // of the rectangle instead of below the rectangle. So we choose the top of the rectangle as the top of the line
        // instead of the top of the widget, so that distance between the initial and final window locations isn't huge.
        // Other backends (SFML and GLFW) currently ignore the rectangle.
        const auto absoluteLineTopPos = getAbsolutePosition({ 0, caretPosition.y });
        const float caretHeight = std::max(m_fontCached.getFontHeight(m_textSizeCached), m_lineHeight);
        const FloatRect inputRect = {absoluteLineTopPos, getAbsolutePosition({getSize().x, caretPosition.y + caretHeight}) - absoluteLineTopPos};

        if (m_parentGui)
        {
            if (focused)
                m_parentGui->startTextInput(inputRect);
            else
                m_parentGui->stopTextInput();
        }

        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};

            if (m_parentGui)
                m_parentGui->updateTextCursorPosition(inputRect, getAbsolutePosition({caretPosition.x + m_caretWidthCached, caretPosition.y}));
        }
        else // Unfocusing
        {
            // If there is a selection then undo it now
            if (m_selStart != m_selEnd)
            {
                m_selStart = m_selEnd;
                updateSelectionTexts();
            }
        }

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::enableMonospacedFontOptimization(bool enable)
    {
        m_monospacedFontOptimizationEnabled = enable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextArea::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextArea::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextArea::isMouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isSet() || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextArea::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);

        pos -= getPosition();

        bool isDragging = true; // User may start selecting text

        // If there is a scrollbar then pass the event
        if ((m_verticalScrollbar->isShown()) && (m_verticalScrollbar->isMouseOnWidget(pos)))
        {
            isDragging = m_verticalScrollbar->leftMousePressed(pos);
            recalculateVisibleLines();
        }
        else if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->isMouseOnWidget(pos))
        {
            isDragging = m_horizontalScrollbar->leftMousePressed(pos);
        }
        else // The click occurred on the text area
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return false;

            const auto caretPosition = findCaretPosition(pos);
            const auto oldSelEnd = m_selEnd;

            // Check if this is a double click
            if ((m_possibleDoubleClick) && (m_selStart == m_selEnd) && (caretPosition == m_selEnd))
            {
                // The next click is going to be a normal one again
                m_possibleDoubleClick = false;

                // If the click was to the right of the end of line then make sure to select the word on the left
                if (m_lines[m_selStart.y].length() > 1 && (m_selStart.x == (m_lines[m_selStart.y].length()-1) || m_selStart.x == m_lines[m_selStart.y].length()))
                {
                    m_selStart.x--;
                    m_selEnd.x = m_selStart.x;
                }

                bool selectingWhitespace;
                if (isWhitespace(m_lines[m_selStart.y][m_selStart.x]))
                    selectingWhitespace = true;
                else
                    selectingWhitespace = false;

                // Move start pointer to the beginning of the word/whitespace
                for (std::size_t i = m_selStart.x; i > 0; --i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selStart.y][i-1]))
                    {
                        m_selStart.x = i;
                        break;
                    }
                    else
                        m_selStart.x = 0;
                }

                // Move end pointer to the end of the word/whitespace
                for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].length(); ++i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selEnd.y][i]))
                    {
                        m_selEnd.x = i;
                        break;
                    }
                    else
                        m_selEnd.x = m_lines[m_selEnd.y].length();
                }
            }
            else // No double clicking
            {
                if (!keyboard::isShiftPressed())
                    m_selStart = caretPosition;

                m_selEnd = caretPosition;

                // If the next click comes soon enough then it will be a double click
                m_possibleDoubleClick = true;
            }

            // If the caret position changed, emit signal.
            if (oldSelEnd != m_selEnd)
                onCaretPositionChange.emit(this);

            // Update the texts
            updateSelectionTexts();

            // The caret should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::leftMouseReleased(Vector2f pos)
    {
        // If there is a scrollbar then pass it the event
        if (m_verticalScrollbar->isShown())
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_verticalScrollbar->isMouseDown())
            {
                m_verticalScrollbar->leftMouseReleased(pos - getPosition());
                recalculateVisibleLines();
            }
        }

        if (m_horizontalScrollbar->isShown())
        {
            if (m_horizontalScrollbar->isMouseDown())
                m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse event should go to the vertical scrollbar
        if (m_verticalScrollbar->isShown() && ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->isMouseOnWidget(pos)))
        {
            m_verticalScrollbar->mouseMoved(pos);
            recalculateVisibleLines();
        }

        // Check if the mouse event should go to the horizontal scrollbar
        else if (m_horizontalScrollbar->isShown() && ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->isMouseOnWidget(pos)))
        {
            m_horizontalScrollbar->mouseMoved(pos);
        }

        // If the mouse is held down then you are selecting text
        else if (m_mouseDown)
        {
            auto caretPosition = findCaretPosition(pos);
            const auto oldSelEnd = m_selEnd;

            if (caretPosition != m_selEnd)
            {
                m_selEnd = caretPosition;
                updateSelectionTexts();
            }

            // Check if the caret is located above or below the view
            if (m_verticalScrollbar->isShown())
            {
                if (m_selEnd.y <= m_topLine)
                    m_verticalScrollbar->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
                else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                    m_verticalScrollbar->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight) - m_verticalScrollbar->getViewportSize()));

                recalculateVisibleLines();
            }

            if (oldSelEnd != m_selEnd)
                onCaretPositionChange.emit(this);
        }

        // Inform the scrollbars that the mouse is not on them
        else
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->mouseNoLongerOnWidget();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();

        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->leftMouseButtonNoLongerDown();

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->leftMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::keyPressed(const Event::KeyEvent& event)
    {
        if (event.code == Event::KeyboardKey::Tab)
            insertTextAtCaretPosition(m_tabText);
        else if (event.code == Event::KeyboardKey::Enter)
            textEntered('\n');
        else if (event.code == Event::KeyboardKey::Backspace)
            backspaceKeyPressed();
        else if (event.code == Event::KeyboardKey::Delete)
            deleteKeyPressed();
        else if (keyboard::isKeyPressCopy(event))
            copySelectedTextToClipboard();
        else if (keyboard::isKeyPressCut(event))
            cutSelectedTextToClipboard();
        else if (keyboard::isKeyPressPaste(event))
            pasteTextFromClipboard();
        else if (keyboard::isKeyPressSelectAll(event))
            selectAllText();
        else
        {
            bool caretMoved = true;
            if (event.code == Event::KeyboardKey::PageUp)
                moveCaretPageUp();
            else if (event.code == Event::KeyboardKey::PageDown)
                moveCaretPageDown();
            else if (keyboard::isKeyPressMoveCaretLeft(event))
                moveCaretLeft(event.shift);
            else if (keyboard::isKeyPressMoveCaretRight(event))
                moveCaretRight(event.shift);
            else if (keyboard::isKeyPressMoveCaretWordBegin(event))
                moveCaretWordBegin();
            else if (keyboard::isKeyPressMoveCaretWordEnd(event))
                moveCaretWordEnd();
            else if (keyboard::isKeyPressMoveCaretUp(event))
                updateSelEnd(findCaretPosition({m_caretPosition.x, m_caretPosition.y - (m_lineHeight / 2.f) - m_verticalScrollbar->getValue()}));
            else if (keyboard::isKeyPressMoveCaretDown(event))
                updateSelEnd(findCaretPosition({m_caretPosition.x, m_caretPosition.y + (m_lineHeight * 1.5f) - m_verticalScrollbar->getValue()}));
            else if (keyboard::isKeyPressMoveCaretLineStart(event))
                updateSelEnd({0, m_selEnd.y});
            else if (keyboard::isKeyPressMoveCaretLineEnd(event))
                updateSelEnd({ m_lines[m_selEnd.y].length(), m_selEnd.y });
            else if (keyboard::isKeyPressMoveCaretDocumentBegin(event))
                updateSelEnd({0, 0});
            else if (keyboard::isKeyPressMoveCaretDocumentEnd(event))
                updateSelEnd({m_lines[m_lines.size()-1].length(), m_lines.size()-1});
            else
                caretMoved = false;

            if (caretMoved)
            {
                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextArea::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Tab)
         || (event.code == Event::KeyboardKey::Enter)
         || (event.code == Event::KeyboardKey::Backspace)
         || (event.code == Event::KeyboardKey::Delete)
         || (event.code == Event::KeyboardKey::PageUp)
         || (event.code == Event::KeyboardKey::PageDown)
         || (keyboard::isKeyPressCopy(event))
         || (keyboard::isKeyPressCut(event))
         || (keyboard::isKeyPressPaste(event))
         || (keyboard::isKeyPressSelectAll(event))
         || (keyboard::isKeyPressMoveCaretLeft(event))
         || (keyboard::isKeyPressMoveCaretRight(event))
         || (keyboard::isKeyPressMoveCaretWordBegin(event))
         || (keyboard::isKeyPressMoveCaretWordEnd(event))
         || keyboard::isKeyPressMoveCaretLineStart(event)
         || keyboard::isKeyPressMoveCaretUp(event)
         || keyboard::isKeyPressMoveCaretDown(event)
         || keyboard::isKeyPressMoveCaretDocumentBegin(event)
         || keyboard::isKeyPressMoveCaretLineEnd(event)
         || keyboard::isKeyPressMoveCaretDocumentEnd(event))
        {
            return true;
        }
        else
            return Widget::canHandleKeyPress(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::textEntered(char32_t key)
    {
        if (m_readOnly)
            return;

        // Don't allow carriage return characters, they only cause trouble
        if (key == '\r')
            return;

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.length() + 1 > m_maxChars))
            return;

        auto insert = TGUI_LAMBDA_CAPTURE_EQ_THIS()
        {
            deleteSelectedCharacters();

            const std::size_t caretPosition = getSelectionEnd();

            m_text.insert(caretPosition, 1, key);
            m_lines[m_selEnd.y].insert(m_selEnd.x, 1, key);

            // Increment the caret position, unless you type a newline at the start of a line while that line only existed due to word wrapping
            if ((key != U'\n') || (m_selEnd.x > 0) || (m_selEnd.y == 0) || m_lines[m_selEnd.y-1].empty() || (m_text[caretPosition-1] == U'\n'))
            {
                m_selStart.x++;
                m_selEnd.x++;
            }

            // Do not emit onCaretPositionChanged signal yet, as it could be invalid.
            rearrangeText(true, false);
        };

        // If there is a scrollbar then inserting can't go wrong
        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Never)
        {
            const auto oldSelEnd = m_selEnd;
            insert();
            if (oldSelEnd != m_selEnd)
                onCaretPositionChange.emit(this);
        }
        else // There is no scrollbar, the text may not fit
        {
            // Store the data so that it can be reverted
            const auto oldText = m_text;
            const auto oldSelStart = m_selStart;
            const auto oldSelEnd = m_selEnd;

            // Try to insert the character
            insert();

            // Undo the insert if the text does not fit
            if (m_lines.size() > static_cast<std::size_t>(getInnerSize().y / m_lineHeight))
            {
                m_text = oldText;
                m_selStart = oldSelStart;
                m_selEnd = oldSelEnd;

                rearrangeText(true);
            }
            // If the caret position changed, emit signal.
            else if (oldSelEnd != m_selEnd)
            {
                onCaretPositionChange.emit(this);
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextArea::scrolled(float delta, Vector2f pos, bool touch)
    {
        bool scrollbarMoved = false;
        if (m_horizontalScrollbar->isShown()
         && !touch
         && (!m_verticalScrollbar->isShown() || m_horizontalScrollbar->isMouseOnWidget(pos - getPosition()) || keyboard::isShiftPressed()))
        {
            scrollbarMoved = m_horizontalScrollbar->scrolled(delta, pos - getPosition(), touch);
        }
        else if (m_verticalScrollbar->isShown())
        {
            scrollbarMoved = m_verticalScrollbar->scrolled(delta, pos - getPosition(), touch);
        }

        if (scrollbarMoved)
            recalculateVisibleLines();

        return scrollbarMoved;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2<std::size_t> TextArea::findCaretPosition(Vector2f position) const
    {
        position.x -= m_bordersCached.getLeft() + m_paddingCached.getLeft();
        position.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return {m_lines[m_lines.size()-1].size(), m_lines.size()-1};

        // Find on which line the mouse is
        std::size_t lineNumber;
        if (m_verticalScrollbar->isShown())
        {
            if (position.y + m_verticalScrollbar->getValue() < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor((position.y + m_verticalScrollbar->getValue()) / m_lineHeight));
        }
        else
        {
            if (position.y < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor(position.y / m_lineHeight));
        }

        // Check if you clicked behind everything
        if (lineNumber + 1 > m_lines.size())
            return {m_lines[m_lines.size()-1].size(), m_lines.size()-1};

        // Find between which character the mouse is standing
        float width = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached) - m_horizontalScrollbar->getValue();
        char32_t prevChar = 0;
        for (std::size_t i = 0; i < m_lines[lineNumber].size(); ++i)
        {
            float charWidth;
            const char32_t curChar = m_lines[lineNumber][i];
            //if (curChar == U'\n')
            //    return Vector2<std::size_t>(m_lines[lineNumber].getSize() - 1, lineNumber); // TextArea strips newlines but this code is kept for when this function is generalized
            //else
            if (curChar == U'\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', getTextSize(), false).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, getTextSize(), false).advance);

            const float kerning = m_fontCached.getKerning(prevChar, curChar, getTextSize(), false);
            if (width + charWidth + kerning <= position.x)
                width += charWidth + kerning;
            else
            {
                if (position.x < width + kerning + (charWidth / 2.0f))
                    return {i, lineNumber};
                else
                    return {i + 1, lineNumber};
            }

            prevChar = curChar;
        }

        // You clicked behind the last character
        return {m_lines[lineNumber].length(), lineNumber};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextArea::getIndexOfSelectionPos(Vector2<std::size_t> selectionPos) const
    {
        auto findIndex = [this](std::size_t line)
        {
            std::size_t counter = 0;
            for (std::size_t i = 0; i < line; ++i)
            {
                counter += m_lines[i].length();
                if ((counter < m_text.length()) && (m_text[counter] == U'\n'))
                    counter += 1;
            }

            return counter;
        };

        return findIndex(selectionPos.y) + selectionPos.x;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::deleteSelectedCharacters()
    {
        if (m_selStart != m_selEnd)
        {
            const std::size_t selStart = getSelectionStart();
            const std::size_t selEnd = getSelectionEnd();
            if (selStart <= selEnd)
            {
                m_text.erase(selStart, selEnd - selStart);
                updateSelEnd(m_selStart);
            }
            else
            {
                m_text.erase(selEnd, selStart - selEnd);
                m_selStart = m_selEnd;
            }

            rearrangeText(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::insertTextAtCaretPosition(String text)
    {
        if (text.empty())
            return;

        // If given string exceeds character limit, remove characters from the end until it fits.
        const auto combinedSize = m_text.size() + text.size();
        if (m_maxChars != 0 && combinedSize > m_maxChars)
            text = text.substr(0, text.size() - (combinedSize - m_maxChars));

        if (text.empty())
            return;

        // Insert string.
        m_text.insert(getSelectionEnd(), text);
        m_lines[m_selEnd.y].insert(m_selEnd.x, text);

        m_selEnd.x += text.length();
        m_selStart = m_selEnd;
        rearrangeText(true, false);
        onCaretPositionChange.emit(this);

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::moveCaretLeft(bool shiftPressed)
    {
        if ((m_selStart != m_selEnd) && !shiftPressed)
        {
            if ((m_selStart.y < m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x < m_selEnd.x)))
                updateSelEnd(m_selStart);
            else
                m_selStart = m_selEnd;
        }
        else
        {
            if (m_selEnd.x > 0)
            {
                m_selEnd.x--;
                onCaretPositionChange.emit(this);
            }
            else
            {
                // You are at the left side of a line so move up
                if (m_selEnd.y > 0)
                {
                    m_selEnd.y--;
                    m_selEnd.x = m_lines[m_selEnd.y].length();
                    onCaretPositionChange.emit(this);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::moveCaretRight(bool shiftPressed)
    {
        if ((m_selStart != m_selEnd) && !shiftPressed)
        {
            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                updateSelEnd(m_selStart);
        }
        else
        {
            // Move to the next line if you are at the end of the line
            if (m_selEnd.x == m_lines[m_selEnd.y].length())
            {
                if (m_selEnd.y + 1 < m_lines.size())
                {
                    m_selEnd.y++;
                    m_selEnd.x = 0;
                    onCaretPositionChange.emit(this);
                }
            }
            else
            {
                m_selEnd.x++;
                onCaretPositionChange.emit(this);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::moveCaretWordBegin()
    {
        // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
        bool skippedWhitespace = false;
        bool done = false;
        const auto oldSelEnd = m_selEnd;
        for (std::size_t j = m_selEnd.y + 1; j > 0; --j)
        {
            for (std::size_t i = m_selEnd.x; i > 0; --i)
            {
                if (skippedWhitespace)
                {
                    if (isWhitespace(m_lines[m_selEnd.y][i-1]))
                    {
                        m_selEnd.x = i;
                        done = true;
                        break;
                    }
                }
                else
                {
                    if (!isWhitespace(m_lines[m_selEnd.y][i-1]))
                        skippedWhitespace = true;
                }
            }

            if (!done)
            {
                if (!skippedWhitespace)
                {
                    if (m_selEnd.y > 0)
                    {
                        m_selEnd.y--;
                        m_selEnd.x = m_lines[m_selEnd.y].length();
                    }
                }
                else
                {
                    m_selEnd.x = 0;
                    break;
                }
            }
            else
                break;
        }
        if (oldSelEnd != m_selEnd)
            onCaretPositionChange.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::moveCaretWordEnd()
    {
        // Move to the end of the word (or to the end of the next word when already at the end)
        bool skippedWhitespace = false;
        bool done = false;
        const auto oldSelEnd = m_selEnd;
        for (std::size_t j = m_selEnd.y; j < m_lines.size(); ++j)
        {
            for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].length(); ++i)
            {
                if (skippedWhitespace)
                {
                    if (isWhitespace(m_lines[m_selEnd.y][i]))
                    {
                        m_selEnd.x = i;
                        done = true;
                        break;
                    }
                }
                else
                {
                    if (!isWhitespace(m_lines[m_selEnd.y][i]))
                        skippedWhitespace = true;
                }
            }

            if (!done)
            {
                if (!skippedWhitespace)
                {
                    if (m_selEnd.y + 1 < m_lines.size())
                    {
                        m_selEnd.y++;
                        m_selEnd.x = 0;
                    }
                }
                else
                {
                    m_selEnd.x = m_lines[m_selEnd.y].length();
                    break;
                }
            }
            else
                break;
        }
        if (oldSelEnd != m_selEnd)
            onCaretPositionChange.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::moveCaretPageUp()
    {
        const auto oldSelEnd = m_selEnd;
        // Move to the top line when not there already
        if (m_selEnd.y != m_topLine)
            m_selEnd.y = m_topLine;
        else
        {
            // Scroll up when we already where at the top line
            const auto visibleLines = static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight);
            if (m_topLine < visibleLines - 1)
                m_selEnd.y = 0;
            else
                m_selEnd.y = m_topLine - visibleLines + 1;
        }

        m_selEnd.x = 0;
        if (oldSelEnd != m_selEnd)
            onCaretPositionChange.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::moveCaretPageDown()
    {
        const auto oldSelEnd = m_selEnd;
        // Move to the bottom line when not there already
        if (m_topLine + m_visibleLines > m_lines.size())
            m_selEnd.y = m_lines.size() - 1;
        else if (m_selEnd.y != m_topLine + m_visibleLines - 1)
            m_selEnd.y = m_topLine + m_visibleLines - 1;
        else
        {
            // Scroll down when we already where at the bottom line
            const auto visibleLines = static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight);
            if (m_selEnd.y + visibleLines >= m_lines.size() + 2)
                m_selEnd.y = m_lines.size() - 1;
            else
                m_selEnd.y = m_selEnd.y + visibleLines - 2;
        }

        m_selEnd.x = m_lines[m_selEnd.y].length();
        if (oldSelEnd != m_selEnd)
            onCaretPositionChange.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::backspaceKeyPressed()
    {
        if (m_readOnly)
            return;

        // Check that we did not select any characters
        if (m_selStart == m_selEnd)
        {
            const std::size_t pos = getSelectionEnd();
            if (pos > 0)
            {
                if (m_selEnd.x > 0)
                {
                    // There is a specific case that we have to watch out for. When we are removing the last character on
                    // a line which was placed there by word wrap and a newline follows this character then the caret
                    // has to be placed at the line above (before the newline) instead of at the same line (after the newline)
                    if ((m_lines[m_selEnd.y].length() == 1) && (pos > 1) && (pos < m_text.length()) && (m_text[pos-2] != '\n') && (m_text[pos] == '\n') && (m_selEnd.y > 0))
                    {
                        m_selEnd.y--;
                        m_selEnd.x = m_lines[m_selEnd.y].length();
                    }
                    else // Just remove the character normally
                        --m_selEnd.x;
                }
                else // At the beginning of the line
                {
                    if (m_selEnd.y > 0)
                    {
                        --m_selEnd.y;
                        m_selEnd.x = m_lines[m_selEnd.y].length();

                        if ((m_text[pos-1] != '\n') && m_selEnd.x > 0)
                            --m_selEnd.x;
                    }
                }

                m_selStart = m_selEnd;

                m_text.erase(pos - 1, 1);
                // If the "special case" above passes, and we let rearrangeText() emit the onCaretPositionChange signal,
                // the same signal will be emitted twice. So prevent sending signal in rearrangeText() and always send it manually afterwards.
                rearrangeText(true, false);
                onCaretPositionChange.emit(this);
            }
        }
        else // When you did select some characters then delete them
            deleteSelectedCharacters();

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::deleteKeyPressed()
    {
        if (m_readOnly)
            return;

        // Check that we did not select any characters
        if (m_selStart == m_selEnd)
        {
            m_text.erase(getSelectionEnd(), 1);
            rearrangeText(true);
        }
        else // You did select some characters, so remove them
            deleteSelectedCharacters();

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::copySelectedTextToClipboard()
    {
        const std::size_t selStart = getSelectionStart();
        const std::size_t selEnd = getSelectionEnd();
        if (selStart <= selEnd)
            getBackend()->setClipboard(m_text.substr(selStart, selEnd - selStart));
        else
            getBackend()->setClipboard(m_text.substr(selEnd, selStart - selEnd));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::cutSelectedTextToClipboard()
    {
        copySelectedTextToClipboard();

        if (!m_readOnly)
            deleteSelectedCharacters();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::pasteTextFromClipboard()
    {
        if (m_readOnly)
            return;

        String clipboardContents = getBackend()->getClipboard();

        // Remove carriage returns from the string, as they cause issues
        clipboardContents.replace('\r', U"");

        // Only continue pasting if you actually have to do something
        if ((m_selStart != m_selEnd) || (clipboardContents != U""))
        {
            deleteSelectedCharacters();
            insertTextAtCaretPosition(clipboardContents);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::selectAllText()
    {
        m_selStart = {0, 0};
        updateSelEnd(Vector2<std::size_t>(m_lines[m_lines.size()-1].length(), m_lines.size()-1));
        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::rearrangeText(bool keepSelection, const bool emitCaretChangedPosition)
    {
        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return;

        String string;
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
            string = m_text;
        else
        {
            // Find the maximum width of one line
            const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);
            float maxLineWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2 * textOffset;
            if (m_verticalScrollbar->isShown())
                maxLineWidth -= m_verticalScrollbar->getSize().x;

            // Don't do anything when there is no room for the text
            if (maxLineWidth <= 0)
                return;

            string = Text::wordWrap(maxLineWidth, m_text, m_fontCached, m_textSizeCached, false);
        }

        // Store the current selection position when we are keeping the selection
        const std::size_t selStart = keepSelection ? getSelectionStart() : 0;
        const std::size_t selEnd = keepSelection ? getSelectionEnd() : 0;

        // Split the string in multiple lines
        m_maxLineWidth = 0;
        m_lines.clear();
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        std::size_t longestLineCharCount = 0;
        std::size_t longestLineIndex = 0;
        while (newLinePos != String::npos)
        {
            newLinePos = string.find('\n', searchPosStart);

            if (newLinePos != String::npos)
                m_lines.push_back(string.substr(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.push_back(string.substr(searchPosStart));

            if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
            {
                if (m_monospacedFontOptimizationEnabled)
                {
                    if (m_lines.back().length() > longestLineCharCount)
                    {
                        longestLineCharCount = m_lines.back().length();
                        longestLineIndex = m_lines.size() - 1;
                    }
                }
                else // Not using optimization for monospaced font, so really calculate the width of every line
                {
                    float lineWidth = Text::getLineWidth(m_lines.back(), m_fontCached, m_textSizeCached);
                    if (lineWidth > m_maxLineWidth)
                        m_maxLineWidth = lineWidth;
                }
            }

            searchPosStart = newLinePos + 1;
        }

        if ((m_horizontalScrollbarPolicy != Scrollbar::Policy::Never) && m_monospacedFontOptimizationEnabled)
            m_maxLineWidth = Text::getLineWidth(m_lines[longestLineIndex], m_fontCached, m_textSizeCached);

        // Check if we should try to keep our selection
        if (keepSelection)
        {
            std::size_t index = 0;
            Vector2<std::size_t> newSelStart;
            Vector2<std::size_t> newSelEnd;
            bool newSelStartFound = false;
            bool newSelEndFound = false;

            // Look for the new locations of our selection
            for (std::size_t i = 0; i < m_lines.size(); ++i)
            {
                index += m_lines[i].length();

                if (!newSelStartFound && (index >= selStart))
                {
                    newSelStart = Vector2<std::size_t>(m_lines[i].length() - (index - selStart), i);

                    newSelStartFound = true;
                    if (newSelEndFound)
                        break;
                }

                if (!newSelEndFound && (index >= selEnd))
                {
                    newSelEnd = Vector2<std::size_t>(m_lines[i].length() - (index - selEnd), i);

                    newSelEndFound = true;
                    if (newSelStartFound)
                        break;
                }

                // Skip newlines in the text
                if (m_text[index] == U'\n')
                    ++index;
            }

            // Keep the selection when possible
            if (newSelStartFound && newSelEndFound)
            {
                m_selStart = newSelStart;
                if (emitCaretChangedPosition)
                    updateSelEnd(newSelEnd);
                else
                    m_selEnd = newSelEnd;
            }
            else // The text has changed too much, the selection can't be kept
            {
                m_selStart = Vector2<std::size_t>(m_lines[m_lines.size()-1].length(), m_lines.size()-1);
                if (emitCaretChangedPosition)
                    updateSelEnd(m_selStart);
                else
                    m_selEnd = m_selStart;
            }
        }
        else // Set the caret at the back of the text
        {
            m_selStart = Vector2<std::size_t>(m_lines[m_lines.size()-1].length(), m_lines.size()-1);
            if (emitCaretChangedPosition)
                updateSelEnd(m_selStart);
            else
                m_selEnd = m_selStart;
        }

        updateScrollbars();

        // Tell the scrollbars how many pixels the text contains
        const bool verticalScrollbarShown = m_verticalScrollbar->isShown();
        const bool horizontalScrollbarShown = m_horizontalScrollbar->isShown();

        m_verticalScrollbar->setMaximum(static_cast<unsigned int>((m_lines.size() - 1) * m_lineHeight
                                                                  + std::max(m_fontCached.getFontHeight(m_textSizeCached), m_lineHeight)
                                                                  + Text::getExtraVerticalPadding(m_textSizeCached)));

        m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_maxLineWidth
                                                                    + Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached) * 2));

        if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
        {
            // Word-wrap will have to be done again if the vertical scrollbar just appeared or disappeared
            if (verticalScrollbarShown != m_verticalScrollbar->isShown())
            {
                rearrangeText(true);
                return;
            }
            else
                updateScrollbars();
        }
        else // Horizontal scrollbar is enabled
        {
            if ((verticalScrollbarShown != m_verticalScrollbar->isShown()) || (horizontalScrollbarShown != m_horizontalScrollbar->isShown()))
                updateScrollbars();
        }

        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::updateScrollbars()
    {
        if (m_horizontalScrollbar->isShown())
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, getInnerSize().y - m_horizontalScrollbar->getSize().y});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_horizontalScrollbar->getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, getInnerSize().y});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
        }

        if (m_verticalScrollbar->isShown())
        {
            m_horizontalScrollbar->setSize({getInnerSize().x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_verticalScrollbar->getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
        else
        {
            m_horizontalScrollbar->setSize({getInnerSize().x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::updateSelectionTexts()
    {
        // If there is no selection then just put the whole text in m_textBeforeSelection
        if (m_selStart == m_selEnd)
        {
            String displayedText;
            for (const auto& line : m_lines)
                displayedText += line + U"\n";

            m_textBeforeSelection.setString(displayedText);
            m_textSelection1.setString(U"");
            m_textSelection2.setString(U"");
            m_textAfterSelection1.setString(U"");
            m_textAfterSelection2.setString(U"");
        }
        else // Some text is selected
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            // Set the text before the selection
            if (selectionStart.y > 0)
            {
                String string;
                for (std::size_t i = 0; i < selectionStart.y; ++i)
                    string += m_lines[i] + U"\n";

                string += m_lines[selectionStart.y].substr(0, selectionStart.x);
                m_textBeforeSelection.setString(string);
            }
            else
                m_textBeforeSelection.setString(m_lines[0].substr(0, selectionStart.x));

            // Set the selected text
            if (m_selStart.y == m_selEnd.y)
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substr(selectionStart.x, selectionEnd.x - selectionStart.x));
                m_textSelection2.setString(U"");
            }
            else
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substr(selectionStart.x, m_lines[selectionStart.y].length() - selectionStart.x));

                String string;
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                    string += m_lines[i] + U"\n";

                string += m_lines[selectionEnd.y].substr(0, selectionEnd.x);

                m_textSelection2.setString(string);
            }

            // Set the text after the selection
            {
                m_textAfterSelection1.setString(m_lines[selectionEnd.y].substr(selectionEnd.x, m_lines[selectionEnd.y].length() - selectionEnd.x));

                String string;
                for (std::size_t i = selectionEnd.y + 1; i < m_lines.size(); ++i)
                    string += m_lines[i] + U"\n";

                m_textAfterSelection2.setString(string);
            }
        }

        // Check if the caret is located above or below the view
        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Never)
        {
            if (m_selEnd.y <= m_topLine)
                m_verticalScrollbar->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
            else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                m_verticalScrollbar->setValue(static_cast<unsigned int>((m_selEnd.y * m_lineHeight)
                                                                        + std::max(m_fontCached.getFontHeight(m_textSizeCached), m_lineHeight)
                                                                        + Text::getExtraVerticalPadding(m_textSizeCached)
                                                                        - m_verticalScrollbar->getViewportSize()));
        }

        // Position the caret
        {
            /// TODO: Implement a way to calculate text size without creating a text object?
            const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);
            Text tempText;
            tempText.setFont(m_fontCached);
            tempText.setCharacterSize(getTextSize());
            tempText.setString(m_lines[m_selEnd.y].substr(0, m_selEnd.x));

            float kerning = 0;
            if ((m_selEnd.x > 0) && (m_selEnd.x < m_lines[m_selEnd.y].length()))
                kerning = m_fontCached.getKerning(m_lines[m_selEnd.y][m_selEnd.x - 1], m_lines[m_selEnd.y][m_selEnd.x], m_textSizeCached, false);

            const float caretLeft = textOffset + tempText.findCharacterPos(tempText.getString().length()).x + kerning;
            const float caretTop = static_cast<float>(m_selEnd.y) * m_lineHeight;
            m_caretPosition = {caretLeft, caretTop};
        }

        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
        {
            const unsigned int left = m_horizontalScrollbar->getValue();
            if (m_caretPosition.x <= left)
            {
                unsigned int newValue =
                    static_cast<unsigned int>(std::max(0, static_cast<int>(m_caretPosition.x
                                                                           - (Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached) * 2))));
                m_horizontalScrollbar->setValue(newValue);
            }
            else if (m_caretPosition.x > (left + m_horizontalScrollbar->getViewportSize()))
            {
                unsigned int newValue = static_cast<unsigned int>(m_caretPosition.x
                                                                  + (Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached) * 2)
                                                                  - m_horizontalScrollbar->getViewportSize());
                m_horizontalScrollbar->setValue(newValue);
            }
        }

        recalculatePositions();

        // Send an event when the selection changed
        if ((m_selStart != m_lastSelection.first) || (m_selEnd != m_lastSelection.second))
        {
            // Only send the event when there is an actual change, not when the caret position moved
            if ((m_selStart != m_selEnd) || (m_lastSelection.first != m_lastSelection.second))
                onSelectionChange.emit(this);

            m_lastSelection.first = m_selStart;
            m_lastSelection.second = m_selEnd;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TextArea::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextArea::updateTime(Duration elapsedTime)
    {
        bool screenRefreshRequired = Widget::updateTime(elapsedTime);

        // Only show/hide the caret every half second
        if (m_animationTimeElapsed >= getEditCursorBlinkRate())
        {
            // Reset the elapsed time
            m_animationTimeElapsed = {};

            // Switch the value of the visible flag
            m_caretVisible = !m_caretVisible;

            // Too slow for double clicking
            m_possibleDoubleClick = false;

            screenRefreshRequired = true;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::recalculatePositions()
    {
        if (!m_fontCached)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);

        // Calculate the position of the text objects
        m_selectionRects.clear();
        m_textBeforeSelection.setPosition({textOffset, 0});
        m_defaultText.setPosition({textOffset, 0});

        if (m_selStart != m_selEnd)
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            float kerningSelectionStart = 0;
            if ((selectionStart.x > 0) && (selectionStart.x < m_lines[selectionStart.y].length()))
                kerningSelectionStart = m_fontCached.getKerning(m_lines[selectionStart.y][selectionStart.x-1], m_lines[selectionStart.y][selectionStart.x], m_textSizeCached, false);

            float kerningSelectionEnd = 0;
            if ((selectionEnd.x > 0) && (selectionEnd.x < m_lines[selectionEnd.y].length()))
                kerningSelectionEnd = m_fontCached.getKerning(m_lines[selectionEnd.y][selectionEnd.x-1], m_lines[selectionEnd.y][selectionEnd.x], m_textSizeCached, false);

            if (selectionStart.x > 0)
            {
                m_textSelection1.setPosition({textOffset + m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().length()).x + kerningSelectionStart,
                                              m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});
            }
            else
                m_textSelection1.setPosition({textOffset, m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});

            m_textSelection2.setPosition({textOffset, static_cast<float>((selectionStart.y + 1) * m_lineHeight)});

            if (!m_textSelection2.getString().empty() || (selectionEnd.x == 0))
            {
                m_textAfterSelection1.setPosition({textOffset + m_textSelection2.findCharacterPos(m_textSelection2.getString().length()).x + kerningSelectionEnd,
                                                   m_textSelection2.getPosition().y + ((selectionEnd.y - selectionStart.y - 1) * m_lineHeight)});
            }
            else
                m_textAfterSelection1.setPosition({m_textSelection1.getPosition().x + m_textSelection1.findCharacterPos(m_textSelection1.getString().length()).x + kerningSelectionEnd,
                                                   m_textSelection1.getPosition().y});

            m_textAfterSelection2.setPosition({textOffset, static_cast<float>(selectionEnd.y + 1) * m_lineHeight});

            // Recalculate the selection rectangles
            {
                m_selectionRects.emplace_back(m_textSelection1.getPosition().x, static_cast<float>(selectionStart.y) * m_lineHeight, 0.f, m_lineHeight);

                if (!m_lines[selectionStart.y].empty())
                {
                    m_selectionRects.back().width = m_textSelection1.findCharacterPos(m_textSelection1.getString().length()).x;

                    // There is kerning when the selection is on just this line
                    if (selectionStart.y == selectionEnd.y)
                        m_selectionRects.back().width += kerningSelectionEnd;
                }

                /// TODO: Implement a way to calculate text size without creating a text object?
                Text tempText;
                tempText.setFont(m_fontCached);
                tempText.setCharacterSize(getTextSize());
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                {
                    m_selectionRects.back().width += textOffset;
                    m_selectionRects.emplace_back(m_textSelection2.getPosition().x - textOffset, static_cast<float>(i) * m_lineHeight, textOffset, m_lineHeight);

                    if (!m_lines[i].empty())
                    {
                        tempText.setString(m_lines[i]);
                        m_selectionRects.back().width += tempText.findCharacterPos(tempText.getString().length()).x;
                    }
                }

                if (selectionStart.y != selectionEnd.y)
                {
                    m_selectionRects.back().width += textOffset;

                    if (m_textSelection2.getString() != U"")
                    {
                        tempText.setString(m_lines[selectionEnd.y].substr(0, selectionEnd.x));
                        m_selectionRects.emplace_back(m_textSelection2.getPosition().x - textOffset, static_cast<float>(selectionEnd.y) * m_lineHeight,
                                                      textOffset + tempText.findCharacterPos(tempText.getString().length()).x + kerningSelectionEnd, m_lineHeight);
                    }
                    else
                        m_selectionRects.emplace_back(0.f, static_cast<float>(selectionEnd.y) * m_lineHeight, textOffset, m_lineHeight);
                }
            }
        }

        if (m_parentGui)
        {
            const Vector2f caretPosition = {m_caretPosition.x + m_bordersCached.getLeft() + m_paddingCached.getLeft() - static_cast<float>(m_horizontalScrollbar->getValue()),
                                            m_caretPosition.y + m_bordersCached.getTop() + m_paddingCached.getTop() - static_cast<float>(m_verticalScrollbar->getValue())};
            const auto absoluteLineTopPos = getAbsolutePosition({0, caretPosition.y});
            const float caretHeight = std::max(m_fontCached.getFontHeight(m_textSizeCached), m_lineHeight);
            const FloatRect inputRect = {absoluteLineTopPos, getAbsolutePosition({getSize().x, caretPosition.y + caretHeight}) - absoluteLineTopPos};
            m_parentGui->updateTextCursorPosition(inputRect, getAbsolutePosition({caretPosition.x + m_caretWidthCached, caretPosition.y}));
        }

        recalculateVisibleLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::recalculateVisibleLines()
    {
        if (m_lineHeight == 0)
            return;

        float horiScrollOffset = 0.0f;
        if (m_horizontalScrollbar->isShown())
        {
            horiScrollOffset = m_horizontalScrollbar->getSize().y;
            m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
        }

        m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - horiScrollOffset) / m_lineHeight), m_lines.size());

        // Store which area is visible
        if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->setPosition({getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop()});

            m_topLine = static_cast<std::size_t>(m_verticalScrollbar->getValue() / m_lineHeight);

            // The scrollbar may be standing between lines in which case one more line is visible
            if (((static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - horiScrollOffset) % static_cast<unsigned int>(m_lineHeight)) != 0)
             || ((m_verticalScrollbar->getValue() % static_cast<unsigned int>(m_lineHeight)) != 0))
                m_visibleLines++;
        }
        else // There is no scrollbar
        {
            m_topLine = 0;
            m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - horiScrollOffset) / m_lineHeight), m_lines.size());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TextArea::getSignal(String signalName)
    {
        if (signalName == onTextChange.getName())
            return onTextChange;
        else if (signalName == onSelectionChange.getName())
            return onSelectionChange;
        else if (signalName == onCaretPositionChange.getName())
            return onCaretPositionChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == U"TextColor")
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection1.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection2.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == U"SelectedTextColor")
        {
            m_textSelection1.setColor(getSharedRenderer()->getSelectedTextColor());
            m_textSelection2.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == U"DefaultTextColor")
        {
            m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor());
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"Scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (getSharedRenderer()->getScrollbarWidth() == 0)
            {
                m_verticalScrollbar->setSize({m_verticalScrollbar->getDefaultWidth(), m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, m_horizontalScrollbar->getDefaultWidth()});
                setSize(m_size);
            }
        }
        else if (property == U"ScrollbarWidth")
        {
            const float width = (getSharedRenderer()->getScrollbarWidth() != 0) ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, width});
            setSize(m_size);
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"SelectedTextBackgroundColor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"CaretColor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == U"CaretWidth")
        {
            m_caretWidthCached = getSharedRenderer()->getCaretWidth();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_verticalScrollbar->setInheritedOpacity(m_opacityCached);
            m_horizontalScrollbar->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection1.setOpacity(m_opacityCached);
            m_textAfterSelection2.setOpacity(m_opacityCached);
            m_textSelection1.setOpacity(m_opacityCached);
            m_textSelection2.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection1.setFont(m_fontCached);
            m_textSelection2.setFont(m_fontCached);
            m_textAfterSelection1.setFont(m_fontCached);
            m_textAfterSelection2.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);
            setTextSize(getTextSize());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TextArea::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);
        node->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_text));
        if (!getDefaultText().empty())
            node->propertyValuePairs[U"DefaultText"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getDefaultText()));
        node->propertyValuePairs[U"MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maxChars));

        if (m_readOnly)
            node->propertyValuePairs[U"ReadOnly"] = std::make_unique<DataIO::ValueNode>("true");

        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_verticalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs[U"VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs[U"VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs[U"HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs[U"HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value).getString());
        if (node->propertyValuePairs[U"DefaultText"])
            setDefaultText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"DefaultText"]->value).getString());
        if (node->propertyValuePairs[U"MaximumCharacters"])
            setMaximumCharacters(node->propertyValuePairs[U"MaximumCharacters"]->value.toUInt());
        if (node->propertyValuePairs[U"ReadOnly"])
            setReadOnly(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"ReadOnly"]->value).getBool());

        if (node->propertyValuePairs[U"VerticalScrollbarPolicy"])
        {
            String policy = node->propertyValuePairs[U"VerticalScrollbarPolicy"]->value.trim();
            if (policy == U"Automatic")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == U"Always")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == U"Never")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{U"Failed to parse VerticalScrollbarPolicy property, found unknown value '" + policy + U"'."};
        }

        if (node->propertyValuePairs[U"HorizontalScrollbarPolicy"])
        {
            String policy = node->propertyValuePairs[U"HorizontalScrollbarPolicy"]->value.trim();
            if (policy == U"Automatic")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == U"Always")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == U"Never")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{U"Failed to parse HorizontalScrollbarPolicy property, found unknown value '" + policy + U"'."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
            target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        // Draw the contents of the text area
        {
            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

            float clipWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScrollbar->isShown())
                clipWidth -= m_verticalScrollbar->getSize().x;

            float clipHeight = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom();
            if (m_horizontalScrollbar->isShown())
                clipHeight -= m_horizontalScrollbar->getSize().y;

            target.addClippingLayer(states, {{}, {clipWidth, clipHeight}});

            // Move the text according to the scrollars
            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), -static_cast<float>(m_verticalScrollbar->getValue())});

            // Draw the background of the selected text
            for (const auto& selectionRect : m_selectionRects)
            {
                states.transform.translate({selectionRect.left, selectionRect.top});
                target.drawFilledRect(states, {selectionRect.width, selectionRect.height + (std::max(m_fontCached.getFontHeight(m_textSizeCached), m_lineHeight) - m_lineHeight)},
                                      Color::applyOpacity(m_selectedTextBackgroundColorCached, m_opacityCached));
                states.transform.translate({-selectionRect.left, -selectionRect.top});
            }

            // Draw the text
            if (m_text.empty())
                target.drawText(states, m_defaultText);
            else
            {
                target.drawText(states, m_textBeforeSelection);
                if (m_selStart != m_selEnd)
                {
                    target.drawText(states, m_textSelection1);
                    target.drawText(states, m_textSelection2);
                    target.drawText(states, m_textAfterSelection1);
                    target.drawText(states, m_textAfterSelection2);
                }
            }

            // Only draw the caret when needed
            if (m_focused && m_caretVisible && (m_caretWidthCached > 0))
            {
                const float caretHeight = std::max(m_fontCached.getFontHeight(m_textSizeCached), m_lineHeight);
                states.transform.translate({std::ceil(m_caretPosition.x - (m_caretWidthCached / 2.f)), m_caretPosition.y});
                target.drawFilledRect(states, {m_caretWidthCached, caretHeight}, Color::applyOpacity(m_caretColorCached, m_opacityCached));
            }

            target.removeClippingLayer();
        }

        // Draw the scrollbars if needed
        if (m_verticalScrollbar->isShown())
            m_verticalScrollbar->draw(target, statesForScrollbar);

        if (m_horizontalScrollbar->isShown())
            m_horizontalScrollbar->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::setCaretPositionImpl(std::size_t charactersBeforeCaret, bool selEndNeedUpdate, bool emitCaretChangedPosition)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.length())
            charactersBeforeCaret = m_text.length();

        // Find the line and position on that line on which the caret is located
        std::size_t count = 0;
        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            if (count + m_lines[i].length() < charactersBeforeCaret)
            {
                count += m_lines[i].length();
                if ((count < m_text.length()) && (m_text[count] == U'\n'))
                    count += 1;
            }
            else
            {
                m_selStart.y = i;
                m_selStart.x = charactersBeforeCaret - count;

                if (selEndNeedUpdate)
                {
                    if (emitCaretChangedPosition)
                        updateSelEnd(m_selStart);
                    else
                        m_selEnd = m_selStart;
                }
                updateSelectionTexts();
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr TextArea::clone() const
    {
        return std::make_shared<TextArea>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextArea::updateSelEnd(const Vector2<std::size_t>& newValue)
    {
        const bool emit = newValue != m_selEnd;
        m_selEnd = newValue;
        if (emit)
            onCaretPositionChange.emit(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
