/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Scrollbar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/TextBox.hpp>
#include <TGUI/Clipboard.hpp>

#include <SFML/OpenGL.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox()
    {
        m_callback.widgetType = Type_TextBox;
        m_animatedWidget = true;
        m_draggableWidget = true;

        m_renderer = std::make_shared<TextBoxRenderer>(this);

        getRenderer()->setTextColor({0, 0, 0});
        getRenderer()->setSelectedTextColor({255, 255, 255});

        setSize({360, 200});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TextBox::TextBox(const TextBox& scrollbarToCopy) :
        Widget               {scrollbarToCopy},
        m_text               {scrollbarToCopy.m_text},
        m_textSize           {scrollbarToCopy.m_textSize},
        m_lineHeight         {scrollbarToCopy.m_lineHeight},
        m_lines              {scrollbarToCopy.m_lines},
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
        m_selectionRects     {scrollbarToCopy.m_selectionRects},
        m_possibleDoubleClick{scrollbarToCopy.m_possibleDoubleClick},
        m_readOnly           {scrollbarToCopy.m_readOnly}
    {
        if (scrollbarToCopy.m_scroll != nullptr)
            m_scroll = Scrollbar::copy(scrollbarToCopy.m_scroll);
        else
            m_scroll = nullptr;
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

    TextBox::Ptr TextBox::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto textBox = std::make_shared<TextBox>();

        if (themeFileFilename != "")
        {
            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = themeFileFilename.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = themeFileFilename.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    if (it->first == "scrollbar")
                    {
                        if (toLower(it->second) != "none")
                        {
                            if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                                throw Exception{"Failed to parse value for 'Scrollbar' property."};

                            textBox->getRenderer()->setScrollbar(themeFileFilename, it->second.substr(1, it->second.length()-2));
                        }
                        else // There should be no scrollbar
                            textBox->removeScrollbar();
                    }
                    else
                        textBox->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }
        }

        return textBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setPosition(const Layout& position)
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
        else // Recalculate everything
        {
            getRenderer()->m_backgroundTexture.setPosition(getPosition());

            const sf::Font* font = m_textBeforeSelection.getFont();
            sf::Text tempText{"kg", *font, getTextSize()};
            float textShiftY = tempText.getLocalBounds().top;
            Padding padding = getRenderer()->getScaledPadding();

            // Position the caret
            {
                tempText.setString(m_lines[m_selEnd.y].substring(0, m_selEnd.x));

                float kerning = 0;
                if ((m_selEnd.x > 0) && (m_selEnd.x < m_lines[m_selEnd.y].getSize()))
                    kerning = font->getKerning(m_lines[m_selEnd.y][m_selEnd.x-1], m_lines[m_selEnd.y][m_selEnd.x], m_textSize);

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
                    kerningSelectionStart = font->getKerning(m_lines[selectionStart.y][selectionStart.x-1], m_lines[selectionStart.y][selectionStart.x], m_textSize);

                float kerningSelectionEnd = 0;
                if ((selectionEnd.x > 0) && (selectionEnd.x < m_lines[selectionEnd.y].getSize()))
                    kerningSelectionEnd = font->getKerning(m_lines[selectionEnd.y][selectionEnd.x-1], m_lines[selectionEnd.y][selectionEnd.x], m_textSize);

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
                    if ((!m_lines[selectionStart.y].isEmpty()) && (m_lines[selectionStart.y] != "\n"))
                    {
                        m_selectionRects.push_back({m_textSelection1.getPosition().x,
                                                    getPosition().y + padding.top + (selectionStart.y * m_lineHeight),
                                                    m_textSelection1.findCharacterPos(m_lines[selectionStart.y].getSize() - selectionStart.x).x - m_textSelection1.getPosition().x,
                                                    static_cast<float>(m_lineHeight)});

                        // There is kerning when the selection is on just this line
                        if (selectionStart.y == selectionEnd.y)
                            m_selectionRects.back().width += kerningSelectionEnd;
                    }
                    else
                        m_selectionRects.push_back({m_textSelection1.getPosition().x, getPosition().y + padding.top + (selectionStart.y * m_lineHeight), 2, static_cast<float>(m_lineHeight)});

                    for (unsigned int i = selectionStart.y + 1; i < selectionEnd.y; ++i)
                    {
                        if ((!m_lines[i].isEmpty()) && (m_lines[i] != "\n"))
                        {
                            tempText.setString(m_lines[i]);
                            m_selectionRects.push_back({m_textSelection2.getPosition().x, getPosition().y + padding.top + (i * m_lineHeight),
                                                        tempText.findCharacterPos(tempText.getString().getSize()).x, static_cast<float>(m_lineHeight)});
                        }
                        else
                            m_selectionRects.push_back({m_textSelection2.getPosition().x, getPosition().y + padding.top + (i * m_lineHeight), 2, static_cast<float>(m_lineHeight)});
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

            m_visibleLines = std::min<unsigned int>((getSize().y - padding.top - padding.bottom) / m_lineHeight, m_lines.size());

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
                m_visibleLines = std::min<unsigned int>((getSize().y - padding.top - padding.bottom) / m_lineHeight, m_lines.size());
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setSize(const Layout& size)
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

    void TextBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_textSize = size;

        // There is a minimum text size
        if (m_textSize < 8)
            m_textSize = 8;

        // Change the text size
        m_textBeforeSelection.setCharacterSize(m_textSize);
        m_textSelection1.setCharacterSize(m_textSize);
        m_textSelection2.setCharacterSize(m_textSize);
        m_textAfterSelection1.setCharacterSize(m_textSize);
        m_textAfterSelection2.setCharacterSize(m_textSize);

        // Calculate the height of one line
        if (m_textBeforeSelection.getFont())
            m_lineHeight = m_textBeforeSelection.getFont()->getLineSpacing(m_textSize);

        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setMaximumCharacters(unsigned int maxChars)
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

    void TextBox::removeScrollbar()
    {
        m_scroll = nullptr;
        rearrangeText(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        if (m_scroll != nullptr)
            m_scroll->setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TextBox::mouseOnWidget(float x, float y)
    {
        // Pass the event to the scrollbar (if there is one)
        if (m_scroll != nullptr)
            m_scroll->mouseOnWidget(x, y);

        // Check if the mouse is on top of the text box
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;
        else // The mouse is not on top of the text box
        {
            if (m_mouseHover)
                mouseLeftWidget();

            return false;
        }
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

                // Select the whole text
                m_selStart = {0, 0};
                m_selEnd = sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
            }
            else // No double clicking
            {
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
            if (m_scroll->m_mouseDown == true)
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

        m_mouseDown = false;
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
            sf::Vector2u caretPosition = findCaretPosition({x, y});
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
                    m_scroll->setValue(m_selEnd.y * m_lineHeight);
                    updatePosition();
                }
                else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                {
                    m_scroll->setValue(((m_selEnd.y + 1) * m_lineHeight) - m_scroll->getLowValue());
                    updatePosition();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNotOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::mouseNoLongerDown()
    {
        m_mouseDown = false;

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

                m_selStart = m_selEnd;
                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Down:
            {
                if (m_selEnd.y < m_lines.size()-1)
                    m_selEnd = findCaretPosition({m_caretPosition.x, m_caretPosition.y + m_lineHeight});
                else
                    m_selEnd = sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

                m_selStart = m_selEnd;
                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Left:
            {
                // Just undo the selection when some text is selected
                if (m_selStart != m_selEnd)
                {
                    if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                        m_selStart = m_selEnd;
                    else
                        m_selEnd = m_selStart;
                }
                else // When we did not select any text so move left
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
                        else // You are at the beginning of the text
                            break;
                    }

                    m_selStart = m_selEnd;
                }

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Right:
            {
                // Just undo the selection when some text is selected
                if (m_selStart != m_selEnd)
                {
                    if ((m_selStart.y > m_selEnd.y) || ((m_selStart.y == m_selEnd.y) && (m_selStart.x > m_selEnd.x)))
                        m_selEnd = m_selStart;
                    else
                        m_selStart = m_selEnd;
                }
                else // When we did not select any text so move right
                {
                    // Delete a character from the line below you if you are at the end of the line
                    if (m_selEnd.x == m_lines[m_selEnd.y].getSize())
                    {
                        if (m_selEnd.y < m_lines.size()-1)
                        {
                            m_selEnd.y++;
                            m_selEnd.x = 0;
                        }
                        else // You are at the end of the text
                            break;
                    }
                    else // You can still move to the right on this line
                        m_selEnd.x++;

                    m_selStart = m_selEnd;
                }

                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::Home:
            {
                m_selStart = {0, 0};
                m_selEnd = {0, 0};
                updateSelectionTexts();
                break;
            }

            case sf::Keyboard::End:
            {
                m_selStart = sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
                m_selEnd = m_selStart;
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

                // Add the callback (if the user requested it)
                if (m_callbackFunctions[TextChanged].empty() == false)
                {
                    m_callback.trigger = TextChanged;
                    m_callback.text    = m_text;
                    addCallback();
                }

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

                // Add the callback (if the user requested it)
                if (m_callbackFunctions[TextChanged].empty() == false)
                {
                    m_callback.trigger = TextChanged;
                    m_callback.text    = m_text;
                    addCallback();
                }

                break;
            }

            case sf::Keyboard::A:
            {
                if (event.control)
                {
                    m_selStart = {0, 0};
                    m_selEnd = sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
                    updateSelectionTexts();
                }

                break;
            }

            case sf::Keyboard::C:
            {
                if (event.control)
                    TGUI_Clipboard.set(m_textSelection1.getString() + m_textSelection2.getString());

                break;
            }

            case sf::Keyboard::X:
            {
                if (event.control && !m_readOnly)
                {
                    TGUI_Clipboard.set(m_textSelection1.getString() + m_textSelection2.getString());
                    deleteSelectedCharacters();
                }

                break;
            }

            case sf::Keyboard::V:
            {
                if (event.control && !m_readOnly)
                {
                    sf::String clipboardContents = TGUI_Clipboard.get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selStart != m_selEnd) || (clipboardContents != ""))
                    {
                        deleteSelectedCharacters();

                        m_text.insert(findTextCaretPosition().first, clipboardContents);
                        m_lines[m_selStart.y].insert(m_selStart.x, clipboardContents);

                        m_selStart.x += clipboardContents.getSize();
                        m_selEnd = m_selStart;
                        rearrangeText(true);

                        // Add the callback (if the user requested it)
                        if (m_callbackFunctions[TextChanged].empty() == false)
                        {
                            m_callback.trigger = TextChanged;
                            m_callback.text    = m_text;
                            addCallback();
                        }
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

            unsigned int caretPosition = findTextCaretPosition().first;

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
            sf::Vector2u oldSelStart = m_selStart;
            sf::Vector2u oldSelEnd = m_selEnd;

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

        // Add the callback (if the user requested it)
        if (m_callbackFunctions[TextChanged].empty() == false)
        {
            m_callback.trigger = TextChanged;
            m_callback.text    = m_text;
            addCallback();
        }
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
                    m_scroll->setValue(m_scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_lineHeight / 2)));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (m_lineHeight / 2);

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

    void TextBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_selStart != m_selEnd)
        {
            m_selStart = m_selEnd;
            updateSelectionTexts();
        }

        Widget::widgetUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2u TextBox::findCaretPosition(sf::Vector2f position)
    {
        Padding padding = getRenderer()->getScaledPadding();

        position.x -= getPosition().x + padding.left;
        position.y -= getPosition().y + padding.top;

        // Don't continue when line height is 0 or when there is no font yet
        if ((m_lineHeight == 0) || (m_textBeforeSelection.getFont() == nullptr))
            return sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find on which line the mouse is
        unsigned int lineNumber;
        if (m_scroll)
        {
            if (position.y + m_scroll->getValue() < 0)
                return {0, 0};

            lineNumber = static_cast<unsigned int>(std::floor((position.y + m_scroll->getValue()) / m_lineHeight));
        }
        else
        {
            if (position.y < 0)
                return {0, 0};

            lineNumber = static_cast<unsigned int>(std::floor(position.y / m_lineHeight));
        }

        // Check if you clicked behind everything
        if (lineNumber + 1 > m_lines.size())
            return sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);

        // Find between which character the mouse is standing
        float width = 0;
        sf::Uint32 prevChar = 0;
        for (unsigned int i = 0; i < m_lines[lineNumber].getSize(); ++i)
        {
            float charWidth;
            sf::Uint32 curChar = m_lines[lineNumber][i];
            if (curChar == '\n')
                return sf::Vector2u(m_lines[lineNumber].getSize() - 1, lineNumber);
            else if (curChar == '\t')
                charWidth = static_cast<float>(m_textBeforeSelection.getFont()->getGlyph(' ', getTextSize(), false).advance) * 4;
            else
                charWidth = static_cast<float>(m_textBeforeSelection.getFont()->getGlyph(curChar, getTextSize(), false).advance);

            float kerning = static_cast<float>(m_textBeforeSelection.getFont()->getKerning(prevChar, curChar, getTextSize()));
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
        return sf::Vector2u(m_lines[lineNumber].getSize(), lineNumber);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::pair<unsigned int, unsigned int> TextBox::findTextCaretPosition()
    {
        // This function is used to count the amount of characters spread over several lines
        auto count = [this](unsigned int low, unsigned int high)
        {
            unsigned int counter = 0;
            for (unsigned int i = low; i < high; ++i)
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

        std::pair<unsigned int, unsigned int> textCaretPosition{0, 0};

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
        if ((m_lineHeight == 0) || (m_textBeforeSelection.getFont() == nullptr))
            return;

        sf::Vector2u newSelStart;
        sf::Vector2u newSelEnd;
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
        unsigned int index = 0;
        while (index < m_text.getSize())
        {
            unsigned int oldIndex = index;

            float width = 0;
            sf::Uint32 prevChar = 0;
            for (unsigned int i = index; i < m_text.getSize(); ++i)
            {
                float charWidth;
                sf::Uint32 curChar = m_text[i];
                if (curChar == '\n')
                {
                    index++;
                    break;
                }
                else if (curChar == '\t')
                    charWidth = static_cast<float>(m_textBeforeSelection.getFont()->getGlyph(' ', getTextSize(), false).advance) * 4;
                else
                    charWidth = static_cast<float>(m_textBeforeSelection.getFont()->getGlyph(curChar, getTextSize(), false).advance);

                float kerning = static_cast<float>(m_textBeforeSelection.getFont()->getKerning(prevChar, curChar, getTextSize()));
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
                unsigned int indexWithoutWordWrap = index;

                if ((index < m_text.getSize()) && (!isWhitespace(m_text[index])))
                {
                    unsigned int wordWrapCorrection = 0;
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
                    newSelStart = sf::Vector2u(0, m_lines.size() + 1);
                else
                {
                    // The text caret position is the same when the caret is at the beginning or at the end of a line
                    if ((index == textCaretPosition.first) && (index < m_text.getSize()) && (m_selStart.x == 0))
                        newSelStart = sf::Vector2u(0, m_lines.size() + 1);
                    else
                        newSelStart = sf::Vector2u(textCaretPosition.first - oldIndex, m_lines.size());
                }
            }
            if (!newSelEndFound && (index >= textCaretPosition.second))
            {
                newSelEndFound = true;

                // When standing behind a newline, you should be at the line below this one
                if ((textCaretPosition.second > 0) && (m_text[textCaretPosition.second-1] == '\n'))
                    newSelEnd = sf::Vector2u(0, m_lines.size() + 1);
                else
                {
                    // The text caret position is the same when the caret is at the beginning or at the end of a line
                    if ((index == textCaretPosition.second) && (index < m_text.getSize()) && (m_selEnd.x == 0))
                        newSelEnd = sf::Vector2u(0, m_lines.size() + 1);
                    else
                        newSelEnd = sf::Vector2u(textCaretPosition.second - oldIndex, m_lines.size());
                }
            }

            m_lines.push_back(m_text.substring(oldIndex, index - oldIndex));
/*
            // THIS CODE MESSES UP newSelStart AND newSelEnd
            // THIS CODE SHOULD ONLY BE UNCOMMENTED TOGETHER WITH THE CODE IN rearrangeText
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
            m_selStart = sf::Vector2u(m_lines[m_lines.size()-1].getSize(), m_lines.size()-1);
            m_selEnd = m_selStart;
        }

        // Tell the scrollbar how many pixels the text contains
        if (m_scroll != nullptr)
        {
            bool invisibleScrollbar = (m_scroll->getMaximum() <= m_scroll->getLowValue());

            m_scroll->setMaximum(m_lines.size() * m_lineHeight);

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
        Padding padding = getRenderer()->getScaledPadding();

        float maxLineWidth = std::max(0.f, getSize().x - padding.left - padding.right);
        if (m_scroll && (!m_scroll->getAutoHide() || (m_scroll->getMaximum() > m_scroll->getLowValue())))
            maxLineWidth = std::max(0.f, maxLineWidth - m_scroll->getSize().x);

        // If there is no selection then just put the whole text in m_textBeforeSelection
        if (m_selStart == m_selEnd)
        {
            sf::String displayedText;
            for (unsigned int i = 0; i < m_lines.size(); ++i)
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
                for (unsigned int i = 0; i < selectionStart.y; ++i)
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
                m_textSelection1.setString(m_lines[selectionStart.y].substring(selectionStart.x, m_lines[selectionStart.y].getSize() - selectionStart.x));

                sf::String string;
                for (unsigned int i = selectionStart.y + 1; i < selectionEnd.y; ++i)
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
                for (unsigned int i = selectionEnd.y + 1; i < m_lines.size(); ++i)
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
                m_scroll->setValue(m_selEnd.y * m_lineHeight);
            else if (m_selEnd.y + 1 >= m_topLine + m_visibleLines)
                m_scroll->setValue(((m_selEnd.y + 1) * m_lineHeight) - m_scroll->getLowValue());
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_textBeforeSelection.getFont() && m_parent->getGlobalFont())
            getRenderer()->setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBox::update()
    {
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

    void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        Padding padding = getRenderer()->getScaledPadding();

        // Get the global position
        sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + padding.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                        ((getAbsolutePosition().y + padding.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        sf::Vector2f bottomRightPosition = {(getAbsolutePosition().x + getSize().x - padding.right - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                            (getAbsolutePosition().y + getSize().y - padding.bottom - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        // Draw the background and borders
        getRenderer()->draw(target, states);

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the background of the selected text
        for (auto& selectionRect : m_selectionRects)
        {
            sf::RectangleShape rect{{selectionRect.width, selectionRect.height}};
            rect.setPosition({selectionRect.left, selectionRect.top});
            rect.setFillColor(getRenderer()->m_selectedTextBgrColor);
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
                caret.setFillColor(getRenderer()->m_caretColor);
                target.draw(caret, states);
            }
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Draw the scrollbar if there is one
        if (m_scroll != nullptr)
            target.draw(*m_scroll, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backgroundimage")
            extractTextureFromString(property, value, rootPath, m_backgroundTexture);
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "selectedtextcolor")
            setSelectedTextColor(extractColorFromString(property, value));
        else if (property == "selectedtextbackgroundcolor")
            setSelectedTextBackgroundColor(extractColorFromString(property, value));
        else if (property == "caretcolor")
            setCaretColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "padding")
            setPadding(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_backgroundTexture.load(filename, partRect, middlePart, repeated);
        else
            m_backgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setTextColor(const sf::Color& textColor)
    {
        m_textBox->m_textBeforeSelection.setColor(textColor);
        m_textBox->m_textAfterSelection1.setColor(textColor);
        m_textBox->m_textAfterSelection2.setColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_textBox->m_textSelection1.setColor(selectedTextColor);
        m_textBox->m_textSelection2.setColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setSelectedTextBackgroundColor(const sf::Color& selectedTextBackgroundColor)
    {
        m_selectedTextBgrColor = selectedTextBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setBorderColor(const sf::Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setCaretColor(const sf::Color& caretColor)
    {
        m_caretColor = caretColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setCaretWidth(float width = 2)
    {
        m_caretWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setTextFont(const sf::Font& font)
    {
        m_textBox->m_textBeforeSelection.setFont(font);
        m_textBox->m_textSelection1.setFont(font);
        m_textBox->m_textSelection2.setFont(font);
        m_textBox->m_textAfterSelection1.setFont(font);
        m_textBox->m_textAfterSelection2.setFont(font);

        m_textBox->setTextSize(m_textBox->getTextSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);
        m_textBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::setScrollbar(const std::string& scrollbarThemeFileFilename, const std::string& section)
    {
        m_textBox->m_scroll = Scrollbar::create(scrollbarThemeFileFilename, section);
        m_textBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TextBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        if (m_backgroundTexture.getData() == nullptr)
        {
            sf::RectangleShape background(m_textBox->getSize());
            background.setPosition(m_textBox->getPosition());
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }
        else
            target.draw(m_backgroundTexture, states);

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f size = m_textBox->getSize();
            sf::Vector2f position = m_textBox->getPosition();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(m_borderColor);
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
        if (texture.getData() != nullptr)
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

            case Texture::ScalingType::NineSliceScaling:
                break;
            }
        }

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> TextBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<TextBoxRenderer>(new TextBoxRenderer{*this});
        renderer->m_textBox = static_cast<TextBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
