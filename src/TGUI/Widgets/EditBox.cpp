/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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
#include <TGUI/Clipboard.hpp>
#include <TGUI/Clipping.hpp>

/// TODO: Where m_selStart and m_selEnd are compared, use std::min and std::max and merge the if and else bodies

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string EditBox::Validator::All   = ".*";
    std::string EditBox::Validator::Int   = "[+-]?[0-9]*";
    std::string EditBox::Validator::UInt  = "[0-9]*";
    std::string EditBox::Validator::Float = "[+-]?[0-9]*\\.?[0-9]*";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{2}},
                {"padding", Padding{4, 2, 4, 2}},
                {"caretwidth", 1.f},
                {"caretcolor", sf::Color::Black},
                {"bordercolor", Color{60, 60, 60}},
                {"bordercolorhover", sf::Color::Black},
                {"textcolor", Color{60, 60, 60}},
                {"selectedtextcolor", sf::Color::White},
                {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                {"defaulttextcolor", Color{160, 160, 160}},
                {"backgroundcolor", Color{245, 245, 245}},
                {"backgroundcolorhover", sf::Color::White}
                ///TODO: Define default disabled colors
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox()
    {
        m_callback.widgetType = "EditBox";
        m_type = "EditBox";

        addSignal<sf::String>("TextChanged");
        addSignal<sf::String>("ReturnKeyPressed");

        m_draggableWidget = true;
        m_allowFocus = true;

        m_renderer = aurora::makeCopied<EditBoxRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

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

    void EditBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Recalculate the text size when auto scaling
        if (m_textSize == 0)
            setText(m_text);

        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Set the size of the caret
        m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.bottom - m_paddingCached.top});

        // Recalculate the position of the texts
        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::enable()
    {
        Widget::enable();

        m_textBeforeSelection.setColor(getRenderer()->getTextColor());
        m_textAfterSelection.setColor(getRenderer()->getTextColor());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::disable()
    {
        Widget::disable();

        if (getRenderer()->getTextColorDisabled().isSet())
        {
            m_textBeforeSelection.setColor(getRenderer()->getTextColorDisabled());
            m_textAfterSelection.setColor(getRenderer()->getTextColorDisabled());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const sf::String& text)
    {
        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            m_textFull.setCharacterSize(Text::findBestTextSize(m_fontCached, (getInnerSize().y - m_paddingCached.bottom - m_paddingCached.top) * 0.8f));
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

        // Remove all the excess characters if there is a character limit
        if ((m_maxChars > 0) && (m_text.getSize() > m_maxChars))
            m_text.erase(m_maxChars, sf::String::InvalidPos);

        // Set the displayed text
        if (m_passwordChar != '\0')
        {
            sf::String displayedText = m_text;
            std::fill(displayedText.begin(), displayedText.end(), m_passwordChar);

            m_textFull.setString(displayedText);
        }
        else
            m_textFull.setString(m_text);

        // Set the texts
        m_textBeforeSelection.setString(m_textFull.getString());
        m_textSelection.setString("");
        m_textAfterSelection.setString("");

        if (!m_fontCached)
            return;

        // Check if there is a text width limit
        const float width = getVisibleEditBoxWidth();
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            while (m_textFull.getSize().x > width)
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(displayedString.getSize()-1);
                m_textFull.setString(displayedString);
                m_text.erase(m_text.getSize()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());
        }
        else // There is no text cropping
        {
            const float textWidth = m_textFull.getSize().x;

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
        setCaretPosition(m_textFull.getString().getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setDefaultText(const sf::String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& EditBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::selectText(std::size_t start, std::size_t length)
    {
        m_selStart = start;
        m_selEnd = std::min(m_text.getSize(), start + length);
        m_selChars = m_selEnd - m_selStart;

        m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selStart));
        m_textSelection.setString(m_textFull.getString().substring(m_selStart, m_selEnd - m_selStart));
        m_textAfterSelection.setString(m_textFull.getString().substring(m_selEnd));

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getSelectedText() const
    {
        return m_text.substring(std::min(m_selStart, m_selEnd), std::max(m_selStart, m_selEnd));
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
        if ((m_maxChars > 0) && (m_textFull.getString().getSize() > m_maxChars))
        {
            sf::String displayedText = m_textFull.getString();

            // Remove all the excess characters
            m_text.erase(m_maxChars, sf::String::InvalidPos);
            displayedText.erase(m_maxChars, sf::String::InvalidPos);

            // If we passed here then the text has changed.
            m_textBeforeSelection.setString(displayedText);
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
            m_textFull.setString(displayedText);

            // Set the caret behind the last character
            setCaretPosition(displayedText.getSize());
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

        if (!m_fontCached)
            return;

        // Check if the width is being limited
        if (m_limitTextWidth)
        {
            // Delete the last characters when the text no longer fits inside the edit box
            const float width = getVisibleEditBoxWidth();
            while (m_textFull.getSize().x > width)
            {
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(displayedString.getSize()-1);
                m_textFull.setString(displayedString);
                m_text.erase(m_text.getSize()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());

            // There is no clipping
            m_textCropPosition = 0;

            // If the caret was behind the limit, then set it at the end
            if (m_selEnd > m_textFull.getString().getSize())
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
        m_textBeforeSelection.setString(m_textFull.getString());
        m_textSelection.setString("");
        m_textAfterSelection.setString("");

        if (!m_fontCached)
            return;

        // Check if scrolling is enabled
        if (!m_limitTextWidth)
        {
            // Find out the position of the caret
            const float caretPosition = m_textFull.findCharacterPos(m_selEnd).x;

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

    void EditBox::leftMousePressed(sf::Vector2f pos)
    {
        // Find the caret position
        float positionX = pos.x - m_bordersCached.left - m_paddingCached.left;

        std::size_t caretPosition = findCaretPosition(positionX);

        // When clicking on the left of the first character, move the caret to the left
        if ((positionX < 0) && (caretPosition > 0))
            --caretPosition;

        // When clicking on the right of the right character, move the caret to the right
        else if ((positionX > getVisibleEditBoxWidth()) && (caretPosition < m_textFull.getString().getSize()))
            ++caretPosition;

        // Check if this is a double click
        if ((m_possibleDoubleClick) && (m_selChars == 0) && (caretPosition == m_selEnd))
        {
            // The next click is going to be a normal one again
            m_possibleDoubleClick = false;

            // Set the caret at the end of the text
            setCaretPosition(m_textFull.getString().getSize());

            // Select the whole text
            m_selStart = 0;
            m_selEnd = m_text.getSize();
            m_selChars = m_text.getSize();

            // Change the texts
            m_textBeforeSelection.setString("");
            m_textSelection.setString(m_textFull.getString());
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
        m_callback.mouse.x = static_cast<int>(pos.x);
        m_callback.mouse.y = static_cast<int>(pos.y);
        sendSignal("MousePressed", pos);

        recalculateTextPositions();

        // The caret should be visible
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(sf::Vector2f pos)
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
                m_selEnd = findCaretPosition(pos.x - m_bordersCached.left - m_paddingCached.left);
            }
            else // Scrolling is enabled
            {
                const float width = getVisibleEditBoxWidth();

                // Check if the mouse is on the left of the text
                if (pos.x < m_bordersCached.left + m_paddingCached.left)
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        if (m_textCropPosition > m_textFull.getCharacterSize() / 10)
                            m_textCropPosition -= static_cast<unsigned int>(m_textFull.getCharacterSize() / 10.f);
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
                else if ((pos.x > m_bordersCached.left + m_paddingCached.left + width) && (m_textFull.getSize().x > width))
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        if (m_textCropPosition + width < m_textFull.getSize().x + (m_textFull.getCharacterSize() / 10))
                            m_textCropPosition += static_cast<unsigned int>(m_textFull.getCharacterSize() / 10.f);
                        else
                            m_textCropPosition = static_cast<unsigned int>(m_textFull.getSize().x + (m_textFull.getCharacterSize() / 10) - width);
                    }
                    else
                    {
                        if (m_textCropPosition + width < m_textFull.getSize().x)
                            ++m_textCropPosition;
                    }
                }

                // Find out between which characters the mouse is standing
                m_selEnd = findCaretPosition(pos.x - m_bordersCached.left - m_paddingCached.left);
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
                    m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selStart));
                    m_textSelection.setString(m_textFull.getString().substring(m_selStart, m_selChars));
                    m_textAfterSelection.setString(m_textFull.getString().substring(m_selEnd));

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
                    m_textBeforeSelection.setString(m_textFull.getString().substring(0, m_selEnd));
                    m_textSelection.setString(m_textFull.getString().substring(m_selEnd, m_selChars));
                    m_textAfterSelection.setString(m_textFull.getString().substring(m_selStart));

                    recalculateTextPositions();
                }
            }
            else if (m_selChars > 0)
            {
                // Adjust the number of characters that are selected
                m_selChars = 0;

                // Change our three texts
                m_textBeforeSelection.setString(m_textFull.getString());
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
                setCaretPosition(std::min(m_selStart, m_selEnd));
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
                setCaretPosition(std::max(m_selStart, m_selEnd));
            }
            else // When we did not select any text
            {
                // Move the caret to the right
                if (m_selEnd < m_textFull.getString().getSize())
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
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(m_selEnd-1, 1);
                m_textFull.setString(displayedString);
                m_text.erase(m_selEnd-1, 1);

                // Set the caret back on the correct position
                setCaretPosition(m_selEnd - 1);

                const float width = getVisibleEditBoxWidth();

                // Calculate the text width
                const float textWidth = m_textFull.getSize().x;

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
                sf::String displayedString = m_textFull.getString();
                displayedString.erase(m_selEnd, 1);
                m_textFull.setString(displayedString);
                m_text.erase(m_selEnd, 1);

                // Set the caret back on the correct position
                setCaretPosition(m_selEnd);

                // Calculate the text width
                const float textWidth = m_textFull.getSize().x;

                // If the text can be moved to the right then do so
                const float width = getVisibleEditBoxWidth();
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
        sf::String displayedText = m_textFull.getString();
        if (m_passwordChar != '\0')
            displayedText.insert(m_selEnd, m_passwordChar);
        else
            displayedText.insert(m_selEnd, key);

        m_textFull.setString(displayedText);

        // When there is a text width limit then reverse what we just did
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (m_textFull.getSize().x > getVisibleEditBoxWidth())
            {
                // If the text does not fit in the EditBox then delete the added character
                m_text.erase(m_selEnd, 1);
                displayedText.erase(m_selEnd, 1);
                m_textFull.setString(displayedText);
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

    void EditBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            updateSize();
        }
        else if (property == "padding")
        {
            m_paddingCached = getRenderer()->getPadding();

            setText(m_text);

            m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.bottom - m_paddingCached.top});
        }
        else if (property == "caretwidth")
        {
            m_caret.setPosition({m_caret.getPosition().x + ((m_caret.getSize().x - getRenderer()->getCaretWidth()) / 2.0f), m_caret.getPosition().y});
            m_caret.setSize({getRenderer()->getCaretWidth(), getInnerSize().y - m_paddingCached.bottom - m_paddingCached.top});
        }
        else if ((property == "textcolor") || (property == "textcolordisabled"))
        {
            if (m_enabled || !getRenderer()->getTextColorDisabled().isSet())
            {
                m_textBeforeSelection.setColor(getRenderer()->getTextColor());
                m_textAfterSelection.setColor(getRenderer()->getTextColor());
            }
            else
            {
                m_textBeforeSelection.setColor(getRenderer()->getTextColorDisabled());
                m_textAfterSelection.setColor(getRenderer()->getTextColorDisabled());
            }
        }
        else if (property == "selectedtextcolor")
        {
            m_textSelection.setColor(getRenderer()->getSelectedTextColor());
        }
        else if (property == "defaulttextcolor")
        {
            m_defaultText.setColor(getRenderer()->getDefaultTextColor());
        }
        else if (property == "texture")
        {
            m_sprite.setTexture(getRenderer()->getTexture());
        }
        else if (property == "texturehover")
        {
            m_spriteHover.setTexture(getRenderer()->getTextureHover());
        }
        else if (property == "texturedisabled")
        {
            m_spriteDisabled.setTexture(getRenderer()->getTextureDisabled());
        }
        else if (property == "texturefocused")
        {
            m_spriteFocused.setTexture(getRenderer()->getTextureFocused());
            m_allowFocus = m_spriteFocused.isSet();
        }
        else if (property == "textstyle")
        {
            TextStyle style = getRenderer()->getTextStyle();
            m_textBeforeSelection.setStyle(style);
            m_textAfterSelection.setStyle(style);
            m_textSelection.setStyle(style);
            m_textFull.setStyle(style);
        }
        else if (property == "defaulttextstyle")
        {
            m_defaultText.setStyle(getRenderer()->getDefaultTextStyle());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "bordercolorhover")
        {
            m_borderColorHoverCached = getRenderer()->getBorderColorHover();
        }
        else if (property == "bordercolordisabled")
        {
            m_borderColorDisabledCached = getRenderer()->getBorderColorDisabled();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getRenderer()->getBackgroundColorHover();
        }
        else if (property == "backgroundcolordisabled")
        {
            m_backgroundColorDisabledCached = getRenderer()->getBackgroundColorDisabled();
        }
        else if (property == "caretcolor")
        {
            m_caretColorCached = getRenderer()->getCaretColor();
        }
        else if (property == "caretcolorhover")
        {
            m_caretColorHoverCached = getRenderer()->getCaretColorHover();
        }
        else if (property == "caretcolordisabled")
        {
            m_caretColorDisabledCached = getRenderer()->getCaretColorDisabled();
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            m_selectedTextBackgroundColorCached = getRenderer()->getSelectedTextBackgroundColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection.setOpacity(m_opacityCached);
            m_textSelection.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
            m_spriteFocused.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection.setFont(m_fontCached);
            m_textAfterSelection.setFont(m_fontCached);
            m_textFull.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);

            // Recalculate the text size and position
            setText(m_text);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f EditBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.left - m_bordersCached.right, getSize().y - m_bordersCached.top - m_bordersCached.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getVisibleEditBoxWidth() const
    {
        return std::max(0.f, getSize().x - m_bordersCached.left - m_bordersCached.right - m_paddingCached.left - m_paddingCached.right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::findCaretPosition(float posX)
    {
        // Take the part outside the edit box into account when the text does not fit inside it
        posX += m_textCropPosition;

        // If the text is centered or aligned to the right then the position has to be corrected when the edit box is not entirely full
        if (m_textAlignment != Alignment::Left)
        {
            const float editBoxWidth = getVisibleEditBoxWidth();
            const float textWidth = m_textFull.getSize().x;

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
        const unsigned int textSize = getTextSize();
        const bool bold = (m_textFull.getStyle() & sf::Text::Bold) != 0;

        std::size_t index;
        for (index = 0; index < m_text.getSize(); ++index)
        {
            float charWidth;
            sf::Uint32 curChar = m_text[index];
            if (curChar == '\n')
            {
                // This should not happen as edit box is for single line text, but lets try the next line anyway since we haven't found the position yet
                width = 0;
                prevChar = 0;
                continue;
            }
            else if (curChar == '\t')
                charWidth = static_cast<float>(m_fontCached.getGlyph(' ', textSize, bold).advance) * 4;
            else
                charWidth = static_cast<float>(m_fontCached.getGlyph(curChar, textSize, bold).advance);

            const float kerning = static_cast<float>(m_fontCached.getKerning(prevChar, curChar, textSize));
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

        std::size_t pos = std::min(m_selStart, m_selEnd);

        // Erase the characters
        sf::String displayedString = m_textFull.getString();
        displayedString.erase(pos, m_selChars);
        m_textFull.setString(displayedString);
        m_text.erase(pos, m_selChars);

        // Set the caret back on the correct position
        setCaretPosition(pos);

        // If the text can be moved to the right then do so
        const float width = getVisibleEditBoxWidth();
        const float textWidth = m_textFull.getSize().x;
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
        float textX = m_paddingCached.left - m_textCropPosition;
        float textY = m_paddingCached.top + (((getInnerSize().y - m_paddingCached.bottom - m_paddingCached.top) - m_textFull.getSize().y) / 2.f);

        // Check if the layout wasn't left
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the text width
            const float textWidth = m_textFull.getSize().x;

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
        m_textBeforeSelection.setPosition(textX, textY);
        m_defaultText.setPosition(textX, textY);

        // Check if there is a selection
        if (m_selChars != 0)
        {
            // Watch out for the kerning
            if (m_textBeforeSelection.getString().getSize() > 0)
                textX += m_fontCached.getKerning(m_textFull.getString()[m_textBeforeSelection.getString().getSize() - 1], m_textFull.getString()[m_textBeforeSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            textX += m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_selectedTextBackground.setSize({m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x,
                                              getInnerSize().y - m_paddingCached.top - m_paddingCached.bottom});
            m_selectedTextBackground.setPosition({textX, m_paddingCached.top});

            // Set the text selected text on the correct position
            m_textSelection.setPosition(textX, textY);

            // Watch out for kerning
            if (m_textFull.getString().getSize() > m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize())
                textX += m_fontCached.getKerning(m_textFull.getString()[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize() - 1], m_textFull.getString()[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            // Set the text selected text on the correct position
            textX += m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x;
            m_textAfterSelection.setPosition(textX, textY);
        }

        // Set the position of the caret
        caretLeft += m_textFull.findCharacterPos(m_selEnd).x - (m_caret.getSize().x * 0.5f);
        m_caret.setPosition({caretLeft, m_paddingCached.top});
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
        if (m_bordersCached != Borders{0})
        {
            if (!m_enabled && m_borderColorDisabledCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorDisabledCached);
            else if (m_mouseHover && m_borderColorHoverCached.isSet())
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorHoverCached);
            else
                drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);

            states.transform.translate({m_bordersCached.left, m_bordersCached.top});
        }

        // Draw the background
        if (m_sprite.isSet())
        {
            if (m_spriteDisabled.isSet())
                m_spriteDisabled.draw(target, states);
            else
            {
                if (m_mouseHover && m_spriteHover.isSet())
                    m_spriteHover.draw(target, states);
                else
                    m_sprite.draw(target, states);

                // When the edit box is focused then draw an extra image
                if (m_focused && m_spriteFocused.isSet())
                    m_spriteFocused.draw(target, states);
            }
        }
        else // There is no background texture
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorDisabledCached);
            else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorHoverCached);
            else
                drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);
        }

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        Clipping clipping{target, states, {m_paddingCached.left, m_paddingCached.top}, {getInnerSize().x - m_paddingCached.left - m_paddingCached.right, getInnerSize().y - m_paddingCached.top - m_paddingCached.bottom}};

        if ((m_textBeforeSelection.getString() != "") || (m_textSelection.getString() != ""))
        {
            m_textBeforeSelection.draw(target, states);

            if (m_textSelection.getString() != "")
            {
                states.transform.translate(m_selectedTextBackground.getPosition());
                drawRectangleShape(target, states, m_selectedTextBackground.getSize(), m_selectedTextBackgroundColorCached);
                states.transform.translate(-m_selectedTextBackground.getPosition());

                m_textSelection.draw(target, states);
                m_textAfterSelection.draw(target, states);
            }
        }
        else if (m_defaultText.getString() != "")
        {
            m_defaultText.draw(target, states);
        }

        // Draw the caret
        states.transform.translate(m_caret.getPosition());
        if (m_focused && m_caretVisible)
        {
            if (!m_enabled && m_caretColorDisabledCached.isSet())
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorDisabledCached);
            else if (m_mouseHover && m_caretColorHoverCached.isSet())
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorHoverCached);
            else
                drawRectangleShape(target, states, m_caret.getSize(), m_caretColorCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
