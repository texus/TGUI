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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/Clipping.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string EditBox::Validator::Int   = "[+-]?[0-9]*";
    std::string EditBox::Validator::UInt  = "[0-9]*";
    std::string EditBox::Validator::Float = "[+-]?[0-9]*\\.?[0-9]*";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox()
    {
        m_callback.widgetType = "EditBox";
        m_draggableWidget = true;
        m_allowFocus = true;

        addSignal<sf::String>("TextChanged");
        addSignal<sf::String>("ReturnKeyPressed");

        m_defaultText.setStyle(sf::Text::Italic);

        m_caret.setSize({1, 0});

        m_renderer = std::make_shared<EditBoxRenderer>(this);
        reload();

        setSize({240, 30});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::create()
    {
        return std::make_shared<EditBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::copy(EditBox::ConstPtr editBox)
    {
        if (editBox)
            return std::static_pointer_cast<EditBox>(editBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        getRenderer()->m_textureHover.setPosition(getPosition());
        getRenderer()->m_textureNormal.setPosition(getPosition());
        getRenderer()->m_textureFocused.setPosition(getPosition());

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Recalculate the text size when auto scaling
        if (m_textSize == 0)
            setText(m_text);

        getRenderer()->m_textureHover.setSize(getSize());
        getRenderer()->m_textureNormal.setSize(getSize());
        getRenderer()->m_textureFocused.setSize(getSize());

        // Set the size of the caret
        m_caret.setSize({m_caret.getSize().x,
                         getSize().y - getRenderer()->getScaledPadding().bottom - getRenderer()->getScaledPadding().top});

        // Recalculate the position of the images and texts
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f EditBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setFont(const Font& font)
    {
        Widget::setFont(font);

        if (font.getFont())
        {
            m_textBeforeSelection.setFont(*font.getFont());
            m_textSelection.setFont(*font.getFont());
            m_textAfterSelection.setFont(*font.getFont());
            m_textFull.setFont(*font.getFont());
            m_defaultText.setFont(*font.getFont());
        }

        // Recalculate the text size and position
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const sf::String& text)
    {
        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            m_textFull.setCharacterSize(findBestTextSize(getFont(), (getSize().y - getRenderer()->getScaledPadding().bottom - getRenderer()->getScaledPadding().top) * 0.85f));
            m_textBeforeSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textAfterSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_defaultText.setCharacterSize(m_textFull.getCharacterSize());
        }
        else // When the text has a fixed size
        {
            m_textFull.setCharacterSize(m_textSize);
            m_textBeforeSelection.setCharacterSize(m_textSize);
            m_textSelection.setCharacterSize(m_textSize);
            m_textAfterSelection.setCharacterSize(m_textSize);
            m_defaultText.setCharacterSize(m_textSize);
        }

        // Change the text if allowed
        if (m_regexString == ".*")
            m_text = text;
        else if (std::regex_match(text.toAnsiString(), m_regex))
            m_text = text.toAnsiString(); // Unicode is not supported when using regex because it can't be checked
        else // Clear the text
            m_text = "";

        m_displayedText = m_text;

        // If there is a character limit then check if it is exeeded
        if ((m_maxChars > 0) && (m_displayedText.getSize() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            m_displayedText.erase(m_maxChars, sf::String::InvalidPos);
        }

        // Check if there is a password character
        if (m_passwordChar != '\0')
        {
            // Loop every character and change it
            for (std::size_t i = 0; i < m_text.getSize(); ++i)
                m_displayedText[i] = m_passwordChar;
        }

        // Set the texts
        m_textBeforeSelection.setString(m_displayedText);
        m_textSelection.setString("");
        m_textAfterSelection.setString("");
        m_textFull.setString(m_displayedText);

        float width = getVisibleEditBoxWidth();

        // Check if there is a text width limit
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            while (m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x - m_textBeforeSelection.getPosition().x > width)
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                m_text.erase(m_text.getSize()-1);
                m_displayedText.erase(m_displayedText.getSize()-1);

                // Set the new text
                m_textBeforeSelection.setString(m_displayedText);
            }

            // Set the full text again
            m_textFull.setString(m_displayedText);
        }
        else // There is no text cropping
        {
            // Calculate the text width
            float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

            // If the text can be moved to the right then do so
            if (textWidth > width)
            {
                if (textWidth - m_textCropPosition < width)
                    m_textCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_textCropPosition = 0;
        }

        // Set the caret behind the last character
        setCaretPosition(m_displayedText.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setDefaultText(const sf::String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getSelectedText() const
    {
        if (m_selStart < m_selEnd)
            return m_text.substring(m_selStart, m_selChars);
        else if (m_selStart > m_selEnd)
            return m_text.substring(m_selEnd, m_selChars);
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to re-position the text
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordCharacter(char passwordChar)
    {
        // Change the password character
        m_passwordChar = passwordChar;

        // Recalculate the text position
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_displayedText.getSize() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            m_displayedText.erase(m_maxChars, sf::String::InvalidPos);

            // If we passed here then the text has changed.
            m_textBeforeSelection.setString(m_displayedText);
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
            m_textFull.setString(m_displayedText);

            // Set the caret behind the last character
            setCaretPosition(m_displayedText.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setAlignment(Alignment alignment)
    {
        m_textAlignment = alignment;
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::limitTextWidth(bool limitWidth)
    {
        m_limitTextWidth = limitWidth;

        // Check if the width is being limited
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            float width = getVisibleEditBoxWidth();
            while (m_textBeforeSelection.findCharacterPos(m_displayedText.getSize()).x - m_textBeforeSelection.getPosition().x > width)
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                m_text.erase(m_text.getSize()-1);
                m_displayedText.erase(m_displayedText.getSize()-1);
                m_textBeforeSelection.setString(m_displayedText);
            }

            // The full text might have changed
            m_textFull.setString(m_displayedText);

            // There is no clipping
            m_textCropPosition = 0;

            // If the caret was behind the limit, then set it at the end
            if (m_selEnd > m_displayedText.getSize())
                setCaretPosition(m_selEnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.getSize())
            charactersBeforeCaret = m_text.getSize();

        // Set the caret to the correct position
        m_selChars = 0;
        m_selStart = charactersBeforeCaret;
        m_selEnd = charactersBeforeCaret;

        // Change our texts
        m_textBeforeSelection.setString(m_displayedText);
        m_textSelection.setString("");
        m_textAfterSelection.setString("");
        m_textFull.setString(m_displayedText);

        // Check if scrolling is enabled
        if (!m_limitTextWidth)
        {
            // Find out the position of the caret
            float caretPosition = m_textFull.findCharacterPos(m_selEnd).x;

            if (m_selEnd == m_displayedText.getSize())
                caretPosition += m_textFull.getCharacterSize() / 10.f;

            // If the caret is too far on the right then adjust the cropping
            if (m_textCropPosition + getVisibleEditBoxWidth() < caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition - getVisibleEditBoxWidth());

            // If the caret is too far on the left then adjust the cropping
            if (m_textCropPosition > caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition);
        }

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::getCaretPosition() const
    {
        return m_selEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setCaretWidth(float width)
    {
        m_caret.setPosition(m_caret.getPosition().x + ((m_caret.getSize().x - width) / 2.0f), m_caret.getPosition().y);
        m_caret.setSize({width, getSize().y - getRenderer()->getScaledPadding().bottom - getRenderer()->getScaledPadding().top});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setInputValidator(const std::string& regex)
    {
        m_regexString = regex;
        m_regex = m_regexString;

        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& EditBox::getInputValidator()
    {
        return m_regexString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::selectText()
    {
        m_selStart = 0;
        m_selEnd = m_text.getSize();
        m_selChars = m_text.getSize();

        m_textBeforeSelection.setString("");
        m_textSelection.setString(m_displayedText);
        m_textAfterSelection.setString("");

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setOpacity(float opacity)
    {
        ClickableWidget::setOpacity(opacity);

        getRenderer()->m_textureNormal.setColor({getRenderer()->m_textureNormal.getColor().r, getRenderer()->m_textureNormal.getColor().g, getRenderer()->m_textureNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureHover.setColor({getRenderer()->m_textureHover.getColor().r, getRenderer()->m_textureHover.getColor().g, getRenderer()->m_textureHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureFocused.setColor({getRenderer()->m_textureFocused.getColor().r, getRenderer()->m_textureFocused.getColor().g, getRenderer()->m_textureFocused.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_textBeforeSelection.setFillColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textAfterSelection.setFillColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textSelection.setFillColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
        m_selectedTextBackground.setFillColor(calcColorOpacity(getRenderer()->m_selectedTextBackgroundColor, getOpacity()));
        m_defaultText.setFillColor(calcColorOpacity(getRenderer()->m_defaultTextColor, getOpacity()));
#else
        m_textBeforeSelection.setColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textAfterSelection.setColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        m_textSelection.setColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
        m_selectedTextBackground.setFillColor(calcColorOpacity(getRenderer()->m_selectedTextBackgroundColor, getOpacity()));
        m_defaultText.setColor(calcColorOpacity(getRenderer()->m_defaultTextColor, getOpacity()));
#endif
        m_caret.setFillColor(calcColorOpacity(getRenderer()->m_caretColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f EditBox::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(float x, float y)
    {
        // Find the caret position
        float positionX = x - getPosition().x - getRenderer()->getScaledPadding().left;

        std::size_t caretPosition = findCaretPosition(positionX);

        // When clicking on the left of the first character, move the caret to the left
        if ((positionX < 0) && (caretPosition > 0))
            --caretPosition;

        // When clicking on the right of the right character, move the caret to the right
        else if ((positionX > getVisibleEditBoxWidth()) && (caretPosition < m_displayedText.getSize()))
            ++caretPosition;

        // Check if this is a double click
        if ((m_possibleDoubleClick) && (m_selChars == 0) && (caretPosition == m_selEnd))
        {
            // The next click is going to be a normal one again
            m_possibleDoubleClick = false;

            // Set the caret at the end of the text
            setCaretPosition(m_displayedText.getSize());

            // Select the whole text
            m_selStart = 0;
            m_selEnd = m_text.getSize();
            m_selChars = m_text.getSize();

            // Change the texts
            m_textBeforeSelection.setString("");
            m_textSelection.setString(m_displayedText);
            m_textAfterSelection.setString("");
        }
        else // No double clicking
        {
            // Set the new caret
            setCaretPosition(caretPosition);

            // If the next click comes soon enough then it will be a double click
            m_possibleDoubleClick = true;
        }

        // Set the mouse down flag
        m_mouseDown = true;
        m_callback.mouse.x = static_cast<int>(x - getPosition().x);
        m_callback.mouse.y = static_cast<int>(y - getPosition().y);
        sendSignal("MousePressed", sf::Vector2f{x - getPosition().x, y - getPosition().y});

        recalculateTextPositions();

        // The caret should be visible
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(float x, float)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_mouseDown)
        {
            // Check if there is a text width limit
            if (m_limitTextWidth)
            {
                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(x - getPosition().x - getRenderer()->getScaledPadding().left);
            }
            else // Scrolling is enabled
            {
                float width = getVisibleEditBoxWidth();

                // Check if the mouse is on the left of the text
                if (x - getPosition().x < getRenderer()->getScaledPadding().left)
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        if (m_textCropPosition > m_textFull.getCharacterSize() / 10)
                            m_textCropPosition -= static_cast<unsigned int>(std::floor(m_textFull.getCharacterSize() / 10.f + 0.5f));
                        else
                            m_textCropPosition = 0;
                    }
                    else
                    {
                        if (m_textCropPosition)
                            --m_textCropPosition;
                    }
                }
                // Check if the mouse is on the right of the text AND there is a possibility to scroll
                else if ((x - getPosition().x > getRenderer()->getScaledPadding().left + width) && (m_textFull.findCharacterPos(m_displayedText.getSize()).x > width))
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        if (m_textCropPosition + width < m_textFull.findCharacterPos(m_displayedText.getSize()).x + (m_textFull.getCharacterSize() / 10))
                            m_textCropPosition += static_cast<unsigned int>(std::floor(m_textFull.getCharacterSize() / 10.f + 0.5f));
                        else
                            m_textCropPosition = static_cast<unsigned int>(m_textFull.findCharacterPos(m_displayedText.getSize()).x + (m_textFull.getCharacterSize() / 10) - width);
                    }
                    else
                    {
                        if (m_textCropPosition + width < m_textFull.findCharacterPos(m_displayedText.getSize()).x)
                            ++m_textCropPosition;
                    }
                }

                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(x - getPosition().x - getRenderer()->getScaledPadding().left);
            }

            // Check if we are selecting text from left to right
            if (m_selEnd > m_selStart)
            {
                // There is no need to redo everything when nothing changed
                if (m_selChars != (m_selEnd - m_selStart))
                {
                    // Adjust the number of characters that are selected
                    m_selChars = m_selEnd - m_selStart;

                    // Change our three texts
                    m_textBeforeSelection.setString(m_displayedText.toWideString().substr(0, m_selStart));
                    m_textSelection.setString(m_displayedText.toWideString().substr(m_selStart, m_selChars));
                    m_textAfterSelection.setString(m_displayedText.toWideString().substr(m_selEnd));

                    recalculateTextPositions();
                }
            }
            else if (m_selEnd < m_selStart)
            {
                // There is no need to redo everything when nothing changed
                if (m_selChars != (m_selStart - m_selEnd))
                {
                    // Adjust the number of characters that are selected
                    m_selChars = m_selStart - m_selEnd;

                    // Change our three texts
                    m_textBeforeSelection.setString(m_displayedText.toWideString().substr(0, m_selEnd));
                    m_textSelection.setString(m_displayedText.toWideString().substr(m_selEnd, m_selChars));
                    m_textAfterSelection.setString(m_displayedText.toWideString().substr(m_selStart));

                    recalculateTextPositions();
                }
            }
            else if (m_selChars > 0)
            {
                // Adjust the number of characters that are selected
                m_selChars = 0;

                // Change our three texts
                m_textBeforeSelection.setString(m_displayedText);
                m_textSelection.setString("");
                m_textAfterSelection.setString("");

                recalculateTextPositions();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Check if one of the correct keys was pressed
        if (event.code == sf::Keyboard::Left)
        {
            // Check if we have selected some text
            if (m_selChars > 0)
            {
                // We will not move the caret, but just undo the selection
                if (m_selStart < m_selEnd)
                    setCaretPosition(m_selStart);
                else
                    setCaretPosition(m_selEnd);
            }
            else // When we did not select any text
            {
                // Move the caret to the left
                if (m_selEnd > 0)
                    setCaretPosition(m_selEnd - 1);
            }

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else if (event.code == sf::Keyboard::Right)
        {
            // Check if we have selected some text
            if (m_selChars > 0)
            {
                // We will not move the caret, but just undo the selection
                if (m_selStart < m_selEnd)
                    setCaretPosition(m_selEnd);
                else
                    setCaretPosition(m_selStart);
            }
            else // When we did not select any text
            {
                // Move the caret to the right
                if (m_selEnd < m_displayedText.getSize())
                    setCaretPosition(m_selEnd + 1);
            }

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else if (event.code == sf::Keyboard::Home)
        {
            // Set the caret to the beginning of the text
            setCaretPosition(0);

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else if (event.code == sf::Keyboard::End)
        {
            // Set the caret behind the text
            setCaretPosition(m_text.getSize());

            // Our caret has moved, it should be visible
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else if (event.code == sf::Keyboard::Return)
        {
            m_callback.text = m_text;
            sendSignal("ReturnKeyPressed", getText());
        }
        else if (event.code == sf::Keyboard::BackSpace)
        {
            // Make sure that we did not select any characters
            if (m_selChars == 0)
            {
                // We can't delete any characters when you are at the beginning of the string
                if (m_selEnd == 0)
                    return;

                // Erase the character
                m_text.erase(m_selEnd-1, 1);
                m_displayedText.erase(m_selEnd-1, 1);

                // Set the caret back on the correct position
                setCaretPosition(m_selEnd - 1);

                float width = getVisibleEditBoxWidth();

                // Calculate the text width
                float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

                // If the text can be moved to the right then do so
                if (textWidth > width)
                {
                    if (textWidth - m_textCropPosition < width)
                        m_textCropPosition = static_cast<unsigned int>(textWidth - width);
                }
                else
                    m_textCropPosition = 0;
            }
            else // When you did select some characters, delete them
                deleteSelectedCharacters();

            // The caret should be visible again
            m_caretVisible = true;
            m_animationTimeElapsed = {};

            m_callback.text = m_text;
            sendSignal("TextChanged", getText());
        }
        else if (event.code == sf::Keyboard::Delete)
        {
            // Make sure that no text is selected
            if (m_selChars == 0)
            {
                // When the caret is at the end of the line then you can't delete anything
                if (m_selEnd == m_text.getSize())
                    return;

                // Erase the character
                m_text.erase(m_selEnd, 1);
                m_displayedText.erase(m_selEnd, 1);

                // Set the caret back on the correct position
                setCaretPosition(m_selEnd);

                // Calculate the text width
                float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

                // If the text can be moved to the right then do so
                float width = getVisibleEditBoxWidth();
                if (textWidth > width)
                {
                    if (textWidth - m_textCropPosition < width)
                        m_textCropPosition = static_cast<unsigned int>(textWidth - width);
                }
                else
                    m_textCropPosition = 0;
            }
            else // You did select some characters, delete them
                deleteSelectedCharacters();

            // The caret should be visible again
            m_caretVisible = true;
            m_animationTimeElapsed = {};

            m_callback.text = m_text;
            sendSignal("TextChanged", getText());
        }
        else
        {
            // Check if you are copying, pasting or cutting text
            if (event.control && !event.alt && !event.shift && !event.system)
            {
                if (event.code == sf::Keyboard::C)
                {
                    Clipboard::set(m_textSelection.getString());
                }
                else if (event.code == sf::Keyboard::V)
                {
                    auto clipboardContents = Clipboard::get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selChars > 0) || (clipboardContents.getSize() > 0))
                    {
                        deleteSelectedCharacters();

                        std::size_t oldCaretPos = m_selEnd;

                        if (m_text.getSize() > m_selEnd)
                            setText(m_text.toWideString().substr(0, m_selEnd) + Clipboard::get() + m_text.toWideString().substr(m_selEnd, m_text.getSize() - m_selEnd));
                        else
                            setText(m_text + clipboardContents);

                        setCaretPosition(oldCaretPos + clipboardContents.getSize());

                        m_callback.text = m_text;
                        sendSignal("TextChanged", getText());
                    }
                }
                else if (event.code == sf::Keyboard::X)
                {
                    Clipboard::set(m_textSelection.getString());
                    deleteSelectedCharacters();

                    m_callback.text = m_text;
                    sendSignal("TextChanged", getText());
                }
                else if (event.code == sf::Keyboard::A)
                {
                    selectText();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::textEntered(sf::Uint32 key)
    {
        // Only add the character when the regex matches
        if (m_regexString != ".*")
        {
            sf::String text = m_text;
            text.insert(m_selEnd, key);

            // The character has to match the regex
            if (!std::regex_match(text.toAnsiString(), m_regex))
                return;
        }

        // If there are selected characters then delete them first
        if (m_selChars > 0)
            deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.getSize() + 1 > m_maxChars))
            return;

        // Insert our character
        m_text.insert(m_selEnd, key);

        // Change the displayed text
        if (m_passwordChar != '\0')
            m_displayedText.insert(m_selEnd, m_passwordChar);
        else
            m_displayedText.insert(m_selEnd, key);

        // Append the character to the text
        m_textFull.setString(m_displayedText);

        // When there is a text width limit then reverse what we just did
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (m_textFull.findCharacterPos(m_displayedText.getSize()).x > getVisibleEditBoxWidth())
            {
                // If the text does not fit in the EditBox then delete the added character
                m_text.erase(m_selEnd, 1);
                m_displayedText.erase(m_selEnd, 1);
                return;
            }
        }

        // Move our caret forward
        setCaretPosition(m_selEnd + 1);

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        m_callback.text = m_text;
        sendSignal("TextChanged", getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::widgetFocused()
    {
    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(true);
    #endif

        Widget::widgetFocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_selChars)
            setCaretPosition(m_selEnd);

    #if defined (SFML_SYSTEM_ANDROID) || defined (SFML_SYSTEM_IOS)
        sf::Keyboard::setVirtualKeyboardVisible(false);
    #endif

        Widget::widgetUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_textBeforeSelection.setFillColor({0, 0, 0});
        m_textSelection.setFillColor({255, 255, 255});
        m_textAfterSelection.setFillColor({0, 0, 0});
        m_defaultText.setFillColor({160, 160, 160});
#else
        m_textBeforeSelection.setColor({0, 0, 0});
        m_textSelection.setColor({255, 255, 255});
        m_textAfterSelection.setColor({0, 0, 0});
        m_defaultText.setColor({160, 160, 160});
#endif
        m_caret.setFillColor({0, 0, 0});
        m_selectedTextBackground.setFillColor({0, 110, 255});

        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setPadding({4, 2, 4, 2});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackgroundColorNormal({245, 245, 245});
        getRenderer()->setBackgroundColorHover({255, 255, 255});
        getRenderer()->setNormalTexture({});
        getRenderer()->setHoverTexture({});
        getRenderer()->setFocusTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_textureNormal.isLoaded())
                    setSize(getRenderer()->m_textureNormal.getImageSize());
            }

            // Call setText to re-position the text
            setText(m_text);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getVisibleEditBoxWidth()
    {
        return std::max(0.f, getSize().x - getRenderer()->getScaledPadding().left - getRenderer()->getScaledPadding().right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::findCaretPosition(float posX)
    {
        // This code will crash when the editbox is empty. We need to avoid this.
        if (m_displayedText.isEmpty())
            return 0;

        // Take the part outside the edit box into account when the text does not fit inside it
        posX += m_textCropPosition;

        // If the text is centered or aligned to the right then the position has to be corrected when the edit box is not entirely full
        if (m_textAlignment != Alignment::Left)
        {
            float editBoxWidth = getVisibleEditBoxWidth();
            float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

            if (textWidth < editBoxWidth)
            {
                // Set the number of pixels to move
                if (m_textAlignment == Alignment::Center)
                    posX -= (editBoxWidth - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    posX -= editBoxWidth - textWidth;
            }
        }

        float width = 0;
        sf::Uint32 prevChar = 0;
        unsigned int textSize = getTextSize();
        bool bold = false; /// TODO: (getRenderer()->getTextStyle() & sf::Text::Bold) != 0;

        std::size_t index;
        for (index = 0; index < m_text.getSize(); ++index)
        {
            float charWidth;
            sf::Uint32 curChar = m_text[index];
            if (curChar == '\n')
                width = 0; // This should not happen as edit box is for single line text, but lets try the next line anyway since we haven't found the position yet
            else if (curChar == '\t')
                charWidth = static_cast<float>(getFont()->getGlyph(' ', textSize, bold).advance) * 4;
            else
                charWidth = static_cast<float>(getFont()->getGlyph(curChar, textSize, bold).advance);

            float kerning = static_cast<float>(getFont()->getKerning(prevChar, curChar, textSize));
            if (width + charWidth < posX)
                width += charWidth + kerning;
            else
            {
                // If the mouse is on the second halve of the character then the caret should be on the right of it
                if (width + charWidth - posX < charWidth / 2.f)
                    index++;

                break;
            }

            prevChar = curChar;
        }

        return index;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::deleteSelectedCharacters()
    {
        // Nothing to delete when no text was selected
        if (m_selChars == 0)
            return;

        // Check if the characters were selected from left to right
        if (m_selStart < m_selEnd)
        {
            // Erase the characters
            m_text.erase(m_selStart, m_selChars);
            m_displayedText.erase(m_selStart, m_selChars);

            // Set the caret back on the correct position
            setCaretPosition(m_selStart);
        }
        else // When the text is selected from right to left
        {
            // Erase the characters
            m_text.erase(m_selEnd, m_selChars);
            m_displayedText.erase(m_selEnd, m_selChars);

            // Set the caret back on the correct position
            setCaretPosition(m_selEnd);
        }

        // Calculate the text width
        float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

        // If the text can be moved to the right then do so
        float width = getVisibleEditBoxWidth();
        if (textWidth > width)
        {
            if (textWidth - m_textCropPosition < width)
                m_textCropPosition = static_cast<unsigned int>(textWidth - width);
        }
        else
            m_textCropPosition = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::recalculateTextPositions()
    {
        Padding padding = getRenderer()->getScaledPadding();

        float textX = getPosition().x + padding.left - m_textCropPosition;
        float textY = 0;
        if (getFont())
        {
            textY = std::round((getPosition().y + padding.top - getTextVerticalCorrection(getFont(), getTextSize()))
                               + ((getSize().y - padding.bottom - padding.top) - getFont()->getLineSpacing(getTextSize())) / 2.f);
        }

        // Check if the layout wasn't left
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the text width
            float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

            // Check if a layout would make sense
            if (textWidth < getVisibleEditBoxWidth())
            {
                // Put the text on the correct position
                if (m_textAlignment == Alignment::Center)
                    textX += (getVisibleEditBoxWidth() - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    textX += getVisibleEditBoxWidth() - textWidth;
            }
        }

        float caretLeft = textX;

        // Set the text before the selection on the correct position
        m_textBeforeSelection.setPosition(std::floor(textX + 0.5f), textY);
        m_defaultText.setPosition(std::floor(textX + 0.5f), textY);

        // Check if there is a selection
        if (m_selChars != 0)
        {
            // Watch out for the kerning
            if (m_textBeforeSelection.getString().getSize() > 0)
                textX += m_font->getKerning(m_displayedText[m_textBeforeSelection.getString().getSize() - 1], m_displayedText[m_textBeforeSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            textX += m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x - m_textBeforeSelection.getPosition().x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_selectedTextBackground.setSize({m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x - m_textSelection.getPosition().x,
                                              getSize().y - padding.top - padding.bottom});
            m_selectedTextBackground.setPosition(std::floor(textX + 0.5f), std::floor(getPosition().y + padding.top + 0.5f));

            // Set the text selected text on the correct position
            m_textSelection.setPosition(std::floor(textX + 0.5f), textY);

            // Watch out for kerning
            if (m_displayedText.getSize() > m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize())
                textX += m_font->getKerning(m_displayedText[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize() - 1], m_displayedText[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            // Set the text selected text on the correct position
            textX += m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x  - m_textSelection.getPosition().x;
            m_textAfterSelection.setPosition(std::floor(textX + 0.5f), textY);
        }

        // Set the position of the caret
        caretLeft += m_textFull.findCharacterPos(m_selEnd).x - (m_caret.getSize().x * 0.5f);
        m_caret.setPosition(std::floor(caretLeft + 0.5f), std::floor(padding.top + getPosition().y + 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::update(sf::Time elapsedTime)
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

    void EditBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        getRenderer()->draw(target, states);

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        Padding padding = getRenderer()->getScaledPadding();
        Clipping clipping{target, states, {getPosition().x + padding.left, getPosition().y + padding.top}, {getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}};

        if ((m_textBeforeSelection.getString() != "") || (m_textSelection.getString() != ""))
        {
            target.draw(m_textBeforeSelection, states);

            if (m_textSelection.getString() != "")
            {
                target.draw(m_selectedTextBackground, states);

                target.draw(m_textSelection, states);
                target.draw(m_textAfterSelection, states);
            }
        }
        else if (m_defaultText.getString() != "")
        {
            target.draw(m_defaultText, states);
        }

        // Draw the caret
        if ((m_focused) && (m_caretVisible))
            target.draw(m_caret, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);
        if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "caretwidth")
            m_editBox->setCaretWidth(Deserializer::deserialize(ObjectConverter::Type::Number, value).getNumber());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextcolor")
            setSelectedTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextbackgroundcolor")
            setSelectedTextBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "defaulttextcolor")
            setDefaultTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "caretcolor")
            setCaretColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolornormal")
            setBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolorhover")
            setBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "normalimage")
            setNormalTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "hoverimage")
            setHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "focusedimage")
            setFocusTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "padding")
                setPadding(value.getBorders());
            else if (property == "borders")
                setBorders(value.getBorders());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "selectedtextcolor")
                setSelectedTextColor(value.getColor());
            else if (property == "selectedtextbackgroundcolor")
                setSelectedTextBackgroundColor(value.getColor());
            else if (property == "defaulttextcolor")
                setDefaultTextColor(value.getColor());
            else if (property == "caretcolor")
                setCaretColor(value.getColor());
            else if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "backgroundcolornormal")
                setBackgroundColorNormal(value.getColor());
            else if (property == "backgroundcolorhover")
                setBackgroundColorHover(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "normalimage")
                setNormalTexture(value.getTexture());
            else if (property == "hoverimage")
                setHoverTexture(value.getTexture());
            else if (property == "focusedimage")
                setFocusTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Number)
        {
            if (property == "caretwidth")
                m_editBox->setCaretWidth(value.getNumber());
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter EditBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "padding")
            return m_padding;
        else if (property == "borders")
            return m_borders;
        else if (property == "caretwidth")
            return m_editBox->getCaretWidth();
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "selectedtextcolor")
            return m_selectedTextColor;
        else if (property == "selectedtextbackgroundcolor")
            return m_selectedTextBackgroundColor;
        else if (property == "defaulttextcolor")
            return m_defaultTextColor;
        else if (property == "caretcolor")
            return m_caretColor;
        else if (property == "backgroundcolor")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolornormal")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolorhover")
            return m_backgroundColorHover;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "normalimage")
            return m_textureNormal;
        else if (property == "hoverimage")
            return m_textureHover;
        else if (property == "focusedimage")
            return m_textureFocused;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> EditBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureNormal.isLoaded())
        {
            pairs["NormalImage"] = m_textureNormal;
            if (m_textureHover.isLoaded())
                pairs["HoverImage"] = m_textureHover;
            if (m_textureFocused.isLoaded())
                pairs["FocusedImage"] = m_textureFocused;
        }
        else
        {
            pairs["BackgroundColorNormal"] = m_backgroundColorNormal;
            pairs["BackgroundColorHover"] = m_backgroundColorHover;
        }

        pairs["CaretWidth"] = m_editBox->getCaretWidth();
        pairs["TextColor"] = m_textColor;
        pairs["SelectedTextColor"] = m_selectedTextColor;
        pairs["SelectedTextBackgroundColor"] = m_selectedTextBackgroundColor;
        pairs["DefaultTextColor"] = m_defaultTextColor;
        pairs["CaretColor"] = m_caretColor;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;

        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);

        // Recalculate the text size
        m_editBox->setText(m_editBox->m_text);

        // Set the size of the caret
        m_editBox->m_caret.setSize({m_editBox->m_caret.getSize().x,
                                    m_editBox->getSize().y - getScaledPadding().bottom - getScaledPadding().top});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setCaretWidth(float width)
    {
        m_editBox->setCaretWidth(width);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setTextColor(const Color& textColor)
    {
        m_textColor = textColor;
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_editBox->m_textBeforeSelection.setFillColor(calcColorOpacity(m_textColor, m_editBox->getOpacity()));
        m_editBox->m_textAfterSelection.setFillColor(calcColorOpacity(m_textColor, m_editBox->getOpacity()));
#else
        m_editBox->m_textBeforeSelection.setColor(calcColorOpacity(m_textColor, m_editBox->getOpacity()));
        m_editBox->m_textAfterSelection.setColor(calcColorOpacity(m_textColor, m_editBox->getOpacity()));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setSelectedTextColor(const Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_editBox->m_textSelection.setFillColor(calcColorOpacity(m_selectedTextColor, m_editBox->getOpacity()));
#else
        m_editBox->m_textSelection.setColor(calcColorOpacity(m_selectedTextColor, m_editBox->getOpacity()));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setSelectedTextBackgroundColor(const Color& selectedTextBackgroundColor)
    {
        m_selectedTextBackgroundColor = selectedTextBackgroundColor;
        m_editBox->m_selectedTextBackground.setFillColor(calcColorOpacity(m_selectedTextBackgroundColor, m_editBox->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setDefaultTextColor(const Color& defaultTextColor)
    {
        m_defaultTextColor = defaultTextColor;
#if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        m_editBox->m_defaultText.setFillColor(calcColorOpacity(m_defaultTextColor, m_editBox->getOpacity()));
#else
        m_editBox->m_defaultText.setColor(calcColorOpacity(m_defaultTextColor, m_editBox->getOpacity()));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setBackgroundColor(const Color& color)
    {
        setBackgroundColorNormal(color);
        setBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setBackgroundColorNormal(const Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setBackgroundColorHover(const Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setCaretColor(const Color& caretColor)
    {
        m_caretColor = caretColor;
        m_editBox->m_caret.setFillColor(calcColorOpacity(m_caretColor, m_editBox->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setNormalTexture(const Texture& texture)
    {
        m_textureNormal = texture;
        if (m_textureNormal.isLoaded())
        {
            m_textureNormal.setPosition(m_editBox->getPosition());
            m_textureNormal.setSize(m_editBox->getSize());
            m_textureNormal.setColor({m_textureNormal.getColor().r, m_textureNormal.getColor().g, m_textureNormal.getColor().b, static_cast<sf::Uint8>(m_editBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setHoverTexture(const Texture& texture)
    {
        m_textureHover = texture;
        if (m_textureHover.isLoaded())
        {
            m_textureHover.setPosition(m_editBox->getPosition());
            m_textureHover.setSize(m_editBox->getSize());
            m_textureHover.setColor({m_textureHover.getColor().r, m_textureHover.getColor().g, m_textureHover.getColor().b, static_cast<sf::Uint8>(m_editBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setFocusTexture(const Texture& texture)
    {
        m_textureFocused = texture;
        if (m_textureFocused.isLoaded())
        {
            m_textureFocused.setPosition(m_editBox->getPosition());
            m_textureFocused.setSize(m_editBox->getSize());
            m_textureFocused.setColor({m_textureFocused.getColor().r, m_textureFocused.getColor().g, m_textureFocused.getColor().b, static_cast<sf::Uint8>(m_editBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setDefaultTextStyle(sf::Uint32 style)
    {
        m_editBox->m_defaultText.setStyle(style);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Check if there is a background texture
        if (m_textureNormal.isLoaded())
        {
            if (m_editBox->m_mouseHover && m_textureHover.isLoaded())
                target.draw(m_textureHover, states);
            else
                target.draw(m_textureNormal, states);

            // When the edit box is focused then draw an extra image
            if (m_editBox->m_focused && m_textureFocused.isLoaded())
                target.draw(m_textureFocused, states);
        }
        else // There is no background texture
        {
            sf::RectangleShape editBox(m_editBox->getSize());
            editBox.setPosition(m_editBox->getPosition());

            if (m_editBox->m_mouseHover)
                editBox.setFillColor(calcColorOpacity(m_backgroundColorHover, m_editBox->getOpacity()));
            else
                editBox.setFillColor(calcColorOpacity(m_backgroundColorNormal, m_editBox->getOpacity()));

            target.draw(editBox, states);
        }

        // Draw the borders around the edit box
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_editBox->getPosition();
            sf::Vector2f size = m_editBox->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_editBox->getOpacity()));
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

    Padding EditBoxRenderer::getScaledPadding() const
    {
        Padding padding = getPadding();
        Padding scaledPadding = padding;

        auto& texture = m_textureNormal;
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

    std::shared_ptr<WidgetRenderer> EditBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<EditBoxRenderer>(*this);
        renderer->m_editBox = static_cast<EditBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
