/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Container.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Clipping.hpp>

#include <cassert>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox()
    {
        m_callback.widgetType = "TextBox";
        m_draggableWidget = true;

        addSignal<sf::String>("TextChanged");

        m_renderer = std::make_shared<TextBoxRenderer>(this);
        reload();

        setSize({360, 189});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox(const TextBox& scrollbarToCopy) :
        Widget               {scrollbarToCopy},
        m_text               {scrollbarToCopy.m_text},
        m_textSize           {scrollbarToCopy.m_textSize},
        m_lineHeight         {scrollbarToCopy.m_lineHeight},
        m_lines              (scrollbarToCopy.m_lines), // Did not compile in VS2013 when using braces
        m_maxChars           {scrollbarToCopy.m_maxChars},
        m_topLine            {scrollbarToCopy.m_topLine},
        m_visibleLines       {scrollbarToCopy.m_visibleLines},
        m_selStart           {scrollbarToCopy.m_selStart},
        m_selEnd             {scrollbarToCopy.m_selEnd},
        m_caretPosition      {scrollbarToCopy.m_caretPosition},
        m_caretVisible       {scrollbarToCopy.m_caretVisible},
        m_textBeforeSelection{scrollbarToCopy.m_textBeforeSelection},
        m_textSelection1     {scrollbarToCopy.m_textSelection1},
        m_textSelection2     {scrollbarToCopy.m_textSelection2},
        m_textAfterSelection1{scrollbarToCopy.m_textAfterSelection1},
        m_textAfterSelection2{scrollbarToCopy.m_textAfterSelection2},
        m_selectionRects     (scrollbarToCopy.m_selectionRects), // Did not compile in VS2013 when using braces
        m_scroll             {Scrollbar::copy(scrollbarToCopy.m_scroll)},
        m_possibleDoubleClick{scrollbarToCopy.m_possibleDoubleClick},
        m_readOnly           {scrollbarToCopy.m_readOnly}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox& TextBox::operator= (const TextBox& right)
    {
        if (this != &right)
        {
            TextBox temp(right);
            Widget::operator=(right);

            std::swap(m_text,                temp.m_text);
            std::swap(m_textSize,            temp.m_textSize);
            std::swap(m_lineHeight,          temp.m_lineHeight);
            std::swap(m_lines,               temp.m_lines);
            std::swap(m_maxChars,            temp.m_maxChars);
            std::swap(m_topLine,             temp.m_topLine);
            std::swap(m_visibleLines,        temp.m_visibleLines);
            std::swap(m_selStart,            temp.m_selStart);
            std::swap(m_selEnd,              temp.m_selEnd);
            std::swap(m_caretPosition,       temp.m_caretPosition);
            std::swap(m_caretVisible,        temp.m_caretVisible);
            std::swap(m_textBeforeSelection, temp.m_textBeforeSelection);
            std::swap(m_textSelection1,      temp.m_textSelection1);
            std::swap(m_textSelection2,      temp.m_textSelection2);
            std::swap(m_textAfterSelection1, temp.m_textAfterSelection1);
            std::swap(m_textAfterSelection2, temp.m_textAfterSelection2);
            std::swap(m_selectionRects,      temp.m_selectionRects);
            std::swap(m_scroll,              temp.m_scroll);
            std::swap(m_possibleDoubleClick, temp.m_possibleDoubleClick);
            std::swap(m_readOnly,            temp.m_readOnly);
        }

        return *this;
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

    void TextBox::setPosition(const Layout2d& position)
    {
        // When the position is changed we just have to move the stuff. Otherwise it means we have to recalculate things.
        if (getPosition() != position.getValue())
        {
            sf::Vector2f diff = position.getValue() - getPosition();

            Widget::setPosition(position);

            getRenderer()->m_backgroundTexture.setPosition(getPosition());

            m_caretPosition += diff;

            // Just move everything
            m_textBeforeSelection.setPosition(m_textBeforeSelection.getPosition() + diff);
            if (m_selStart != m_selEnd)
            {
                m_textSelection1.setPosition(m_textSelection1.getPosition() + diff);
                m_textSelection2.setPosition(m_textSelection2.getPosition() + diff);
                m_textAfterSelection1.setPosition(m_textAfterSelection1.getPosition() + diff);
                m_textAfterSelection2.setPosition(m_textAfterSelection2.getPosition() + diff);
            }

            if (m_scroll)
                m_scroll->setPosition(m_scroll->getPosition() + diff);
        }
        else if (m_font) // Recalculate everything
        {
            getRenderer()->m_backgroundTexture.setPosition(getPosition());

            sf::Text tempText{"", *m_font, getTextSize()};
            float textShiftY = getTextVerticalCorrection(getFont(), getTextSize());
            Padding padding = getRenderer()->getScaledPadding();

            // Position the caret
            {
                tempText.setString(m_lines[m_selEnd.y].substring(0, m_selEnd.x));

                float kerning = 0;
                if ((m_selEnd.x > 0) && (m_selEnd.x < m_lines[m_selEnd.y].getSize()))
                    kerning = m_font->getKerning(m_lines[m_selEnd.y][m_selEnd.x-1], m_lines[m_selEnd.y][m_selEnd.x], m_textSize);

                m_caretPosition = {getPosition().x + padding.left + tempText.findCharacterPos(tempText.getString().getSize()).x + kerning,
                                   getPosition().y + padding.top + (m_selEnd.y * m_lineHeight)};

            }

            // Calculate the position of the text objects
            m_selectionRects.clear();
            m_textBeforeSelection.setPosition({getPosition().x + padding.left, getPosition().y + padding.top - textShiftY});
            if (m_selStart != m_selEnd)
            {
                auto selectionStart = m_selStart;
                auto selectionEnd = m_selEnd;

                if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                    std::swap(selectionStart, selectionEnd);

                float kerningSelectionStart = 0;
                if ((selectionStart.x > 0) && (selectionStart.x < m_lines[selectionStart.y].getSize()))
                    kerningSelectionStart = m_font->getKerning(m_lines[selectionStart.y][selectionStart.x-1], m_lines[selectionStart.y][selectionStart.x], m_textSize);

                float kerningSelectionEnd = 0;
                if ((selectionEnd.x > 0) && (selectionEnd.x < m_lines[selectionEnd.y].getSize()))
                    kerningSelectionEnd = m_font->getKerning(m_lines[selectionEnd.y][selectionEnd.x-1], m_lines[selectionEnd.y][selectionEnd.x], m_textSize);

                if (selectionStart.x > 0)
                {
                    m_textSelection1.setPosition({m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x + kerningSelectionStart,
                                                  m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});
                }
                else
                    m_textSelection1.setPosition({getPosition().x + padding.left, m_textBeforeSelection.getPosition().y + (selectionStart.y * m_lineHeight)});

                m_textSelection2.setPosition({getPosition().x + padding.left, getPosition().y + padding.top + ((selectionStart.y + 1) * m_lineHeight) - textShiftY});

                if ((m_textSelection2.getString() != "") || (m_textSelection1.getString()[m_textSelection1.getString().getSize()-1] == '\n'))
                {
                    m_textAfterSelection1.setPosition({m_textSelection2.findCharacterPos(m_textSelection2.getString().getSize()).x + kerningSelectionEnd,
                                                       m_textSelection2.getPosition().y + ((selectionEnd.y - selectionStart.y - 1) * m_lineHeight)});
                }
                else
                    m_textAfterSelection1.setPosition({m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()).x + kerningSelectionEnd, m_textSelection1.getPosition().y});

                m_textAfterSelection2.setPosition({getPosition().x + padding.left, getPosition().y + padding.top + ((selectionEnd.y + 1) * m_lineHeight) - textShiftY});

                // Recalculate the selection rectangles
                {
                    m_selectionRects.push_back({m_textSelection1.getPosition().x, getPosition().y + padding.top + (selectionStart.y * m_lineHeight), 0, static_cast<float>(m_lineHeight)});

                    if ((!m_lines[selectionStart.y].isEmpty()) && (m_lines[selectionStart.y] != "\n"))
                    {
                        if (m_textSelection1.getString()[m_textSelection1.getString().getSize()-1] == '\n')
                            m_selectionRects.back().width = m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()-1).x - m_textSelection1.getPosition().x;
                        else
                            m_selectionRects.back().width = m_textSelection1.findCharacterPos(m_textSelection1.getString().getSize()).x - m_textSelection1.getPosition().x;

                        // There is kerning when the selection is on just this line
                        if (selectionStart.y == selectionEnd.y)
                            m_selectionRects.back().width += kerningSelectionEnd;
                    }

                    // The selection should still be visible even when no character is selected on that line
                    if (m_selectionRects.back().width == 0)
                        m_selectionRects.back().width = 2;

                    for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                    {
                        m_selectionRects.push_back({m_textSelection2.getPosition().x, getPosition().y + padding.top + (i * m_lineHeight), 0, static_cast<float>(m_lineHeight)});

                        if ((!m_lines[i].isEmpty()) && (m_lines[i] != "\n"))
                        {
                            tempText.setString(m_lines[i]);

                            if (tempText.getString()[tempText.getString().getSize()-1] == '\n')
                                m_selectionRects.back().width = tempText.findCharacterPos(tempText.getString().getSize()-1).x;
                            else
                                m_selectionRects.back().width = tempText.findCharacterPos(tempText.getString().getSize()).x;
                        }
                        else
                            m_selectionRects.back().width = 2;
                    }

                    if (m_textSelection2.getString() != "")
                    {
                        tempText.setString(m_lines[selectionEnd.y].substring(0, selectionEnd.x));
                        m_selectionRects.push_back({m_textSelection2.getPosition().x, getPosition().y + padding.top + (selectionEnd.y * m_lineHeight),
                                                    tempText.findCharacterPos(tempText.getString().getSize()).x + kerningSelectionEnd, static_cast<float>(m_lineHeight)});
                    }
                }
            }

            // If there is a scrollbar then move the text depending on the value of the scrollbar
            if (m_scroll)
            {
                for (auto& selectionRect : m_selectionRects)
                    selectionRect.top -= m_scroll->getValue();

                m_textBeforeSelection.setPosition({m_textBeforeSelection.getPosition().x, m_textBeforeSelection.getPosition().y - m_scroll->getValue()});
                m_textSelection1.setPosition({m_textSelection1.getPosition().x, m_textSelection1.getPosition().y - m_scroll->getValue()});
                m_textSelection2.setPosition({m_textSelection2.getPosition().x, m_textSelection2.getPosition().y - m_scroll->getValue()});
                m_textAfterSelection1.setPosition({m_textAfterSelection1.getPosition().x, m_textAfterSelection1.getPosition().y - m_scroll->getValue()});
                m_textAfterSelection2.setPosition({m_textAfterSelection2.getPosition().x, m_textAfterSelection2.getPosition().y - m_scroll->getValue()});

                m_scroll->setPosition({getPosition().x + getSize().x - padding.right - m_scroll->getSize().x, getPosition().y + padding.top});

                m_caretPosition = {m_caretPosition.x, m_caretPosition.y - m_scroll->getValue()};
            }

            m_visibleLines = std::min(static_cast<std::size_t>((getSize().y - padding.top - padding.bottom) / m_lineHeight), m_lines.size());

            // Store which area is visible
            if (m_scroll != nullptr)
            {
                m_topLine = m_scroll->getValue() / m_lineHeight;

                // The scrollbar may be standing between lines in which case one more line is visible
                if (((static_cast<unsigned int>(getSize().y - padding.top - padding.bottom) % m_lineHeight) != 0) || ((m_scroll->getValue() % m_lineHeight) != 0))
                    m_visibleLines++;
            }
            else // There is no scrollbar
            {
                m_topLine = 0;
                m_visibleLines = std::min(static_cast<std::size_t>((getSize().y - padding.top - padding.bottom) / m_lineHeight), m_lines.size());
            }
        }
        else // There is no font, so there can't be calculations
        {
            m_topLine = 0;
            m_visibleLines = 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        // Don't continue when line height is 0
        if (m_lineHeight == 0)
            return;

        // If there is a scrollbar then reinitialize it
        if (m_scroll != nullptr)
        {
            Padding padding = getRenderer()->getScaledPadding();
            m_scroll->setLowValue(static_cast<unsigned int>(getSize().y - padding.top - padding.bottom));
            m_scroll->setSize({m_scroll->getSize().x, getSize().y - padding.top - padding.bottom});
        }

        // The size of the text box has changed, update the text
        rearrangeText(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f TextBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setFont(const Font& font)
    {
        Widget::setFont(font);

        if (font.getFont())
        {
            m_textBeforeSelection.setFont(*font.getFont());
            m_textSelection1.setFont(*font.getFont());
            m_textSelection2.setFont(*font.getFont());
            m_textAfterSelection1.setFont(*font.getFont());
            m_textAfterSelection2.setFont(*font.getFont());
        }

        setTextSize(getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setText(const sf::String& text)
    {
        m_text = text;

        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::addText(const sf::String& text)
    {
        setText(m_text + text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String TextBox::getSelectedText() const
    {
        return m_textSelection1.getString() + m_textSelection2.getString();
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
        if (m_font)
            m_lineHeight = static_cast<unsigned int>(m_font->getLineSpacing(m_textSize));
        else
            m_lineHeight = 0;

        updateSize();
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

    void TextBox::setScrollbar(Scrollbar::Ptr scrollbar)
    {
        m_scroll = scrollbar;

        if (m_scroll)
            updateSize();
        else
            rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr TextBox::getScrollbar()
    {
        return m_scroll;
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

    void TextBox::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        if (m_scroll != nullptr)
            m_scroll->setOpacity(m_opacity);

        getRenderer()->m_backgroundTexture.setColor({getRenderer()->m_backgroundTexture.getColor().r, getRenderer()->m_backgroundTexture.getColor().g, getRenderer()->m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        m_textBeforeSelection.setColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textAfterSelection1.setColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textAfterSelection2.setColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textSelection1.setColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
        m_textSelection2.setColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f TextBox::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag
        m_mouseDown = true;

        // This will be true when the click did not occur on the scrollbar
        bool clickedOnTextBox = true;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_scroll->getValue();

            // Pass the event
            if (m_scroll->mouseOnWidget(x, y))
            {
                m_scroll->leftMousePressed(x, y);
                clickedOnTextBox = false;
            }

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_scroll->getValue())
                updatePosition();
        }

        // If the click occurred on the text box
        if (clickedOnTextBox)
        {
            // Don't continue when line height is 0
            if (m_lineHeight == 0)
                return;

            auto caretPosition = findCaretPosition({x, y});

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
                bool done = false;
                for (std::size_t j = m_selStart.y + 1; j > 0; --j)
                {
                    for (std::size_t i = m_selStart.x; i > 0; --i)
                    {
                        if (selectingWhitespace != isWhitespace(m_lines[m_selStart.y][i-1]))
                        {
                            m_selStart.x = i;
                            done = true;
                            break;
                        }
                        else
                            m_selStart.x = 0;
                    }

                    if (!done)
                    {
                        if (!selectingWhitespace && m_selStart.x == 0)
                        {
                            if (m_selStart.y > 0)
                            {
                                m_selStart.y--;
                                m_selStart.x = m_lines[m_selStart.y].getSize();
                            }
                            else
                                break;
                        }
                    }
                    else
                    {
                        if (m_selStart.x == m_lines[m_selStart.y].getSize())
                        {
                            m_selStart.y++;
                            m_selStart.x = 0;
                        }
                        break;
                    }
                }

                // Move start pointer to the end of the word/whitespace
                done = false;
                for (std::size_t j = m_selEnd.y; j < m_lines.size(); ++j)
                {
                    for (std::size_t i = m_selEnd.x; i < m_lines[m_selEnd.y].getSize(); ++i)
                    {
                        if (selectingWhitespace != isWhitespace(m_lines[m_selEnd.y][i]))
                        {
                            m_selEnd.x = i;
                            done = true;
                            break;
                        }
                        else
                            m_selEnd.x = m_lines[m_selEnd.y].getSize();
                    }

                    if (!done)
                    {
                        if (!selectingWhitespace && m_selEnd.x == m_lines[m_selEnd.y].getSize())
                        {
                            if (m_selEnd.y + 1 < m_lines.size())
                            {
                                m_selEnd.y++;
                                m_selEnd.x = 0;
                            }
                            else
                                break;
                        }
                    }
                    else
                    {
                        if (m_selEnd.x == m_lines[m_selEnd.y].getSize())
                        {
                            m_selEnd.y--;
                            m_selEnd.x = m_lines[m_selEnd.y].getSize();
                        }
                        break;
                    }
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

    void TextBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_scroll != nullptr)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_scroll->m_mouseDown)
            {
                // Don't continue when line height is 0
                if (m_lineHeight == 0)
                    return;

                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Pass the event
                m_scroll->leftMouseReleased(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                {
                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_scroll->setValue(m_scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_scroll->setValue(m_scroll->getValue() + m_lineHeight - (m_scroll->getValue() % m_lineHeight));
                    }
                    else if (m_scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_scroll->setValue(m_scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_scroll->getValue() % m_lineHeight > 0)
                            m_scroll->setValue(m_scroll->getValue() - (m_scroll->getValue() % m_lineHeight));
                        else
                            m_scroll->setValue(m_scroll->getValue() - m_lineHeight);
                    }

                    updatePosition();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Check if you are dragging the thumb of the scrollbar
            if (m_scroll->m_mouseDown)
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_scroll->getValue();

                // Pass the event, even when the mouse is not on top of the scrollbar
                m_scroll->mouseMoved(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_scroll->getValue())
                    updatePosition();
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_scroll->mouseOnWidget(x, y))
                    m_scroll->mouseMoved(x, y);
            }
        }

        // If the mouse is held down then you are selecting text
        if (m_mouseDown && (!m_scroll || !m_scroll->m_mouseDown))
        {
            sf::Vector2<std::size_t> caretPosition = findCaretPosition({x, y});
            if (caretPosition != m_selEnd)
            {
                m_selEnd = caretPosition;
                updateSelectionTexts();
            }

            // Check if the caret is located above or below the view
            if (m_scroll != nullptr)
            {
                if (m_selEnd.y <= m_topLine)
                {
                    m_scroll->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
                    updatePosition();
                }
                else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                {
                    m_scroll->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight) - m_scroll->getLowValue()));
                    updatePosition();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();

        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        switch (event.code)
        {
            case sf::Keyboard::Up:
            {
                if (m_selEnd.y > 0)
                    m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y - m_lineHeight});
                else
                    m_selEnd = {0, 0};

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Down:
            {
                if (m_selEnd.y < m_lines.size()-1)
                    m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y + m_lineHeight});
                else
                    m_selEnd = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Left:
            {
                if (event.control)
                {
                    // Move to the beginning of the word (or to the previous word when already at the beginning)
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
                            if (m_selEnd.y > 0)
                            {
                                m_selEnd.y--;
                                if (!m_lines[m_selEnd.y].isEmpty() && m_lines[m_selEnd.y][m_lines[m_selEnd.y].getSize()-1] == '\n')
                                {
                                    if (!skippedWhitespace)
                                        m_selEnd.x = m_lines[m_selEnd.y].getSize()-1;
                                    else
                                    {
                                        m_selEnd.x = 0;
                                        m_selEnd.y++;
                                        break;
                                    }
                                }
                                else
                                    m_selEnd.x = m_lines[m_selEnd.y].getSize();
                            }
                            else
                            {
                                m_selEnd.x = 0;
                                m_selEnd.y = 0;
                            }
                        }
                        else
                            break;
                    }
                }
                else
                {
                    if (m_selEnd.x > 0)
                        m_selEnd.x--;
                    else
                    {
                        // You are at the left side of a line so move up
                        if (m_selEnd.y > 0)
                        {
                            m_selEnd.y--;
                            m_selEnd.x = m_lines[m_selEnd.y].getSize() - 1;
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
                // You can still move to the right on this line
                if (event.control)
                {
                    // Move to the beginning of the word (or to the previous word when already at the beginning)
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
                                if (m_selEnd.y+1 < m_lines.size())
                                {
                                    m_selEnd.y++;
                                    m_selEnd.x = 0;
                                }
                            }
                            else
                            {
                                if (!m_lines[m_selEnd.y].isEmpty() && (m_lines[m_selEnd.y][m_lines[m_selEnd.y].getSize()-1] == '\n'))
                                    m_selEnd.x = m_lines[m_selEnd.y].getSize() - 1;
                                else
                                    m_selEnd.x = m_lines[m_selEnd.y].getSize();
                            }
                        }
                        else
                            break;
                    }
                }
                else
                {
                    // Move to the next line if you are at the end of the line
                    if ((m_selEnd.x == m_lines[m_selEnd.y].getSize()) || ((m_selEnd.x+1 == m_lines[m_selEnd.y].getSize()) && (m_lines[m_selEnd.y][m_selEnd.x] == '\n')))
                    {
                        if (m_selEnd.y < m_lines.size()-1)
                        {
                            m_selEnd.y++;
                            m_selEnd.x = 0;
                        }
                    }
                    else
                        m_selEnd.x++;
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
                {
                    if (!m_lines[m_selEnd.y].isEmpty() && (m_lines[m_selEnd.y][m_lines[m_selEnd.y].getSize()-1] == '\n'))
                        m_selEnd.x = m_lines[m_selEnd.y].getSize() - 1;
                    else
                        m_selEnd.x = m_lines[m_selEnd.y].getSize();
                }

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
                    Padding padding = getRenderer()->getScaledPadding();
                    auto visibleLines = static_cast<std::size_t>((getSize().y - padding.top - padding.bottom) / m_lineHeight);
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
                    Padding padding = getRenderer()->getScaledPadding();
                    auto visibleLines = static_cast<std::size_t>((getSize().y - padding.top - padding.bottom) / m_lineHeight);
                    if (m_selEnd.y + visibleLines >= m_lines.size() + 2)
                        m_selEnd.y = m_lines.size() - 1;
                    else
                        m_selEnd.y = m_selEnd.y + visibleLines - 2;
                }

                if (!m_lines[m_selEnd.y].isEmpty() && (m_lines[m_selEnd.y][m_lines[m_selEnd.y].getSize()-1] == '\n'))
                    m_selEnd.x = m_lines[m_selEnd.y].getSize() - 1;
                else
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

                // Make sure that we did not select any characters
                if (m_selStart == m_selEnd)
                {
                    // Delete the previous character on this line
                    if (m_selEnd.x > 0)
                    {
                        m_selEnd.x--;
                    }
                    else // We are at the left side of the line
                    {
                        // Delete a character from the line above you
                        if (m_selEnd.y > 0)
                        {
                            m_selEnd.y--;
                            m_selEnd.x = m_lines[m_selEnd.y].getSize() - 1;
                        }
                        else // You are at the beginning of the text
                            break;
                    }

                    m_selStart = m_selEnd;
                    m_text.erase(findTextCaretPosition().second, 1);
                    rearrangeText(true);
                }
                else // When you did select some characters then delete them
                    deleteSelectedCharacters();

                // The caret should be visible again
                m_caretVisible = true;
                m_animationTimeElapsed = {};

                m_callback.text = m_text;
                sendSignal("TextChanged", m_text);
                break;
            }

            case sf::Keyboard::Delete:
            {
                if (m_readOnly)
                    break;

                // Make sure that no text is selected
                if (m_selStart == m_selEnd)
                {
                    // Delete the next character on this line
                    if (m_selEnd.x == m_lines[m_selEnd.y].getSize())
                    {
                        // Delete a character from the line below you
                        if (m_selEnd.y < m_lines.size()-1)
                        {
                            m_selEnd.y++;
                            m_selEnd.x = 0;

                            m_selStart = m_selEnd;
                        }
                        else // You are at the end of the text
                            break;
                    }

                    m_text.erase(findTextCaretPosition().second, 1);
                    rearrangeText(true);
                }
                else // You did select some characters, so remove them
                    deleteSelectedCharacters();

                m_callback.text = m_text;
                sendSignal("TextChanged", m_text);
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
                if (event.control && !event.alt && !event.shift && !event.system)
                    Clipboard::set(m_textSelection1.getString() + m_textSelection2.getString());

                break;
            }

            case sf::Keyboard::X:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    Clipboard::set(m_textSelection1.getString() + m_textSelection2.getString());
                    deleteSelectedCharacters();
                }

                break;
            }

            case sf::Keyboard::V:
            {
                if (event.control && !event.alt && !event.shift && !event.system && !m_readOnly)
                {
                    sf::String clipboardContents = Clipboard::get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selStart != m_selEnd) || (clipboardContents != ""))
                    {
                        deleteSelectedCharacters();

                        m_text.insert(findTextCaretPosition().first, clipboardContents);
                        m_lines[m_selStart.y].insert(m_selStart.x, clipboardContents);

                        m_selStart.x += clipboardContents.getSize();
                        m_selEnd = m_selStart;
                        rearrangeText(true);

                        m_callback.text = m_text;
                        sendSignal("TextChanged", m_text);
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

    void TextBox::textEntered(sf::Uint32 key)
    {
        if (m_readOnly)
            return;

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
            return;

        auto insert = [=]()
        {
            deleteSelectedCharacters();

            std::size_t caretPosition = findTextCaretPosition().first;

            m_text.insert(caretPosition, key);
            m_lines[m_selEnd.y].insert(m_selEnd.x, key);

            m_selStart.x++;
            m_selEnd.x++;

            rearrangeText(true);
        };

        // If there is a scrollbar then inserting can't go wrong
        if (m_scroll)
        {
            insert();
        }
        else // There is no scrollbar, the text may not fit
        {
            // Store the data so that it can be reverted
            sf::String oldText = m_text;
            auto oldSelStart = m_selStart;
            auto oldSelEnd = m_selEnd;

            // Try to insert the character
            insert();

            // Undo the insert if the text does not fit
            if (oldText.getSize() + 1 != m_text.getSize())
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

        m_callback.text = m_text;
        sendSignal("TextChanged", m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseWheelMoved(int delta, int, int)
    {
        // Only do something when there is a scrollbar
        if (m_scroll != nullptr)
        {
            if (m_scroll->getLowValue() < m_scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_scroll->setValue(m_scroll->getValue() + (static_cast<unsigned int>(-delta) * m_lineHeight));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * m_lineHeight;

                    // Scroll up
                    if (change < m_scroll->getValue())
                        m_scroll->setValue(m_scroll->getValue() - change);
                    else
                        m_scroll->setValue(0);
                }
            }
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::widgetFocused()
    {
    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(true);
    #endif

        Widget::widgetFocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_selStart != m_selEnd)
        {
            m_selStart = m_selEnd;
            updateSelectionTexts();
        }

    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(false);
    #endif

        Widget::widgetUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2<std::size_t> TextBox::findCaretPosition(sf::Vector2f position)
    {
        Padding padding = getRenderer()->getScaledPadding();

        position.x -= getPosition().x + padding.left;
        position.y -= getPosition().y + padding.top;

        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_font == nullptr))
            return sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find on which line the mouse is
        std::size_t lineNumber;
        if (m_scroll)
        {
            if (position.y + m_scroll->getValue() < 0)
                return {0, 0};

            lineNumber = static_cast<std::size_t>(std::floor((position.y + m_scroll->getValue()) / m_lineHeight));
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
        float width = 0;
        sf::Uint32 prevChar = 0;
        for (std::size_t i = 0; i < m_lines[lineNumber].getSize(); ++i)
        {
            float charWidth;
            sf::Uint32 curChar = m_lines[lineNumber][i];
            if (curChar == '\n')
                return sf::Vector2<std::size_t>(m_lines[lineNumber].getSize() - 1, lineNumber);
            else if (curChar == '\t')
                charWidth = static_cast<float>(m_font->getGlyph(' ', getTextSize(), false).advance) * 4;
            else
                charWidth = static_cast<float>(m_font->getGlyph(curChar, getTextSize(), false).advance);

            float kerning = static_cast<float>(m_font->getKerning(prevChar, curChar, getTextSize()));
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

    std::pair<std::size_t, std::size_t> TextBox::findTextCaretPosition()
    {
        // This function is used to count the amount of characters spread over several lines
        auto count = [this](std::size_t low, std::size_t high)
        {
            std::size_t counter = 0;
            for (std::size_t i = low; i < high; ++i)
            {
                counter += m_lines[i].getSize();
/*
                // THIS CODE SHOULD ONLY BE UNCOMMENTED TOGETHER WITH THE CODE IN rearrangeText
                // If the next line starts with just a space, then the space need not be visible
                if ((counter + 1 < m_text.getSize()) && (m_text[counter] == ' ') && (!isWhitespace(m_text[counter + 1])))
                {
                    if ((counter == 0) || (!isWhitespace(m_text[counter-1])))
                        counter++;
                }
*/
            }
            return counter;
        };

        std::pair<std::size_t, std::size_t> textCaretPosition{0, 0};

        // Find the location in the text where the selection starts
        textCaretPosition.first = count(0, m_selStart.y) + m_selStart.x;

        // Find the location in the text where the selection ends
        if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
            textCaretPosition.second = textCaretPosition.first - count(m_selEnd.y, m_selStart.y) + m_selEnd.x - m_selStart.x;
        else
            textCaretPosition.second = textCaretPosition.first + count(m_selStart.y, m_selEnd.y) + m_selEnd.x - m_selStart.x;

        return textCaretPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::deleteSelectedCharacters()
    {
        if (m_selStart != m_selEnd)
        {
            auto textCaretPosition = findTextCaretPosition();

            if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
            {
                m_text.erase(textCaretPosition.second, textCaretPosition.first - textCaretPosition.second);
                m_selStart = m_selEnd;
            }
            else
            {
                m_text.erase(textCaretPosition.first, textCaretPosition.second - textCaretPosition.first);
                m_selEnd = m_selStart;
            }

            rearrangeText(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::rearrangeText(bool keepSelection)
    {
        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_font == nullptr))
            return;

        sf::Vector2<std::size_t> newSelStart;
        sf::Vector2<std::size_t> newSelEnd;
        bool newSelStartFound = false;
        bool newSelEndFound = false;
        auto textCaretPosition = findTextCaretPosition();
        Padding padding = getRenderer()->getScaledPadding();

        // Find the maximum width of one line
        float maxLineWidth = std::max(0.f, getSize().x - padding.left - padding.right);
        if (m_scroll && (!m_scroll->getAutoHide() || (m_scroll->getMaximum() > m_scroll->getLowValue())))
            maxLineWidth = std::max(0.f, maxLineWidth - m_scroll->getSize().x);

        // Split the text over multiple lines
        m_lines.clear();
        std::size_t index = 0;
        while (index < m_text.getSize())
        {
            std::size_t oldIndex = index;

            float width = 0;
            sf::Uint32 prevChar = 0;
            for (std::size_t i = index; i < m_text.getSize(); ++i)
            {
                float charWidth;
                sf::Uint32 curChar = m_text[i];
                if (curChar == '\n')
                {
                    index++;
                    break;
                }
                else if (curChar == '\t')
                    charWidth = static_cast<float>(m_font->getGlyph(' ', getTextSize(), false).advance) * 4;
                else
                    charWidth = static_cast<float>(m_font->getGlyph(curChar, getTextSize(), false).advance);

                float kerning = static_cast<float>(m_font->getKerning(prevChar, curChar, getTextSize()));
                if (width + charWidth + kerning <= maxLineWidth)
                {
                    width += charWidth + kerning;
                    index++;
                }
                else
                    break;

                prevChar = curChar;
            }

            // Every line contains at least one character
            if (index == oldIndex)
                index++;

            // Implement the word-wrap
            if (m_text[index-1] != '\n')
            {
                std::size_t indexWithoutWordWrap = index;

                if ((index < m_text.getSize()) && (!isWhitespace(m_text[index])))
                {
                    std::size_t wordWrapCorrection = 0;
                    while ((index > oldIndex) && (!isWhitespace(m_text[index - 1])))
                    {
                        wordWrapCorrection++;
                        index--;
                    }

                    // The word can't be split but there is no other choice, it does not fit on the line
                    if ((index - oldIndex) <= wordWrapCorrection)
                        index = indexWithoutWordWrap;
                }
            }

            if (!newSelStartFound && (index >= textCaretPosition.first))
            {
                newSelStartFound = true;

                // When standing behind a newline, you should be at the line below this one
                if ((textCaretPosition.first > 0) && (m_text[textCaretPosition.first-1] == '\n'))
                    newSelStart = sf::Vector2<std::size_t>(0, m_lines.size() + 1);
                else
                {
                    // The text caret position is the same when the caret is at the beginning or at the end of a line
                    if ((index == textCaretPosition.first) && (index < m_text.getSize()) && (m_selStart.x == 0))
                        newSelStart = sf::Vector2<std::size_t>(0, m_lines.size() + 1);
                    else
                        newSelStart = sf::Vector2<std::size_t>(textCaretPosition.first - oldIndex, m_lines.size());
                }
            }
            if (!newSelEndFound && (index >= textCaretPosition.second))
            {
                newSelEndFound = true;

                // When standing behind a newline, you should be at the line below this one
                if ((textCaretPosition.second > 0) && (m_text[textCaretPosition.second-1] == '\n'))
                    newSelEnd = sf::Vector2<std::size_t>(0, m_lines.size() + 1);
                else
                {
                    // The text caret position is the same when the caret is at the beginning or at the end of a line
                    if ((index == textCaretPosition.second) && (index < m_text.getSize()) && (m_selEnd.x == 0))
                        newSelEnd = sf::Vector2<std::size_t>(0, m_lines.size() + 1);
                    else
                        newSelEnd = sf::Vector2<std::size_t>(textCaretPosition.second - oldIndex, m_lines.size());
                }
            }

            m_lines.push_back(m_text.substring(oldIndex, index - oldIndex));
/*
            // THIS CODE MESSES UP newSelStart AND newSelEnd
            // THIS CODE SHOULD ONLY BE UNCOMMENTED TOGETHER WITH THE CODE IN findTextCaretPosition
            // If the next line starts with just a space, then the space need not be visible
            if ((index + 1 < m_text.getSize()) && (m_text[index] == ' ') && (!isWhitespace(m_text[index + 1])))
            {
                if ((index == 0) || (!isWhitespace(m_text[index-1])))
                    index++;
            }
*/
        }

        // There is always one line, even if it is empty
        if (m_lines.empty())
            m_lines.push_back("");

        // If the last line ends with a newline, then add an extra line
        if ((!m_lines.back().isEmpty()) && (m_lines.back()[m_lines.back().getSize()-1] == '\n'))
            m_lines.push_back("");

        // Correct the caret positions
        if (keepSelection && newSelStartFound && newSelEndFound)
        {
            m_selStart = newSelStart;
            m_selEnd = newSelEnd;
        }
        else // The text has changed too much, the selection can't be kept
        {
            m_selStart = sf::Vector2<std::size_t>(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
            m_selEnd = m_selStart;
        }

        // Tell the scrollbar how many pixels the text contains
        if (m_scroll != nullptr)
        {
            bool invisibleScrollbar = (m_scroll->getMaximum() <= m_scroll->getLowValue());

            m_scroll->setMaximum(static_cast<unsigned int>(m_lines.size() * m_lineHeight));

            // We may have to recalculate what we just calculated if the scrollbar just appeared or disappeared
            if (m_scroll->getAutoHide())
            {
                if (invisibleScrollbar != (m_scroll->getMaximum() <= m_scroll->getLowValue()))
                {
                    rearrangeText(true);
                    return;
                }
            }
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
            {
                if (((m_lines[i] != "") && (m_lines[i][m_lines[i].getSize()-1] == '\n')) || (i == m_lines.size()-1))
                    displayedText += m_lines[i];
                else
                    displayedText += m_lines[i] + "\n";
            }

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
                {
                    if ((m_lines[i] != "") && (m_lines[i][m_lines[i].getSize()-1] == '\n'))
                        string += m_lines[i];
                    else
                        string += m_lines[i] + "\n";
                }

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
                if (!m_lines[selectionStart.y].isEmpty() && (m_lines[selectionStart.y][m_lines[selectionStart.y].getSize()-1] != '\n') && (selectionEnd.y > selectionStart.y))
                    m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, m_lines[selectionStart.y].getSize() - selectionStart.x) + '\n');
                else
                    m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, m_lines[selectionStart.y].getSize() - selectionStart.x));

                sf::String string;
                for (std::size_t i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                {
                    if ((m_lines[i] != "") && (m_lines[i][m_lines[i].getSize()-1] == '\n'))
                        string += m_lines[i];
                    else
                        string += m_lines[i] + "\n";
                }

                string += m_lines[selectionEnd.y].substring(0, selectionEnd.x);

                m_textSelection2.setString(string);
            }

            // Set the text after the selection
            {
                m_textAfterSelection1.setString(m_lines[selectionEnd.y].substring(selectionEnd.x, m_lines[selectionEnd.y].getSize() - selectionEnd.x));

                sf::String string;
                for (std::size_t i = selectionEnd.y + 1; i < m_lines.size(); ++i)
                {
                    if (((m_lines[i] != "") && (m_lines[i][m_lines[i].getSize()-1] == '\n')) || (i == m_lines.size()-1))
                        string += m_lines[i];
                    else
                        string += m_lines[i] + "\n";
                }

                m_textAfterSelection2.setString(string);
            }
        }

        // Check if the caret is located above or below the view
        if (m_scroll != nullptr)
        {
            if (m_selEnd.y <= m_topLine)
                m_scroll->setValue(static_cast<unsigned int>(m_selEnd.y * m_lineHeight));
            else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                m_scroll->setValue(static_cast<unsigned int>(((m_selEnd.y + 1) * m_lineHeight) - m_scroll->getLowValue()));
        }

        updatePosition();
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

    void TextBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders(2, 2, 2, 2);
        getRenderer()->setTextColor({0, 0, 0});
        getRenderer()->setSelectedTextColor({255, 255, 255});
        getRenderer()->setCaretColor({0, 0, 0});
        getRenderer()->setBackgroundColor({255, 255, 255});
        getRenderer()->setSelectedTextBackgroundColor({0, 110, 255});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackgroundTexture({});
        getRenderer()->setCaretWidth(2);

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background and borders
        getRenderer()->draw(target, states);

        // Draw the contents of the text box
        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            Padding padding = getRenderer()->getScaledPadding();
            Clipping clipping{target, states, {getPosition().x + padding.left, getPosition().y + padding.top}, {getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}};

            // Draw the background of the selected text
            for (auto& selectionRect : m_selectionRects)
            {
                sf::RectangleShape rect{{selectionRect.width, selectionRect.height}};
                rect.setPosition({selectionRect.left, selectionRect.top});
                rect.setFillColor(calcColorOpacity(getRenderer()->m_selectedTextBgrColor, getOpacity()));
                target.draw(rect, states);
            }

            // Draw the text
            target.draw(m_textBeforeSelection, states);
            if (m_selStart != m_selEnd)
            {
                target.draw(m_textSelection1, states);
                target.draw(m_textSelection2, states);
                target.draw(m_textAfterSelection1, states);
                target.draw(m_textAfterSelection2, states);
            }

            // Only draw the caret if it has a width
            if (getRenderer()->m_caretWidth > 0)
            {
                // Only draw it when needed
                if (m_focused && m_caretVisible && (getRenderer()->m_caretWidth > 0))
                {
                    sf::RectangleShape caret({getRenderer()->m_caretWidth, static_cast<float>(m_lineHeight)});
                    caret.setPosition(m_caretPosition.x - (getRenderer()->m_caretWidth * 0.5f), static_cast<float>(m_caretPosition.y));
                    caret.setFillColor(calcColorOpacity(getRenderer()->m_caretColor, getOpacity()));
                    target.draw(caret, states);
                }
            }
        }

        // Draw the scrollbar if there is one
        if (m_scroll != nullptr)
            target.draw(*m_scroll, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextcolor")
            setSelectedTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextbackgroundcolor")
            setSelectedTextBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "caretcolor")
            setCaretColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundimage")
            setBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "scrollbar")
        {
            if (toLower(value) == "none")
                m_textBox->setScrollbar(nullptr);
            else
            {
                if (m_textBox->getTheme() == nullptr)
                    throw Exception{"Failed to load scrollbar, TextBox has no connected theme to load the scrollbar with"};

                m_textBox->setScrollbar(m_textBox->getTheme()->internalLoad(m_textBox->m_primaryLoadingParameter,
                                                                            Deserializer::deserialize(ObjectConverter::Type::String, value).getString()));
            }
        }
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else if (property == "padding")
                setPadding(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "selectedtextcolor")
                setSelectedTextColor(value.getColor());
            else if (property == "selectedtextbackgroundcolor")
                setSelectedTextBackgroundColor(value.getColor());
            else if (property == "caretcolor")
                setCaretColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backgroundimage")
                setBackgroundTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "scrollbar")
            {
                if (toLower(value.getString()) == "none")
                    m_textBox->setScrollbar(nullptr);
                else
                {
                    if (m_textBox->getTheme() == nullptr)
                        throw Exception{"Failed to load scrollbar, TextBox has no connected theme to load the scrollbar with"};

                    m_textBox->setScrollbar(m_textBox->getTheme()->internalLoad(m_textBox->getPrimaryLoadingParameter(), value.getString()));
                }
            }
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter TextBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "padding")
            return m_padding;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "selectedtextcolor")
            return m_selectedTextColor;
        else if (property == "selectedtextbackgroundcolor")
            return m_selectedTextBgrColor;
        else if (property == "caretcolor")
            return m_caretColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "backgroundimage")
            return m_backgroundTexture;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> TextBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_backgroundTexture.isLoaded())
            pairs["BackgroundImage"] = m_backgroundTexture;
        else
            pairs["BackgroundColor"] = m_backgroundColor;

        pairs["TextColor"] = m_textColor;
        pairs["SelectedTextColor"] = m_selectedTextColor;
        pairs["SelectedTextBackgroundColor"] = m_selectedTextBgrColor;
        pairs["CaretColor"] = m_caretColor;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setBackgroundColor(const Color& color)
    {
        m_backgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setTextColor(const Color& color)
    {
        m_textColor = color;
        m_textBox->m_textBeforeSelection.setColor(calcColorOpacity(m_textColor, m_textBox->getOpacity()));
        m_textBox->m_textAfterSelection1.setColor(calcColorOpacity(m_textColor, m_textBox->getOpacity()));
        m_textBox->m_textAfterSelection2.setColor(calcColorOpacity(m_textColor, m_textBox->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setSelectedTextColor(const Color& color)
    {
        m_selectedTextColor = color;
        m_textBox->m_textSelection1.setColor(calcColorOpacity(m_selectedTextColor, m_textBox->getOpacity()));
        m_textBox->m_textSelection2.setColor(calcColorOpacity(m_selectedTextColor, m_textBox->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setSelectedTextBackgroundColor(const Color& color)
    {
        m_selectedTextBgrColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setBorderColor(const Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setCaretColor(const Color& caretColor)
    {
        m_caretColor = caretColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setCaretWidth(float width = 2)
    {
        m_caretWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setBackgroundTexture(const Texture& texture)
    {
        m_backgroundTexture = texture;
        if (m_backgroundTexture.isLoaded())
        {
            m_backgroundTexture.setPosition(m_textBox->getPosition());
            m_backgroundTexture.setSize(m_textBox->getSize());
            m_backgroundTexture.setColor({m_backgroundTexture.getColor().r, m_backgroundTexture.getColor().g, m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_textBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);
        m_textBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        if (m_backgroundTexture.isLoaded())
            target.draw(m_backgroundTexture, states);
        else
        {
            sf::RectangleShape background(m_textBox->getSize());
            background.setPosition(m_textBox->getPosition());
            background.setFillColor(calcColorOpacity(m_backgroundColor, m_textBox->getOpacity()));
            target.draw(background, states);
        }

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f size = m_textBox->getSize();
            sf::Vector2f position = m_textBox->getPosition();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_textBox->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding TextBoxRenderer::getScaledPadding() const
    {
        Padding padding = getPadding();
        Padding scaledPadding = padding;

        auto& texture = m_backgroundTexture;
        if (texture.isLoaded())
        {
            switch (texture.getScalingType())
            {
            case Texture::ScalingType::Normal:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Horizontal:
                scaledPadding.left = padding.left * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.right = padding.right * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Vertical:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.bottom = padding.bottom * (texture.getSize().x / texture.getImageSize().x);
                break;

            case Texture::ScalingType::NineSlice:
                break;
            }
        }

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> TextBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<TextBoxRenderer>(*this);
        renderer->m_textBox = static_cast<TextBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
