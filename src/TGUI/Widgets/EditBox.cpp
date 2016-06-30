/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
        m_type = "EditBox";

        addSignal<sf::String>("TextChanged");
        addSignal<sf::String>("ReturnKeyPressed");

        m_draggableWidget = true;
        m_allowFocus = true;

        m_renderer = aurora::makeCopied<tgui::EditBoxRenderer>();
        setRenderer(m_renderer->getData());

        m_defaultText.setStyle(getRenderer()->getDefaultTextStyle());
        m_caret.setSize({getRenderer()->getCaretWidth(), 0});

        setSize({240, 30});
        getRenderer()->setBorders({2, 2, 2, 2});
        m_textBeforeSelection.setFillColor(getRenderer()->getTextColor());
        m_textAfterSelection.setFillColor(getRenderer()->getTextColor());
        m_textSelection.setFillColor(getRenderer()->getSelectedTextColor());
        m_defaultText.setFillColor(getRenderer()->getDefaultTextColor());
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

    void EditBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Recalculate the text size when auto scaling
        if (m_textSize == 0)
            setText(m_text);

        getRenderer()->getTextureNormal().setSize(getUsableSize());
        getRenderer()->getTextureHover().setSize(getUsableSize());
        getRenderer()->getTextureFocused().setSize(getUsableSize());

        // Set the size of the caret
        m_caret.setSize({m_caret.getSize().x, getUsableSize().y - getRenderer()->getPadding().bottom - getRenderer()->getPadding().top});

        // Recalculate the position of the texts
        recalculateTextPositions();
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
            m_textFull.setCharacterSize(findBestTextSize(getFont(), (getUsableSize().y - getRenderer()->getPadding().bottom - getRenderer()->getPadding().top) * 0.85f));
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

    sf::String EditBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setDefaultText(const sf::String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::selectText(std::size_t start, std::size_t length)
    {
        m_selStart = start;
        m_selEnd = std::min(m_text.getSize(), start + length);
        m_selChars = m_selEnd - m_selStart;

        m_textBeforeSelection.setString(m_displayedText.substring(0, m_selStart));
        m_textSelection.setString(m_displayedText.substring(m_selStart, m_selEnd));
        m_textAfterSelection.setString(m_displayedText.substring(m_selEnd));

        recalculateTextPositions();
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

    unsigned int EditBox::getTextSize() const
    {
        return m_textFull.getCharacterSize();
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

    char EditBox::getPasswordCharacter() const
    {
        return m_passwordChar;
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

    unsigned int EditBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setAlignment(Alignment alignment)
    {
        m_textAlignment = alignment;
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Alignment EditBox::getAlignment() const
    {
        return m_textAlignment;
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

    bool EditBox::isTextWidthLimited() const
    {
        return m_limitTextWidth;
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

    void EditBox::setInputValidator(const std::string& regex)
    {
        m_regexString = regex;
        m_regex = m_regexString;

        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& EditBox::getInputValidator() const
    {
        return m_regexString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(float x, float y)
    {
        // Find the caret position
        float positionX = x - getPosition().x - getRenderer()->getBorders().left - getRenderer()->getPadding().left;

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
            Padding padding = getRenderer()->getPadding();
            Borders borders = getRenderer()->getBorders();

            // Check if there is a text width limit
            if (m_limitTextWidth)
            {
                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(x - getPosition().x - borders.left - padding.left);
            }
            else // Scrolling is enabled
            {
                float width = getVisibleEditBoxWidth();

                // Check if the mouse is on the left of the text
                if (x - getPosition().x < borders.left + padding.left)
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
                else if ((x - getPosition().x > borders.left + padding.left + width) && (m_textFull.findCharacterPos(m_displayedText.getSize()).x > width))
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
                m_selEnd = findCaretPosition(x - getPosition().x - borders.left - padding.left);
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

    void EditBox::rendererChanged(const std::string& property, ObjectConverter&& value)
    {
        if (property == "borders")
        {
            updateSize();
        }
        else if (property == "padding")
        {
            setText(m_text);

            m_caret.setSize({m_caret.getSize().x, getUsableSize().y - getRenderer()->getPadding().bottom - getRenderer()->getPadding().top});
        }
        else if (property == "caretwidth")
        {
            m_caret.setPosition({m_caret.getPosition().x + ((m_caret.getSize().x - value.getNumber()) / 2.0f), m_caret.getPosition().y});
            m_caret.setSize({value.getNumber(), getSize().y - getRenderer()->getPadding().bottom - getRenderer()->getPadding().top});
        }
        else if (property == "textcolor")
        {
            m_textBeforeSelection.setFillColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
            m_textAfterSelection.setFillColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection.setFillColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if (property == "defaulttextcolor")
        {
            m_defaultText.setFillColor(calcColorOpacity(value.getColor(), getRenderer()->getOpacity()));
        }
        else if ((property == "texturenormal") || (property == "texturehover") || (property == "texturefocused"))
        {
            value.getTexture().setPosition({getRenderer()->getBorders().left, getRenderer()->getBorders().top});
            value.getTexture().setSize(getSize());

            if (property == "texturefocused")
                m_allowFocus = value.getTexture().isLoaded();
        }
        else if (property == "textstyle")
        {
            sf::Uint32 style;
            if (value.getType() == ObjectConverter::Type::Number)
                style = value.getNumber();
            else
                style = decodeTextStyle(value.getString());

            m_textBeforeSelection.setStyle(style);
            m_textAfterSelection.setStyle(style);
            m_textSelection.setStyle(style);
            m_textFull.setStyle(style);
        }
        else if (property == "defaulttextstyle")
        {
            sf::Uint32 style;
            if (value.getType() == ObjectConverter::Type::Number)
                style = value.getNumber();
            else
                style = decodeTextStyle(value.getString());

            m_defaultText.setStyle(style);
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            m_textBeforeSelection.setFillColor(calcColorOpacity(getRenderer()->getTextColor(), opacity));
            m_textAfterSelection.setFillColor(calcColorOpacity(getRenderer()->getTextColor(), opacity));
            m_textSelection.setFillColor(calcColorOpacity(getRenderer()->getSelectedTextColor(), opacity));
            m_defaultText.setFillColor(calcColorOpacity(getRenderer()->getDefaultTextColor(), opacity));

            getRenderer()->getTextureNormal().setOpacity(opacity);
            getRenderer()->getTextureHover().setOpacity(opacity);
            getRenderer()->getTextureFocused().setOpacity(opacity);
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolornormal")
              && (property != "backgroundcolorhover")
              && (property != "selectedtextbackgroundcolor")
              && (property != "caretcolor"))
        {
            Widget::rendererChanged(property, std::move(value));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f EditBox::getUsableSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getVisibleEditBoxWidth() const
    {
        return std::max(0.f, getSize().x
                             - getRenderer()->getBorders().left
                             - getRenderer()->getBorders().right
                             - getRenderer()->getPadding().left
                             - getRenderer()->getPadding().right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::findCaretPosition(float posX)
    {
        // This code will crash when the editbox is empty. We need to avoid this.
        if (m_displayedText.isEmpty())
            return 0;

        // Find out what the first visible character is
        std::size_t firstVisibleChar;
        if (m_textCropPosition)
        {
            // Start searching near the caret to quickly find the character even in a very long string
            firstVisibleChar = m_selEnd;

            // Go backwards to find the character
            while (m_textFull.findCharacterPos(firstVisibleChar-1).x > m_textCropPosition)
                --firstVisibleChar;
        }
        else // If the first part is visible then the first character is also visible
            firstVisibleChar = 0;

        sf::String tempString;
        float textWidthWithoutLastChar;
        float fullTextWidth;
        float halfOfLastCharWidth;
        std::size_t lastVisibleChar;
        float width = getVisibleEditBoxWidth();

        // Find out how many pixels the text is moved
        float pixelsToMove = 0;
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the text width
            float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

            // Check if a layout would make sense
            if (textWidth < width)
            {
                // Set the number of pixels to move
                if (m_textAlignment == Alignment::Center)
                    pixelsToMove = (width - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    pixelsToMove = width - textWidth;
            }
        }

        // Find out what the last visible character is, starting from the caret
        lastVisibleChar = m_selEnd;

        // Go forward to find the character
        while (m_textFull.findCharacterPos(lastVisibleChar+1).x < m_textCropPosition + width)
        {
            if (lastVisibleChar == m_displayedText.getSize())
                break;

            ++lastVisibleChar;
        }

        // Set the first part of the text
        tempString = m_displayedText.toWideString().substr(0, firstVisibleChar);
        m_textFull.setString(tempString);

        // Calculate the first position
        fullTextWidth = m_textFull.findCharacterPos(firstVisibleChar).x;

        // for all the other characters, check where you have clicked.
        for (std::size_t i = firstVisibleChar; i < lastVisibleChar; ++i)
        {
            // Add the next character to the temporary string
            tempString += m_displayedText[i];
            m_textFull.setString(tempString);

            // Make some calculations
            textWidthWithoutLastChar = fullTextWidth;
            fullTextWidth = m_textFull.findCharacterPos(i + 1).x;
            halfOfLastCharWidth = (fullTextWidth - textWidthWithoutLastChar) / 2.0f;

            // Check if you have clicked on the first halve of that character
            if (posX < textWidthWithoutLastChar + pixelsToMove + halfOfLastCharWidth - m_textCropPosition)
            {
                m_textFull.setString(m_displayedText);
                return i;
            }
        }

        // If you pass here then you clicked behind all the characters
        m_textFull.setString(m_displayedText);
        return lastVisibleChar;
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
        Padding padding = getRenderer()->getPadding();

        float textX = padding.left - m_textCropPosition;
        float textY = 0;
        if (getFont())
        {
            textY = std::round((padding.top - getTextVerticalCorrection(getFont(), getTextSize()))
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
            m_selectedTextBackground.setPosition({std::floor(textX + 0.5f), std::floor(padding.top + 0.5f)});

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
        m_caret.setPosition({std::floor(caretLeft + 0.5f), std::floor(padding.top + 0.5f)});
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
        states.transform.translate(getPosition());

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getPosition(), getSize(), getRenderer()->getBorderColor());

            // Don't try to draw the text when there is no space left for it
            if ((getSize().x <= borders.left + borders.right) || (getSize().y <= borders.top + borders.bottom))
                return;
        }

        states.transform.translate({borders.left, borders.top});

        // Draw the background
        if (getRenderer()->getTextureNormal().isLoaded())
        {
            if (m_mouseHover && getRenderer()->getTextureHover().isLoaded())
                getRenderer()->getTextureHover().draw(target, states);
            else
                getRenderer()->getTextureNormal().draw(target, states);

            // When the edit box is focused then draw an extra image
            if (m_focused && getRenderer()->getTextureFocused().isLoaded())
                getRenderer()->getTextureFocused().draw(target, states);
        }
        else // There is no background texture
        {
            if (m_mouseHover)
                drawRectangleShape(target, states, getPosition(), getUsableSize(), getRenderer()->getBackgroundColorHover());
            else
                drawRectangleShape(target, states, getPosition(), getUsableSize(), getRenderer()->getBackgroundColorNormal());
        }

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        Padding padding = getRenderer()->getPadding();
        Clipping clipping{target,
                          {getAbsolutePosition().x + padding.left, getAbsolutePosition().y + padding.top},
                          {getAbsolutePosition().x + getSize().x - padding.right, getAbsolutePosition().y + getSize().y - padding.bottom}
                         };

        if ((m_textBeforeSelection.getString() != "") || (m_textSelection.getString() != ""))
        {
            target.draw(m_textBeforeSelection, states);

            if (m_textSelection.getString() != "")
            {
                drawRectangleShape(target, states, m_selectedTextBackground.getPosition(), m_selectedTextBackground.getSize(), getRenderer()->getSelectedTextBackgroundColor());

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
            drawRectangleShape(target, states, m_caret.getPosition(), m_caret.getSize(), getRenderer()->getCaretColor());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setTextStyle(sf::Uint32 style)
    {
        setProperty("textstyle", ObjectConverter{static_cast<float>(style)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Uint32 EditBoxRenderer::getTextStyle() const
    {
        auto it = m_data->propertyValuePairs.find("textstyle");
        if (it != m_data->propertyValuePairs.end())
        {
            if (it->second.getType() == ObjectConverter::Type::String)
                it->second = {static_cast<float>(decodeTextStyle(it->second.getString()))};

            return it->second.getNumber();
        }
        else
        {
            m_data->propertyValuePairs["textstyle"] = {static_cast<float>(sf::Text::Regular)};
            return sf::Text::Regular;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setDefaultTextStyle(sf::Uint32 style)
    {
        setProperty("defaulttextstyle", ObjectConverter{static_cast<float>(style)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Uint32 EditBoxRenderer::getDefaultTextStyle() const
    {
        auto it = m_data->propertyValuePairs.find("defaulttextstyle");
        if (it != m_data->propertyValuePairs.end())
        {
            if (it->second.getType() == ObjectConverter::Type::String)
                it->second = {static_cast<float>(decodeTextStyle(it->second.getString()))};

            return it->second.getNumber();
        }
        else
        {
            m_data->propertyValuePairs["defaulttextstyle"] = {static_cast<float>(sf::Text::Italic)};
            return sf::Text::Italic;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBoxRenderer::setBackgroundColor(const Color& color)
    {
        setBackgroundColorNormal(color);
        setBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Borders, Borders(0))
    TGUI_RENDERER_PROPERTY_OUTLINE(EditBoxRenderer, Padding, Padding(0))

    TGUI_RENDERER_PROPERTY_NUMBER(EditBoxRenderer, CaretWidth, 1.f)

    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, TextColor, Color(60, 60, 60))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextColor, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, SelectedTextBackgroundColor, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, DefaultTextColor, Color(245, 245, 245))
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorNormal, sf::Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BackgroundColorHover, sf::Color::White)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, CaretColor, sf::Color::Black)
    TGUI_RENDERER_PROPERTY_COLOR(EditBoxRenderer, BorderColor, sf::Color::Black)

    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureNormal)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureHover)
    TGUI_RENDERER_PROPERTY_TEXTURE(EditBoxRenderer, TextureFocused)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
