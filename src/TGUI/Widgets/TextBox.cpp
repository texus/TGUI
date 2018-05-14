/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Clipboard.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox()
    {
        m_type = "TextBox";
        m_draggableWidget = true;
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection1.setFont(m_fontCached);
        m_textSelection2.setFont(m_fontCached);
        m_textAfterSelection1.setFont(m_fontCached);
        m_textAfterSelection2.setFont(m_fontCached);

        m_renderer = aurora::makeCopied<TextBoxRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setSize({Text::getLineHeight(m_fontCached, m_textSize) * 18,
                 10 * m_fontCached.getLineSpacing(m_textSize) + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize) + Text::getExtraVerticalPadding(m_textSize)
                 + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::Ptr TextBox::create()
    {
        return std::make_shared<TextBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::Ptr TextBox::copy(TextBox::ConstPtr textBox)
    {
        if (textBox)
            return std::static_pointer_cast<TextBox>(textBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBoxRenderer* TextBox::getSharedRenderer()
    {
        return aurora::downcast<TextBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextBoxRenderer* TextBox::getSharedRenderer() const
    {
        return aurora::downcast<const TextBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBoxRenderer* TextBox::getRenderer()
    {
        return aurora::downcast<TextBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TextBoxRenderer* TextBox::getRenderer() const
    {
        return aurora::downcast<const TextBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // If there is a scrollbar then reinitialize it
        if (isVerticalScrollbarPresent())
        {
            m_verticalScroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));
            m_verticalScroll->setSize({m_verticalScroll->getSize().x, getInnerSize().y});
        }

        // The size of the text box has changed, update the text
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const sf::String& text)
    {
        // Remove all the excess characters when a character limit is set
        if ((m_maxChars > 0) && (text.getSize() > m_maxChars))
            m_text = text.substring(0, m_maxChars);
        else
            m_text = text;

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::addText(const sf::String& text)
    {
        setText(m_text + text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& TextBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String TextBox::getSelectedText() const
    {
        auto pos = findTextSelectionPositions();
        if (pos.first > pos.second)
            std::swap(pos.first, pos.second);

        return m_text.substring(pos.first, pos.second - pos.first);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;
        if (m_textSize < 1)
            m_textSize = 1;

        // Change the text size
        m_textBeforeSelection.setCharacterSize(m_textSize);
        m_textSelection1.setCharacterSize(m_textSize);
        m_textSelection2.setCharacterSize(m_textSize);
        m_textAfterSelection1.setCharacterSize(m_textSize);
        m_textAfterSelection2.setCharacterSize(m_textSize);

        // Calculate the height of one line
        m_lineHeight = static_cast<unsigned int>(m_fontCached.getLineSpacing(m_textSize));

        m_verticalScroll->setScrollAmount(m_lineHeight);

        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int TextBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(std::size_t maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_text.getSize() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            rearrangeText(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setVerticalScrollbarPresent(bool present)
    {
        if (present)
        {
            m_verticalScroll->setVisible(true);
            setSize(m_size);
        }
        else
        {
            m_verticalScroll->setVisible(false);
            rearrangeText(false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::isVerticalScrollbarPresent() const
    {
        return m_verticalScroll->isVisible();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.getSize())
            charactersBeforeCaret = m_text.getSize();

        // Find the line and position on that line on which the caret is located
        std::size_t count = 0;
        for (std::size_t i = 0; i < m_lines.size(); ++i)
        {
            if (count + m_lines[i].getSize() < charactersBeforeCaret)
            {
                count += m_lines[i].getSize();
                if ((count < m_text.getSize()) && (m_text[count] == '\n'))
                    count += 1;
            }
            else
            {
                m_selStart.y = i;
                m_selStart.x = charactersBeforeCaret - count;

                m_selEnd = m_selStart;
                updateSelectionTexts();
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getCaretPosition() const
    {
        return findTextSelectionPositions().second;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TextBox::getLinesCount() const
    {
        return m_lines.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setFocused(bool focused)
    {
        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};
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

    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(focused);
    #endif

        Widget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        // Set the mouse down flag
        m_mouseDown = true;

        // If there is a scrollbar then pass the event
        if ((m_verticalScroll->isShown()) && (m_verticalScroll->mouseOnWidget(pos)))
        {
            m_verticalScroll->leftMousePressed(pos);
            recalculateVisibleLines();
        }
        else // The click occurred on the text box
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return;

            const auto caretPosition = findCaretPosition(pos);

            // Check if this is a double click
            if ((m_possibleDoubleClick) && (m_selStart == m_selEnd) && (caretPosition == m_selEnd))
            {
                // The next click is going to be a normal one again
                m_possibleDoubleClick = false;

                // If the click was to the right of the end of line then make sure to select the word on the left
                if (m_lines[m_selStart.y].getSize() > 1 && (m_selStart.x == (m_lines[m_selStart.y].getSize()-1) || m_selStart.x == m_lines[m_selStart.y].getSize()))
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
                for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].getSize(); ++i)
                {
                    if (selectingWhitespace != isWhitespace(m_lines[m_selEnd.y][i]))
                    {
                        m_selEnd.x = i;
                        break;
                    }
                    else
                        m_selEnd.x = m_lines[m_selEnd.y].getSize();
                }
            }
            else // No double clicking
            {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
                    m_selStart = caretPosition;

                m_selEnd = caretPosition;

                // If the next click comes soon enough then it will be a double click
                m_possibleDoubleClick = true;
            }

            // Update the texts
            updateSelectionTexts();

            // The caret should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMouseReleased(Vector2f pos)
    {
        // If there is a scrollbar then pass it the event
        if (m_verticalScroll->isShown())
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_verticalScroll->isMouseDown())
            {
                m_verticalScroll->leftMouseReleased(pos - getPosition());
                recalculateVisibleLines();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse event should go to the scrollbar
        if (m_verticalScroll->isShown() && ((m_verticalScroll->isMouseDown() && m_verticalScroll->isMouseDownOnThumb()) || m_verticalScroll->mouseOnWidget(pos)))
        {
            m_verticalScroll->mouseMoved(pos);
            recalculateVisibleLines();
        }

        // If the mouse is held down then you are selecting text
        else if (m_mouseDown)
        {
            const sf::Vector2<std::size_t> caretPosition = findCaretPosition(pos);
            if (caretPosition != m_selEnd)
            {
                m_selEnd = caretPosition;
                updateSelectionTexts();
            }

            // Check if the caret is located above or below the view
            if (m_verticalScroll->isShown())
            {
                if (m_selEnd.y <= m_topLine)
                    m_verticalScroll->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
                else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                    m_verticalScroll->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight) - m_verticalScroll->getViewportSize()));

                recalculateVisibleLines();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_verticalScroll->isShown())
            m_verticalScroll->mouseNoLongerOnWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        if (m_verticalScroll->isShown())
            m_verticalScroll->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        switch (event.code)
        {
            case sf::Keyboard::Up:
            {
                m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y - (m_lineHeight / 2.f) - m_verticalScroll->getValue()});

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Down:
            {
                m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y + (m_lineHeight * 1.5f) - m_verticalScroll->getValue()});

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Left:
            {
                if (event.control)
                {
                    // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
                    bool skippedWhitespace = false;
                    bool done = false;
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
                                    m_selEnd.x = m_lines[m_selEnd.y].getSize();
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
                }
                else // Control key is not being pressed
                {
                    if ((m_selStart != m_selEnd) && !event.shift)
                    {
                        if ((m_selStart.y < m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x < m_selEnd.x)))
                            m_selEnd = m_selStart;
                    }
                    else if (m_selEnd.x > 0)
                        m_selEnd.x--;
                    else
                    {
                        // You are at the left side of a line so move up
                        if (m_selEnd.y > 0)
                        {
                            m_selEnd.y--;
                            m_selEnd.x = m_lines[m_selEnd.y].getSize();
                        }
                    }
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Right:
            {
                if (event.control)
                {
                    // Move to the end of the word (or to the end of the next word when already at the end)
                    bool skippedWhitespace = false;
                    bool done = false;
                    for (std::size_t j = m_selEnd.y; j < m_lines.size(); ++j)
                    {
                        for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].getSize(); ++i)
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
                                m_selEnd.x = m_lines[m_selEnd.y].getSize();
                                break;
                            }
                        }
                        else
                            break;
                    }
                }
                else // Control key is not being pressed
                {
                    if ((m_selStart != m_selEnd) && !event.shift)
                    {
                        if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                            m_selEnd = m_selStart;
                    }
                    else
                    {
                        // Move to the next line if you are at the end of the line
                        if (m_selEnd.x == m_lines[m_selEnd.y].getSize())
                        {
                            if (m_selEnd.y + 1 < m_lines.size())
                            {
                                m_selEnd.y++;
                                m_selEnd.x = 0;
                            }
                        }
                        else
                            m_selEnd.x++;
                    }
                }

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Home:
            {
                if (event.control)
                    m_selEnd = {0, 0};
                else
                    m_selEnd.x = 0;

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::End:
            {
                if (event.control)
                    m_selEnd = {m_lines[m_lines.size()-1].getSize(), m_lines.size()-1};
                else
                    m_selEnd.x = m_lines[m_selEnd.y].getSize();

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::PageUp:
            {
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

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::PageDown:
            {
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

                m_selEnd.x = m_lines[m_selEnd.y].getSize();

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Return:
            {
                textEntered('\n');
                break;
            }

            case sf::Keyboard::BackSpace:
            {
                if (m_readOnly)
                    break;

                // Check that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    const std::size_t pos = findTextSelectionPositions().second;
                    if (pos > 0)
                    {
                        if (m_selEnd.x > 0)
                        {
                            // There is a specific case that we have to watch out for. When we are removing the last character on
                            // a line which was placed there by word wrap and a newline follows this character then the caret
                            // has to be placed at the line above (before the newline) instead of at the same line (after the newline)
                            if ((m_lines[m_selEnd.y].getSize() == 1) && (pos > 1) && (pos < m_text.getSize()) && (m_text[pos-2] != '\n') && (m_text[pos] == '\n') && (m_selEnd.y > 0))
                            {
                                m_selEnd.y--;
                                m_selEnd.x = m_lines[m_selEnd.y].getSize();
                            }
                            else // Just remove the character normally
                                --m_selEnd.x;
                        }
                        else // At the beginning of the line
                        {
                            if (m_selEnd.y > 0)
                            {
                                --m_selEnd.y;
                                m_selEnd.x = m_lines[m_selEnd.y].getSize();

                                if ((m_text[pos-1] != '\n') && m_selEnd.x > 0)
                                    --m_selEnd.x;
                            }
                        }

                        m_selStart = m_selEnd;

                        m_text.erase(pos - 1, 1);
                        rearrangeText(true);
                    }
                }
                else // When you did select some characters then delete them
                    deleteSelectedCharacters();

                // The caret should be visible again
                m_caretVisible = true;
                m_animationTimeElapsed = {};

                onTextChange.emit(this, m_text);
                break;
            }

            case sf::Keyboard::Delete:
            {
                if (m_readOnly)
                    break;

                // Check that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    m_text.erase(findTextSelectionPositions().second, 1);
                    rearrangeText(true);
                }
                else // You did select some characters, so remove them
                    deleteSelectedCharacters();

                onTextChange.emit(this, m_text);
                break;
            }

            case sf::Keyboard::A:
            {
                if (event.control && !event.alt && !event.shift && !event.system)
                {
                    m_selStart = {0, 0};
                    m_selEnd = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
                    updateSelectionTexts();
                }

                break;
            }

            case sf::Keyboard::C:
            {
                auto selectionPositions = findTextSelectionPositions();
                if (selectionPositions.first > selectionPositions.second)
                    std::swap(selectionPositions.first, selectionPositions.second);

                if (event.control && !event.alt && !event.shift && !event.system)
                    Clipboard::set(m_text.substring(selectionPositions.first, selectionPositions.second - selectionPositions.first));

                break;
            }

            case sf::Keyboard::X:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    auto selectionPositions = findTextSelectionPositions();
                    if (selectionPositions.first > selectionPositions.second)
                        std::swap(selectionPositions.first, selectionPositions.second);

                    Clipboard::set(m_text.substring(selectionPositions.first, selectionPositions.second - selectionPositions.first));
                    deleteSelectedCharacters();
                }

                break;
            }

            case sf::Keyboard::V:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    const sf::String clipboardContents = Clipboard::get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selStart != m_selEnd) || (clipboardContents != ""))
                    {
                        deleteSelectedCharacters();

                        m_text.insert(findTextSelectionPositions().first, clipboardContents);
                        m_lines[m_selStart.y].insert(m_selStart.x, clipboardContents);

                        m_selStart.x += clipboardContents.getSize();
                        m_selEnd = m_selStart;
                        rearrangeText(true);

                        onTextChange.emit(this, m_text);
                    }
                }

                break;
            }

            default:
                break;
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::textEntered(std::uint32_t key)
    {
        if (m_readOnly)
            return;

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
            return;

        auto insert = [=]()
        {
            deleteSelectedCharacters();

            const std::size_t caretPosition = findTextSelectionPositions().first;

            m_text.insert(caretPosition, key);
            m_lines[m_selEnd.y].insert(m_selEnd.x, key);

            // Increment the caret position, unless you type a newline at the start of a line while that line only existed due to word wrapping
            if ((key != '\n') || (m_selEnd.x > 0) || (m_selEnd.y == 0) || m_lines[m_selEnd.y-1].isEmpty() || (m_text[caretPosition-1] == '\n'))
            {
                m_selStart.x++;
                m_selEnd.x++;
            }

            rearrangeText(true);
        };

        // If there is a scrollbar then inserting can't go wrong
        if (isVerticalScrollbarPresent())
        {
            insert();
        }
        else // There is no scrollbar, the text may not fit
        {
            // Store the data so that it can be reverted
            sf::String oldText = m_text;
            const auto oldSelStart = m_selStart;
            const auto oldSelEnd = m_selEnd;

            // Try to insert the character
            insert();

            // Undo the insert if the text does not fit
            if (m_lines.size() > getInnerSize().y / m_lineHeight)
            {
                m_text = oldText;
                m_selStart = oldSelStart;
                m_selEnd = oldSelEnd;

                rearrangeText(true);
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_verticalScroll->isShown())
        {
            m_verticalScroll->mouseWheelScrolled(delta, pos - getPosition());
            recalculateVisibleLines();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2<std::size_t> TextBox::findCaretPosition(Vector2f position) const
    {
        position.x -= m_bordersCached.getLeft() + m_paddingCached.getLeft();
        position.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find on which line the mouse is
        std::size_t lineNumber;
        if (m_verticalScroll->isShown())
        {
            if (position.y + m_verticalScroll->getValue() < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor((position.y + m_verticalScroll->getValue()) / m_lineHeight));
        }
        else
        {
            if (position.y < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor(position.y / m_lineHeight));
        }

        // Check if you clicked behind everything
        if (lineNumber + 1 > m_lines.size())
            return sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find between which character the mouse is standing
        float width = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
        std::uint32_t prevChar = 0;
        for (std::size_t i = 0; i < m_lines[lineNumber].getSize(); ++i)
        {
            float charWidth;
            const std::uint32_t curChar = m_lines[lineNumber][i];
            //if (curChar == '\n')
            //    return sf::Vector2f<std::size_t>(m_lines[lineNumber].getSize() - 1, lineNumber); // TextBox strips newlines but this code is kept for when this function is generalized
            //else
            if (curChar == '\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', getTextSize(), false).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, getTextSize(), false).advance);

            const float kerning = m_fontCached.getKerning(prevChar, curChar, getTextSize());
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
        return sf::Vector2<std::size_t>(m_lines[lineNumber].getSize(), lineNumber);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<std::size_t, std::size_t> TextBox::findTextSelectionPositions() const
    {
        // This function is used to count the amount of characters spread over several lines
        auto findIndex = [this](std::size_t line)
        {
            std::size_t counter = 0;
            for (std::size_t i = 0; i < line; ++i)
            {
                counter += m_lines[i].getSize();
                if ((counter < m_text.getSize()) && (m_text[counter] == '\n'))
                    counter += 1;
            }

            return counter;
        };

        return {findIndex(m_selStart.y) + m_selStart.x, findIndex(m_selEnd.y) + m_selEnd.x};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::deleteSelectedCharacters()
    {
        if (m_selStart != m_selEnd)
        {
            auto textSelectionPositions = findTextSelectionPositions();

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
            {
                m_text.erase(textSelectionPositions.second, textSelectionPositions.first - textSelectionPositions.second);
                m_selStart = m_selEnd;
            }
            else
            {
                m_text.erase(textSelectionPositions.first, textSelectionPositions.second - textSelectionPositions.first);
                m_selEnd = m_selStart;
            }

            rearrangeText(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rearrangeText(bool keepSelection)
    {
        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_fontCached == nullptr))
            return;

        // Find the maximum width of one line
        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
        float maxLineWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - 2 * textOffset;
        if (m_verticalScroll->isShown())
            maxLineWidth -= m_verticalScroll->getSize().x;

        // Don't do anything when there is no room for the text
        if (maxLineWidth <= 0)
            return;

        // Store the current selection position when we are keeping the selection
        std::pair<std::size_t, std::size_t> textSelectionPositions;
        if (keepSelection)
            textSelectionPositions = findTextSelectionPositions();

        // Fit the text in the available space
        const sf::String string = Text::wordWrap(maxLineWidth, m_text, m_fontCached, m_textSize, false, false);

        // Split the string in multiple lines
        m_lines.clear();
        std::size_t searchPosStart = 0;
        std::size_t newLinePos = 0;
        while (newLinePos != sf::String::InvalidPos)
        {
            newLinePos = string.find('\n', searchPosStart);

            if (newLinePos != sf::String::InvalidPos)
                m_lines.push_back(string.substring(searchPosStart, newLinePos - searchPosStart));
            else
                m_lines.push_back(string.substring(searchPosStart));

            searchPosStart = newLinePos + 1;
        }

        // Check if we should try to keep our selection
        if (keepSelection)
        {
            std::size_t index = 0;
            sf::Vector2<std::size_t> newSelStart;
            sf::Vector2<std::size_t> newSelEnd;
            bool newSelStartFound = false;
            bool newSelEndFound = false;

            // Look for the new locations of our selection
            for (std::size_t i = 0; i < m_lines.size(); ++i)
            {
                index += m_lines[i].getSize();

                if (!newSelStartFound && (index >= textSelectionPositions.first))
                {
                    newSelStart = sf::Vector2<std::size_t>(m_lines[i].getSize() - (index - textSelectionPositions.first), i);

                    newSelStartFound = true;
                    if (newSelEndFound)
                        break;
                }

                if (!newSelEndFound && (index >= textSelectionPositions.second))
                {
                    newSelEnd = sf::Vector2<std::size_t>(m_lines[i].getSize() - (index - textSelectionPositions.second), i);

                    newSelEndFound = true;
                    if (newSelStartFound)
                        break;
                }

                // Skip newlines in the text
                if (m_text[index] == '\n')
                    ++index;
            }

            // Keep the selection when possible
            if (newSelStartFound && newSelEndFound)
            {
                m_selStart = newSelStart;
                m_selEnd = newSelEnd;
            }
            else // The text has changed too much, the selection can't be kept
            {
                m_selStart = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
                m_selEnd = m_selStart;
            }
        }
        else // Set the caret at the back of the text
        {
            m_selStart = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
            m_selEnd = m_selStart;
        }

        // Tell the scrollbar how many pixels the text contains
        const bool scrollbarShown = m_verticalScroll->isShown();

        m_verticalScroll->setMaximum(static_cast<unsigned int>(m_lines.size() * m_lineHeight
                                                              + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)
                                                              + Text::getExtraVerticalPadding(m_textSize)));

        // We may have to recalculate what we just calculated if the scrollbar just appeared or disappeared
        if (scrollbarShown != m_verticalScroll->isShown())
        {
            rearrangeText(true);
            return;
        }

        updateSelectionTexts();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::updateSelectionTexts()
    {
        // If there is no selection then just put the whole text in m_textBeforeSelection
        if (m_selStart == m_selEnd)
        {
            sf::String displayedText;
            for (std::size_t i = 0; i < m_lines.size(); ++i)
                displayedText += m_lines[i] + "\n";

            m_textBeforeSelection.setString(displayedText);
            m_textSelection1.setString("");
            m_textSelection2.setString("");
            m_textAfterSelection1.setString("");
            m_textAfterSelection2.setString("");
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
                sf::String string;
                for (std::size_t i = 0; i < selectionStart.y; ++i)
                    string += m_lines[i] + "\n";

                string += m_lines[selectionStart.y].substring(0, selectionStart.x);
                m_textBeforeSelection.setString(string);
            }
            else
                m_textBeforeSelection.setString(m_lines[0].substring(0, selectionStart.x));

            // Set the selected text
            if (m_selStart.y == m_selEnd.y)
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, selectionEnd.x - selectionStart.x));
                m_textSelection2.setString("");
            }
            else
            {
                m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, m_lines[selectionStart.y].getSize() - selectionStart.x));

                sf::String string;
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                    string += m_lines[i] + "\n";

                string += m_lines[selectionEnd.y].substring(0, selectionEnd.x);

                m_textSelection2.setString(string);
            }

            // Set the text after the selection
            {
                m_textAfterSelection1.setString(m_lines[selectionEnd.y].substring(selectionEnd.x, m_lines[selectionEnd.y].getSize() - selectionEnd.x));

                sf::String string;
                for (std::size_t i = selectionEnd.y + 1; i < m_lines.size(); ++i)
                    string += m_lines[i] + "\n";

                m_textAfterSelection2.setString(string);
            }
        }

        // Check if the caret is located above or below the view
        if (isVerticalScrollbarPresent())
        {
            if (m_selEnd.y <= m_topLine)
                m_verticalScroll->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
            else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                m_verticalScroll->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight)
                                                                    + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)
                                                                    + Text::getExtraVerticalPadding(m_textSize)
                                                                    - m_verticalScroll->getViewportSize()));
        }

        recalculatePositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TextBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // Only show/hide the caret every half second
        if (m_animationTimeElapsed >= sf::milliseconds(500))
        {
            // Reset the elapsed time
            m_animationTimeElapsed = {};

            // Switch the value of the visible flag
            m_caretVisible = !m_caretVisible;

            // Too slow for double clicking
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::recalculatePositions()
    {
        if (!m_fontCached)
            return;

        const float textOffset = Text::getExtraHorizontalPadding(m_fontCached, m_textSize);
        sf::Text tempText{"", *m_fontCached.getFont(), getTextSize()};

        // Position the caret
        {
            tempText.setString(m_lines[m_selEnd.y].substring(0, m_selEnd.x));

            float kerning = 0;
            if ((m_selEnd.x > 0) && (m_selEnd.x < m_lines[m_selEnd.y].getSize()))
                kerning = m_fontCached.getKerning(m_lines[m_selEnd.y][m_selEnd.x-1], m_lines[m_selEnd.y][m_selEnd.x], m_textSize);

            m_caretPosition = {textOffset + tempText.findCharacterPos(tempText.getString().getSize()).x + kerning, static_cast<float>(m_selEnd.y * m_lineHeight)};
        }

        // Calculate the position of the text objects
        m_selectionRects.clear();
        m_textBeforeSelection.setPosition({textOffset, 0});
        if (m_selStart != m_selEnd)
        {
            auto selectionStart = m_selStart;
            auto selectionEnd = m_selEnd;

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                std::swap(selectionStart, selectionEnd);

            float kerningSelectionStart = 0;
            if ((selectionStart.x > 0) && (selectionStart.x < m_lines[selectionStart.y].getSize()))
                kerningSelectionStart = m_fontCached.getKerning(m_lines[selectionStart.y][selectionStart.x-1], m_lines[selectionStart.y][selectionStart.x], m_textSize);

            float kerningSelectionEnd = 0;
            if ((selectionEnd.x > 0) && (selectionEnd.x < m_lines[selectionEnd.y].getSize()))
                kerningSelectionEnd = m_fontCached.getKerning(m_lines[selectionEnd.y][selectionEnd.x-1], m_lines[selectionEnd.y][selectionEnd.x], m_textSize);

            if (selectionStart.x > 0)
            {
                m_textSelection1.setPosition({textOffset + m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x + kerningSelectionStart,
                                              m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});
            }
            else
                m_textSelection1.setPosition({textOffset, m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});

            m_textSelection2.setPosition({textOffset, static_cast<float>((selectionStart.y + 1) * m_lineHeight)});

            if (!m_textSelection2.getString().isEmpty() || (selectionEnd.x == 0))
            {
                m_textAfterSelection1.setPosition({textOffset + m_textSelection2.findCharacterPos(m_textSelection2.getString().getSize()).x + kerningSelectionEnd,
                                                   m_textSelection2.getPosition().y + ((selectionEnd.y - selectionStart.y - 1) * m_lineHeight)});
            }
            else
                m_textAfterSelection1.setPosition({m_textSelection1.getPosition().x + m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()).x + kerningSelectionEnd,
                                                   m_textSelection1.getPosition().y});

            m_textAfterSelection2.setPosition({textOffset, static_cast<float>((selectionEnd.y + 1) * m_lineHeight)});

            // Recalculate the selection rectangles
            {
                m_selectionRects.push_back({m_textSelection1.getPosition().x, static_cast<float>(selectionStart.y * m_lineHeight), 0, static_cast<float>(m_lineHeight)});

                if (!m_lines[selectionStart.y].isEmpty())
                {
                    m_selectionRects.back().width = m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()).x;

                    // There is kerning when the selection is on just this line
                    if (selectionStart.y == selectionEnd.y)
                        m_selectionRects.back().width += kerningSelectionEnd;
                }

                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                {
                    m_selectionRects.back().width += textOffset;
                    m_selectionRects.push_back({m_textSelection2.getPosition().x - textOffset, static_cast<float>(i * m_lineHeight), textOffset, static_cast<float>(m_lineHeight)});

                    if (!m_lines[i].isEmpty())
                    {
                        tempText.setString(m_lines[i]);
                        m_selectionRects.back().width += tempText.findCharacterPos(tempText.getString().getSize()).x;
                    }
                }

                if (selectionStart.y != selectionEnd.y)
                {
                    m_selectionRects.back().width += textOffset;

                    if (m_textSelection2.getString() != "")
                    {
                        tempText.setString(m_lines[selectionEnd.y].substring(0, selectionEnd.x));
                        m_selectionRects.push_back({m_textSelection2.getPosition().x - textOffset, static_cast<float>(selectionEnd.y * m_lineHeight),
                                                    textOffset + tempText.findCharacterPos(tempText.getString().getSize()).x + kerningSelectionEnd, static_cast<float>(m_lineHeight)});
                    }
                    else
                        m_selectionRects.push_back({0, static_cast<float>(selectionEnd.y * m_lineHeight), textOffset, static_cast<float>(m_lineHeight)});
                }
            }
        }

        recalculateVisibleLines();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::recalculateVisibleLines()
    {
        m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight), m_lines.size());

        // Store which area is visible
        if (m_verticalScroll->isShown())
        {
            m_verticalScroll->setPosition({getSize().x - m_bordersCached.getRight() - m_verticalScroll->getSize().x, m_bordersCached.getTop()});

            m_topLine = m_verticalScroll->getValue() / m_lineHeight;

            // The scrollbar may be standing between lines in which case one more line is visible
            if (((static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) % m_lineHeight) != 0) || ((m_verticalScroll->getValue() % m_lineHeight) != 0))
                m_visibleLines++;
        }
        else // There is no scrollbar
        {
            m_topLine = 0;
            m_visibleLines = std::min(static_cast<std::size_t>((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()) / m_lineHeight), m_lines.size());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TextBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTextChange.getName()))
            return onTextChange;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection1.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection2.setColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection1.setColor(getSharedRenderer()->getSelectedTextColor());
            m_textSelection2.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "scrollbar")
        {
            m_verticalScroll->setRenderer(getSharedRenderer()->getScrollbar());
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScroll->getDefaultWidth();
            m_verticalScroll->setSize({width, m_verticalScroll->getSize().y});
            setSize(m_size);
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "caretcolor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == "caretwidth")
        {
            m_caretWidthCached = getSharedRenderer()->getCaretWidth();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_verticalScroll->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection1.setOpacity(m_opacityCached);
            m_textAfterSelection2.setOpacity(m_opacityCached);
            m_textSelection1.setOpacity(m_opacityCached);
            m_textSelection2.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection1.setFont(m_fontCached);
            m_textSelection2.setFont(m_fontCached);
            m_textAfterSelection1.setFont(m_fontCached);
            m_textAfterSelection2.setFont(m_fontCached);
            setTextSize(getTextSize());
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TextBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        node->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getText()));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(to_string(m_maxChars));

        if (m_readOnly)
            node->propertyValuePairs["ReadOnly"] = std::make_unique<DataIO::ValueNode>("true");

        if (!isVerticalScrollbarPresent())
            node->propertyValuePairs["VerticalScrollbarPresent"] = std::make_unique<DataIO::ValueNode>("false");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["text"]->value).getString());
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumcharacters"])
            setMaximumCharacters(tgui::stoi(node->propertyValuePairs["maximumcharacters"]->value));
        if (node->propertyValuePairs["readonly"])
            setReadOnly(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["readonly"]->value).getBool());
        if (node->propertyValuePairs["verticalscrollbarpresent"])
            setVerticalScrollbarPresent(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["verticalscrollbarpresent"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the contents of the text box
        {
            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

            float maxLineWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_verticalScroll->isShown())
                maxLineWidth -= m_verticalScroll->getSize().x;

            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            const Clipping clipping{target, states, {}, {maxLineWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            // Move the text according to the vertical scrollar
            states.transform.translate({0, -static_cast<float>(m_verticalScroll->getValue())});

            // Draw the background of the selected text
            for (const auto& selectionRect : m_selectionRects)
            {
                states.transform.translate({selectionRect.left, selectionRect.top});
                drawRectangleShape(target, states, {selectionRect.width, selectionRect.height + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize)}, m_selectedTextBackgroundColorCached);
                states.transform.translate({-selectionRect.left, -selectionRect.top});
            }

            // Draw the text
            m_textBeforeSelection.draw(target, states);
            if (m_selStart != m_selEnd)
            {
                m_textSelection1.draw(target, states);
                m_textSelection2.draw(target, states);
                m_textAfterSelection1.draw(target, states);
                m_textAfterSelection2.draw(target, states);
            }

            // Only draw the caret when needed
            if (m_focused && m_caretVisible && (m_caretWidthCached > 0))
            {
                const float caretHeight = m_lineHeight + Text::calculateExtraVerticalSpace(m_fontCached, m_textSize);
                states.transform.translate({std::ceil(m_caretPosition.x - (m_caretWidthCached / 2.f)), m_caretPosition.y});
                drawRectangleShape(target, states, {m_caretWidthCached, caretHeight}, m_caretColorCached);
            }
        }

        // Draw the scrollbar if needed
        if (m_verticalScroll->isShown())
            m_verticalScroll->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
