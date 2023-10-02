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


#include <TGUI/Container.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Keyboard.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const char32_t* EditBox::Validator::All   = U".*";
    const char32_t* EditBox::Validator::Int   = U"[+-]?[0-9]*";
    const char32_t* EditBox::Validator::UInt  = U"[0-9]*";
    const char32_t* EditBox::Validator::Float = U"[+-]?[0-9]*\\.?[0-9]*";

#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char EditBox::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false},
        m_regex        {m_regexString.toWideString()}  // Not in header to speed up compilation
    {
        m_textBeforeSelection.setFont(m_fontCached);
        m_textSelection.setFont(m_fontCached);
        m_textAfterSelection.setFont(m_fontCached);
        m_textFull.setFont(m_fontCached);
        m_textSuffix.setFont(m_fontCached);
        m_defaultText.setFont(m_fontCached);

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<EditBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor()); // Needs to be set here in case renderer lacks "DefaultTextColor" property

            setTextSize(getGlobalTextSize());
            setSize({m_textFull.getLineHeight() * 10,
                     std::round(m_textFull.getLineHeight() * 1.25f) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::create()
    {
        return std::make_shared<EditBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Ptr EditBox::copy(const EditBox::ConstPtr& editBox)
    {
        if (editBox)
            return std::static_pointer_cast<EditBox>(editBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBox::getSharedRenderer()
    {
        return aurora::downcast<EditBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const EditBoxRenderer* EditBox::getSharedRenderer() const
    {
        return aurora::downcast<const EditBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBoxRenderer* EditBox::getRenderer()
    {
        return aurora::downcast<EditBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(const Layout2d& size)
    {
        ClickableWidget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        // Recalculate the text size when auto scaling
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            updateTextSize();

        m_sprite.setSize(getInnerSize());
        m_spriteHover.setSize(getInnerSize());
        m_spriteDisabled.setSize(getInnerSize());
        m_spriteFocused.setSize(getInnerSize());

        // Set the size of the caret
        m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});

        // Recalculate the position of the texts
        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setEnabled(bool enabled)
    {
        ClickableWidget::setEnabled(enabled);
        updateTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const String& text)
    {
        // Change the text if allowed
        if ((m_regexString == U".*") || std::regex_match(text.toWideString(), m_regex))
            m_text = text;
        else // Clear the text
            m_text = U"";

        // Remove all the excess characters if there is a character limit
        if ((m_maxChars > 0) && (m_text.length() > m_maxChars))
            m_text.erase(m_maxChars, String::npos);

        // Set the password character again to trigger some other text updates
        setPasswordCharacter(m_passwordChar);

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& EditBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setDefaultText(const String& text)
    {
        m_defaultText.setString(text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& EditBox::getDefaultText() const
    {
        return m_defaultText.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::selectText(std::size_t start, std::size_t length)
    {
        m_selStart = std::min(m_text.length(), start);
        updateSelEnd(length == String::npos ? m_text.length() : std::min(m_text.length(), start + length));
        updateSelection();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String EditBox::getSelectedText() const
    {
        const auto startIndex = std::min(m_selStart, m_selEnd);
        const auto endIndex = std::max(m_selStart, m_selEnd);
        return m_text.substr(startIndex, endIndex - startIndex);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordCharacter(char32_t passwordChar)
    {
        m_passwordChar = passwordChar;

        m_displayedText = m_text;
        if (m_passwordChar != U'\0')
            std::fill(m_displayedText.begin(), m_displayedText.end(), m_passwordChar);

        m_textFull.setString(m_displayedText);
        m_textBeforeSelection.setString(m_textFull.getString());
        m_textSelection.setString("");
        m_textAfterSelection.setString("");

        updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char32_t EditBox::getPasswordCharacter() const
    {
        return m_passwordChar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_maxChars = maxChars;

        // If there is a character limit then check if it is exceeded
        if ((m_maxChars > 0) && (m_displayedText.length() > m_maxChars))
        {
            // Remove all the excess characters
            m_text.erase(m_maxChars, String::npos);
            m_displayedText.erase(m_maxChars, String::npos);

            // If we passed here then the text has changed.
            m_textFull.setString(m_displayedText);
            m_textBeforeSelection.setString(m_textFull.getString());
            m_textSelection.setString("");
            m_textAfterSelection.setString("");

            // Set the caret behind the last character
            setCaretPosition(m_displayedText.length());
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

        setText(getText());
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
            while (!m_displayedText.empty() && (getFullTextWidth() > width))
            {
                m_displayedText.erase(m_displayedText.length()-1);
                m_textFull.setString(m_displayedText);
                m_text.erase(m_text.length()-1);
            }

            m_textBeforeSelection.setString(m_textFull.getString());

            // There is no clipping
            m_textCropPosition = 0;

            // If the caret was behind the limit, then set it at the end
            if (m_selEnd > m_displayedText.length())
                setCaretPosition(m_selEnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isTextWidthLimited() const
    {
        return m_limitTextWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setReadOnly(bool readOnly)
    {
        m_readOnly = readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isReadOnly() const
    {
        return m_readOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setCaretPosition(std::size_t charactersBeforeCaret)
    {
        // The caret position has to stay inside the string
        if (charactersBeforeCaret > m_text.length())
            charactersBeforeCaret = m_text.length();

        // Set the caret to the correct position
        m_selStart = charactersBeforeCaret;
        updateSelEnd(charactersBeforeCaret);
        updateSelection();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::getCaretPosition() const
    {
        return m_selEnd;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::setInputValidator(const String& regex)
    {
        try
        {
            m_regex = regex.toWideString();
        }
        catch (const std::regex_error&)
        {
            return false;
        }

        m_regexString = regex.toWideString();
        setText(m_text);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& EditBox::getInputValidator() const
    {
        return m_regexString;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSuffix(const String& suffix)
    {
        m_textSuffix.setString(suffix);
        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& EditBox::getSuffix() const
    {
        return m_textSuffix.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setFocused(bool focused)
    {
        if (m_parentGui)
        {
            if (focused)
            {
                const auto absoluteInnerPos = getAbsolutePosition(m_bordersCached.getOffset());
                const FloatRect inputRect = {absoluteInnerPos, getAbsolutePosition(getInnerSize()) - absoluteInnerPos};
                m_parentGui->startTextInput(inputRect);
            }
            else
                m_parentGui->stopTextInput();
        }

        if (focused)
        {
            m_caretVisible = true;
            m_animationTimeElapsed = {};
        }
        else // Unfocusing
        {
            // If there is a selection then undo it now
            if (m_selChars)
                setCaretPosition(m_selEnd);

            if (m_focused)
                onReturnOrUnfocus.emit(this, m_text);
        }

        ClickableWidget::setFocused(focused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::isMouseOnWidget(Vector2f pos) const
    {
        if (ClickableWidget::isMouseOnWidget(pos))
        {
            if (!m_transparentTextureCached || !m_sprite.isSet() || !m_sprite.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::leftMousePressed(Vector2f pos)
    {
        ClickableWidget::leftMousePressed(pos);

        pos -= getPosition();

        // Find the caret position
        const float positionX = pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft();

        std::size_t caretPosition = findCaretPosition(positionX);
        const float textOffset = m_textFull.getExtraHorizontalPadding();

        // When clicking on the left of the first character, move the caret to the left
        if ((positionX < textOffset) && (caretPosition > 0))
            --caretPosition;

        // When clicking on the right of the right character, move the caret to the right
        else if ((positionX > getVisibleEditBoxWidth() - textOffset) && (caretPosition < m_displayedText.length()))
            ++caretPosition;

        // Check if this is a double click
        if ((m_possibleDoubleClick) && (m_selChars == 0) && (caretPosition == m_selEnd))
        {
            // The next click is going to be a normal one again
            m_possibleDoubleClick = false;

            // Set the caret at the end of the text
            setCaretPosition(m_displayedText.length());

            // Select the whole text
            m_selStart = 0;
            updateSelEnd(m_text.length());
            updateSelection();
        }
        else // No double clicking
        {
            // Set the new caret
            setCaretPosition(caretPosition);

            // If the next click comes soon enough then it will be a double click
            m_possibleDoubleClick = true;
        }

        // The caret should be visible
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        // The user may be dragging to select text
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_mouseDown)
        {
            const auto oldSelEnd = m_selEnd;

            // Check if there is a text width limit
            if (m_limitTextWidth)
            {
                // Find out between which characters the mouse is standing
                updateSelEnd(findCaretPosition(pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft()));
            }
            else // Scrolling is enabled
            {
                const float width = getVisibleEditBoxWidth();
                const float textOffset = m_textFull.getExtraHorizontalPadding();

                // Check if the mouse is on the left of the text
                if (pos.x < m_bordersCached.getLeft() + m_paddingCached.getLeft() + textOffset)
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
                            m_textCropPosition -= 1;
                    }
                }
                // Check if the mouse is on the right of the text AND there is a possibility to scroll
                else if ((pos.x > m_bordersCached.getLeft() + m_paddingCached.getLeft() + width - textOffset) && (getFullTextWidth() > width))
                {
                    // Move the text by a few pixels
                    if (m_textFull.getCharacterSize() > 10)
                    {
                        const float pixelsToMove = m_textFull.getCharacterSize() / 10.f;
                        if (m_textCropPosition + width + pixelsToMove < getFullTextWidth())
                            m_textCropPosition += static_cast<unsigned int>(pixelsToMove);
                        else
                            m_textCropPosition = static_cast<unsigned int>(getFullTextWidth() - width);
                    }
                    else
                    {
                        if (m_textCropPosition + width < getFullTextWidth())
                            m_textCropPosition += 1;
                    }
                }

                // Find out between which characters the mouse is standing
                updateSelEnd(findCaretPosition(pos.x - m_bordersCached.getLeft() - m_paddingCached.getLeft()));
            }

            if (m_selEnd != oldSelEnd)
                updateSelection();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(const Event::KeyEvent& event)
    {
        if (event.code == Event::KeyboardKey::Enter)
        {
            onReturnKeyPress.emit(this, m_text);
            onReturnOrUnfocus.emit(this, m_text);
        }
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
            selectText();
        else
        {
            bool caretMoved = true;
            if (keyboard::isKeyPressMoveCaretLeft(event))
                moveCaretLeft(event.shift);
            else if (keyboard::isKeyPressMoveCaretRight(event))
                moveCaretRight(event.shift);
            else if (keyboard::isKeyPressMoveCaretWordBegin(event))
                moveCaretWordBegin();
            else if (keyboard::isKeyPressMoveCaretWordEnd(event))
                moveCaretWordEnd();
            else if (keyboard::isKeyPressMoveCaretLineStart(event) || keyboard::isKeyPressMoveCaretUp(event)
                  || keyboard::isKeyPressMoveCaretDocumentBegin(event) || (event.code == Event::KeyboardKey::PageUp))
                updateSelEnd(0);
            else if (keyboard::isKeyPressMoveCaretLineEnd(event) || keyboard::isKeyPressMoveCaretDown(event)
                  || keyboard::isKeyPressMoveCaretDocumentEnd(event) || (event.code == Event::KeyboardKey::PageDown))
                updateSelEnd(m_text.length());
            else
                caretMoved = false;

            if (caretMoved)
            {
                if (!event.shift)
                    m_selStart = m_selEnd;

                updateSelection();
            }
        }

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Enter)
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
         || keyboard::isKeyPressMoveCaretDocumentBegin(event)
         || keyboard::isKeyPressMoveCaretLineEnd(event)
         || keyboard::isKeyPressMoveCaretDocumentEnd(event)
         || (keyboard::isKeyPressMoveCaretUp(event) && !m_navWidgetUp.lock())
         || (keyboard::isKeyPressMoveCaretDown(event) && !m_navWidgetDown.lock()))
        {
            return true;
        }
        else
            return ClickableWidget::canHandleKeyPress(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::textEntered(char32_t key)
    {
        if (m_readOnly)
            return;

        // Only add the character when the regex matches
        if (m_regexString != U".*")
        {
            String text = m_text;
            if (m_selChars == 0)
                text.insert(m_selEnd, 1, key);
            else
            {
                const std::size_t pos = std::min(m_selStart, m_selEnd);
                text.erase(pos, m_selChars);
                text.insert(pos, 1, key);
            }

            // The character has to match the regex
            if (!std::regex_match(text.toWideString(), m_regex))
                return;
        }

        // If there are selected characters then delete them first
        if (m_selChars > 0)
            deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_maxChars > 0) && (m_text.length() + 1 > m_maxChars))
            return;

        // Insert our character
        m_text.insert(m_selEnd, 1, key);

        // Change the displayed text
        if (m_passwordChar != U'\0')
            m_displayedText.insert(m_selEnd, 1, m_passwordChar);
        else
            m_displayedText.insert(m_selEnd, 1, key);

        m_textFull.setString(m_displayedText);

        // When there is a text width limit then reverse what we just did
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (getFullTextWidth() > getVisibleEditBoxWidth())
            {
                // If the text does not fit in the EditBox then delete the added character
                m_text.erase(m_selEnd, 1);
                m_displayedText.erase(m_selEnd, 1);
                m_textFull.setString(m_displayedText);
                return;
            }
        }

        // Move our caret forward
        setCaretPosition(m_selEnd + 1);

        // The caret should be visible again
        m_caretVisible = true;
        m_animationTimeElapsed = {};

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& EditBox::getSignal(String signalName)
    {
        if (signalName == onTextChange.getName())
            return onTextChange;
        else if (signalName == onReturnKeyPress.getName())
            return onReturnKeyPress;
        else if (signalName == onReturnOrUnfocus.getName())
            return onReturnOrUnfocus;
        else if (signalName == onCaretPositionChange.getName())
            return onCaretPositionChange;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            m_paddingCached.updateParentSize(getSize());
            updateTextSize();

            m_caret.setSize({m_caret.getSize().x, getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});
        }
        else if (property == U"CaretWidth")
        {
            m_caret.setPosition({m_caret.getPosition().x + ((m_caret.getSize().x - getSharedRenderer()->getCaretWidth()) / 2.0f), m_caret.getPosition().y});
            m_caret.setSize({getSharedRenderer()->getCaretWidth(), getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()});
        }
        else if ((property == U"TextColor") || (property == U"TextColorDisabled") || (property == U"TextColorFocused"))
        {
            updateTextColor();
        }
        else if (property == U"SelectedTextColor")
        {
            m_textSelection.setColor(getSharedRenderer()->getSelectedTextColor());
        }
        else if (property == U"DefaultTextColor")
        {
            m_defaultText.setColor(getSharedRenderer()->getDefaultTextColor());
        }
        else if (property == U"Texture")
        {
            m_sprite.setTexture(getSharedRenderer()->getTexture());
        }
        else if (property == U"TextureHover")
        {
            m_spriteHover.setTexture(getSharedRenderer()->getTextureHover());
        }
        else if (property == U"TextureDisabled")
        {
            m_spriteDisabled.setTexture(getSharedRenderer()->getTextureDisabled());
        }
        else if (property == U"TextureFocused")
        {
            m_spriteFocused.setTexture(getSharedRenderer()->getTextureFocused());
        }
        else if (property == U"TextStyle")
        {
            const TextStyles style = getSharedRenderer()->getTextStyle();
            m_textBeforeSelection.setStyle(style);
            m_textAfterSelection.setStyle(style);
            m_textSelection.setStyle(style);
            m_textSuffix.setStyle(style);
            m_textFull.setStyle(style);

            // The width of the text can be different, which requires the text to be realigned if it was centered or right-aligned
            updateTextSize();
        }
        else if (property == U"DefaultTextStyle")
        {
            m_defaultText.setStyle(getSharedRenderer()->getDefaultTextStyle());
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BorderColorHover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == U"BorderColorDisabled")
        {
            m_borderColorDisabledCached = getSharedRenderer()->getBorderColorDisabled();
        }
        else if (property == U"BorderColorFocused")
        {
            m_borderColorFocusedCached = getSharedRenderer()->getBorderColorFocused();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == U"BackgroundColorDisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == U"BackgroundColorFocused")
        {
            m_backgroundColorFocusedCached = getSharedRenderer()->getBackgroundColorFocused();
        }
        else if (property == U"CaretColor")
        {
            m_caretColorCached = getSharedRenderer()->getCaretColor();
        }
        else if (property == U"CaretColorHover")
        {
            m_caretColorHoverCached = getSharedRenderer()->getCaretColorHover();
        }
        else if (property == U"CaretColorFocused")
        {
            m_caretColorFocusedCached = getSharedRenderer()->getCaretColorFocused();
        }
        else if (property == U"SelectedTextBackgroundColor")
        {
            m_selectedTextBackgroundColorCached = getSharedRenderer()->getSelectedTextBackgroundColor();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            ClickableWidget::rendererChanged(property);

            m_textBeforeSelection.setOpacity(m_opacityCached);
            m_textAfterSelection.setOpacity(m_opacityCached);
            m_textSelection.setOpacity(m_opacityCached);
            m_defaultText.setOpacity(m_opacityCached);
            m_textSuffix.setOpacity(m_opacityCached);

            m_sprite.setOpacity(m_opacityCached);
            m_spriteHover.setOpacity(m_opacityCached);
            m_spriteDisabled.setOpacity(m_opacityCached);
            m_spriteFocused.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            ClickableWidget::rendererChanged(property);

            m_textBeforeSelection.setFont(m_fontCached);
            m_textSelection.setFont(m_fontCached);
            m_textAfterSelection.setFont(m_fontCached);
            m_textSuffix.setFont(m_fontCached);
            m_textFull.setFont(m_fontCached);
            m_defaultText.setFont(m_fontCached);
            updateTextSize();
        }
        else
            ClickableWidget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> EditBox::save(SavingRenderersMap& renderers) const
    {
        auto node = ClickableWidget::save(renderers);

        if (getAlignment() != EditBox::Alignment::Left)
        {
            if (getAlignment() == EditBox::Alignment::Center)
                node->propertyValuePairs[U"Alignment"] = std::make_unique<DataIO::ValueNode>("Center");
            else
                node->propertyValuePairs[U"Alignment"] = std::make_unique<DataIO::ValueNode>("Right");
        }

        if (getInputValidator() != U".*")
        {
            if (getInputValidator() == EditBox::Validator::Int)
                node->propertyValuePairs[U"InputValidator"] = std::make_unique<DataIO::ValueNode>("Int");
            else if (getInputValidator() == EditBox::Validator::UInt)
                node->propertyValuePairs[U"InputValidator"] = std::make_unique<DataIO::ValueNode>("UInt");
            else if (getInputValidator() == EditBox::Validator::Float)
                node->propertyValuePairs[U"InputValidator"] = std::make_unique<DataIO::ValueNode>("Float");
            else
                node->propertyValuePairs[U"InputValidator"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize({getInputValidator()}));
        }

        if (!m_text.empty())
            node->propertyValuePairs[U"Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(String(m_text)));
        if (!getDefaultText().empty())
            node->propertyValuePairs[U"DefaultText"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getDefaultText()));
        if (getPasswordCharacter() != '\0')
            node->propertyValuePairs[U"PasswordCharacter"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(String(getPasswordCharacter())));
        if (getMaximumCharacters() != 0)
            node->propertyValuePairs[U"MaximumCharacters"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getMaximumCharacters()));
        if (isTextWidthLimited())
            node->propertyValuePairs[U"TextWidthLimited"] = std::make_unique<DataIO::ValueNode>("true");
        if (isReadOnly())
            node->propertyValuePairs[U"ReadOnly"] = std::make_unique<DataIO::ValueNode>("true");
        if (!getSuffix().empty())
            node->propertyValuePairs[U"Suffix"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(getSuffix()));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        ClickableWidget::load(node, renderers);

        if (node->propertyValuePairs[U"Text"])
            setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value).getString());
        if (node->propertyValuePairs[U"DefaultText"])
            setDefaultText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"DefaultText"]->value).getString());
        if (node->propertyValuePairs[U"MaximumCharacters"])
            setMaximumCharacters(node->propertyValuePairs[U"MaximumCharacters"]->value.toUInt());
        if (node->propertyValuePairs[U"TextWidthLimited"])
            limitTextWidth(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"TextWidthLimited"]->value).getBool());
        if (node->propertyValuePairs[U"ReadOnly"])
            setReadOnly(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"ReadOnly"]->value).getBool());
        if (node->propertyValuePairs[U"Suffix"])
            setSuffix(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Suffix"]->value).getString());
        if (node->propertyValuePairs[U"PasswordCharacter"])
        {
            const String pass = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"PasswordCharacter"]->value).getString();
            if (!pass.empty())
                setPasswordCharacter(pass[0]);
        }
        if (node->propertyValuePairs[U"Alignment"])
        {
            if (node->propertyValuePairs[U"Alignment"]->value == U"Left")
                setAlignment(EditBox::Alignment::Left);
            else if (node->propertyValuePairs[U"Alignment"]->value == U"Center")
                setAlignment(EditBox::Alignment::Center);
            else if (node->propertyValuePairs[U"Alignment"]->value == U"Right")
                setAlignment(EditBox::Alignment::Right);
            else
                throw Exception{U"Failed to parse Alignment property. Only the values Left, Center and Right are correct."};
        }
        if (node->propertyValuePairs[U"InputValidator"])
        {
            if (node->propertyValuePairs[U"InputValidator"]->value == U"Int")
                setInputValidator(EditBox::Validator::Int);
            else if (node->propertyValuePairs[U"InputValidator"]->value == U"UInt")
                setInputValidator(EditBox::Validator::UInt);
            else if (node->propertyValuePairs[U"InputValidator"]->value == U"Float")
                setInputValidator(EditBox::Validator::Float);
            else
                setInputValidator(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"InputValidator"]->value).getString());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getFullTextWidth() const
    {
        return m_textFull.getSize().x + (2 * m_textFull.getExtraHorizontalPadding());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f EditBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float EditBox::getVisibleEditBoxWidth() const
    {
        float extraSuffixWidth = 0;
        if (!m_textSuffix.getString().empty())
        {
            const float textOffset = m_textFull.getExtraHorizontalPadding();
            extraSuffixWidth = m_textSuffix.getSize().x + textOffset;
        }

        return std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight() - extraSuffixWidth);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t EditBox::findCaretPosition(float posX)
    {
        // Take the part outside the edit box into account when the text does not fit inside it
        posX += m_textCropPosition;

        if (m_textAlignment == Alignment::Left)
            posX -= m_textFull.getExtraHorizontalPadding();
        else
        {
            // If the text is centered or aligned to the right then the position has to be corrected when the edit box is not entirely full
            const float editBoxWidth = getVisibleEditBoxWidth();
            const float textWidth = getFullTextWidth();
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
        char32_t prevChar = 0;
        const unsigned int textSize = getTextSize();
        const bool bold = (m_textFull.getStyle() & TextStyle::Bold) != 0;

        std::size_t index;
        for (index = 0; index < m_text.length(); ++index)
        {
            float charWidth;
            const char32_t curChar = m_text[index];
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

            const float kerning = m_fontCached.getKerning(prevChar, curChar, textSize, bold);
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

        const std::size_t pos = std::min(m_selStart, m_selEnd);

        // Erase the characters
        m_displayedText.erase(pos, m_selChars);
        m_textFull.setString(m_displayedText);
        m_text.erase(pos, m_selChars);

        // Set the caret back on the correct position
        setCaretPosition(pos);

        // If the text can be moved to the right then do so
        const float width = getVisibleEditBoxWidth();
        const float textWidth = getFullTextWidth();
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
        // If the edit box is resized then it might happen that text which previously didn't fit will now fit inside it
        if (m_textCropPosition != 0)
        {
            const float textWidth = getFullTextWidth();
            const float maxTextCropPosition = textWidth - getVisibleEditBoxWidth();
            m_textCropPosition = std::min(m_textCropPosition, static_cast<unsigned int>(std::max(0.f, maxTextCropPosition)));
        }

        const float textOffset = m_textFull.getExtraHorizontalPadding();
        float textX = m_paddingCached.getLeft() - m_textCropPosition + textOffset;
        const float textY = m_paddingCached.getTop() + (((getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()) - m_textFull.getSize().y) / 2.f);

        // Check if the layout wasn't left
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the text width
            const float textWidth = m_displayedText.empty() ? (m_defaultText.getSize().x + 2 * textOffset) : getFullTextWidth();

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
        m_textBeforeSelection.setPosition({textX, textY});
        m_defaultText.setPosition({textX, textY});

        // Check if there is a selection
        if (m_selChars != 0)
        {
            const auto charsBeforeSelection = std::min(m_selStart, m_selEnd);

            // Watch out for the kerning
            if (charsBeforeSelection > 0)
            {
                const bool bold = (m_textFull.getStyle() & TextStyle::Bold) != 0;
                textX += m_fontCached.getKerning(m_displayedText[charsBeforeSelection - 1],
                                                 m_displayedText[charsBeforeSelection],
                                                 m_textBeforeSelection.getCharacterSize(),
                                                 bold);
            }

            textX += m_textBeforeSelection.findCharacterPos(charsBeforeSelection).x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_selectedTextBackground.setSize({m_textSelection.findCharacterPos(m_selChars).x, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()});
            m_selectedTextBackground.setPosition({textX, m_paddingCached.getTop()});

            // Set the text selected text on the correct position
            m_textSelection.setPosition({textX, textY});

            // Watch out for kerning
            if (m_displayedText.length() > charsBeforeSelection + m_selChars)
            {
                const bool bold = (m_textFull.getStyle() & TextStyle::Bold) != 0;
                textX += m_fontCached.getKerning(m_displayedText[charsBeforeSelection + m_selChars - 1],
                                                 m_displayedText[charsBeforeSelection + m_selChars],
                                                 m_textBeforeSelection.getCharacterSize(),
                                                 bold);
            }

            // Set the text selected text on the correct position
            textX += m_textSelection.findCharacterPos(m_selChars).x;
            m_textAfterSelection.setPosition({textX, textY});
        }

        // Set the position of the caret
        caretLeft += m_textFull.findCharacterPos(m_selEnd).x - (m_caret.getSize().x * 0.5f);
        m_caret.setPosition({caretLeft, m_paddingCached.getTop()});

        if (m_parentGui)
        {
            // The SDL backend will place the IME candidate window below the input rectangle, but for some reason, when the first
            // character is typed it places the window inside the rectangle at the top position. Instead of making the entire
            // edit box our rectangle, we will only select the area of the text itself, to reduce the distance that the window
            // position jumps between its initial and later positions. Other backends (SFML and GLFW) currently ignore the rectangle.
            const Vector2f textPos = {m_bordersCached.getLeft() + m_paddingCached.getLeft(), textY};
            const auto absoluteTextPos = getAbsolutePosition(textPos);
            const FloatRect inputRect = {absoluteTextPos, getAbsolutePosition({textPos.x, textPos.y + m_textFull.getSize().y}) - absoluteTextPos};
            m_parentGui->updateTextCursorPosition(inputRect, getAbsolutePosition({caretLeft + m_caret.getSize().x, textY}));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateSelection()
    {
        // Check if we are selecting text from left to right
        if (m_selEnd > m_selStart)
        {
            // Update the number of characters that are selected
            m_selChars = m_selEnd - m_selStart;

            // Change our three texts
            m_textBeforeSelection.setString(m_displayedText.substr(0, m_selStart));
            m_textSelection.setString(m_displayedText.substr(m_selStart, m_selChars));
            m_textAfterSelection.setString(m_displayedText.substr(m_selEnd));
        }
        else if (m_selEnd < m_selStart)
        {
            // Update the number of characters that are selected
            m_selChars = m_selStart - m_selEnd;

            // Change our three texts
            m_textBeforeSelection.setString(m_displayedText.substr(0, m_selEnd));
            m_textSelection.setString(m_displayedText.substr(m_selEnd, m_selChars));
            m_textAfterSelection.setString(m_displayedText.substr(m_selStart));
        }
        else
        {
            // Update the number of characters that are selected
            m_selChars = 0;

            // Change our three texts
            m_textBeforeSelection.setString(m_textFull.getString());
            m_textSelection.setString("");
            m_textAfterSelection.setString("");
        }

        if (!m_fontCached)
            return;

        // Check if scrolling is enabled
        if (!m_limitTextWidth)
        {
            // Find out the position of the caret
            const float caretPosition = m_textFull.findCharacterPos(m_selEnd).x;

            // If the caret is too far on the right then adjust the cropping
            if (m_textCropPosition + getVisibleEditBoxWidth() - (2 * m_textFull.getExtraHorizontalPadding()) < caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition - getVisibleEditBoxWidth() + (2 * m_textFull.getExtraHorizontalPadding()));

            // If the caret is too far on the left then adjust the cropping
            if (m_textCropPosition > caretPosition)
                m_textCropPosition = static_cast<unsigned int>(caretPosition);
        }

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateTextColor()
    {
        if (!m_enabled && getSharedRenderer()->getTextColorDisabled().isSet())
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColorDisabled());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColorDisabled());
            m_textSuffix.setColor(getSharedRenderer()->getTextColorDisabled());
        }
        else if (m_focused && getSharedRenderer()->getTextColorFocused().isSet())
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColorFocused());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColorFocused());
            m_textSuffix.setColor(getSharedRenderer()->getTextColorFocused());
        }
        else
        {
            m_textBeforeSelection.setColor(getSharedRenderer()->getTextColor());
            m_textAfterSelection.setColor(getSharedRenderer()->getTextColor());
            m_textSuffix.setColor(getSharedRenderer()->getTextColor());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateTextSize()
    {
        // Check if the text is auto sized
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            m_textSizeCached = Text::findBestTextSize(m_fontCached, (getInnerSize().y - m_paddingCached.getBottom() - m_paddingCached.getTop()) * 0.8f);

        m_textFull.setCharacterSize(m_textSizeCached);
        m_textSuffix.setCharacterSize(m_textSizeCached);
        m_textBeforeSelection.setCharacterSize(m_textSizeCached);
        m_textSelection.setCharacterSize(m_textSizeCached);
        m_textAfterSelection.setCharacterSize(m_textSizeCached);
        m_defaultText.setCharacterSize(m_textSizeCached);

        // Check if there is a text width limit
        const float width = getVisibleEditBoxWidth();
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            bool textChanged = false;
            while (!m_displayedText.empty() && (getFullTextWidth() > width))
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                m_displayedText.erase(m_displayedText.length()-1);
                m_textFull.setString(m_displayedText);
                m_text.erase(m_text.length()-1);
                textChanged = true;
            }

            if (textChanged)
            {
                m_textBeforeSelection.setString(m_textFull.getString());
                setCaretPosition(m_displayedText.length());
            }
        }
        else // There is no text cropping
        {
            // If the text can be moved to the right then do so
            const float textWidth = getFullTextWidth();
            if (textWidth > width)
            {
                if (textWidth - m_textCropPosition < width)
                    m_textCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_textCropPosition = 0;
        }

        // Set the caret behind the last character and update the selection
        setCaretPosition(m_displayedText.length());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::updateTime(Duration elapsedTime)
    {
        bool screenRefreshRequired = ClickableWidget::updateTime(elapsedTime);

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

    void EditBox::backspaceKeyPressed()
    {
        if (m_readOnly)
            return;

        // Make sure that we did not select any characters
        if (m_selChars == 0)
        {
            // We can't delete any characters when you are at the beginning of the string
            if (m_selEnd == 0)
                return;

            // Erase the character
            m_displayedText.erase(m_selEnd-1, 1);
            m_textFull.setString(m_displayedText);
            m_text.erase(m_selEnd-1, 1);

            // Set the caret back on the correct position
            setCaretPosition(m_selEnd - 1);

            const float width = getVisibleEditBoxWidth();

            // If the text can be moved to the right then do so
            const float textWidth = getFullTextWidth();
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

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::deleteKeyPressed()
    {
        if (m_readOnly)
            return;

        // Make sure that no text is selected
        if (m_selChars == 0)
        {
            // When the caret is at the end of the line then you can't delete anything
            if (m_selEnd == m_text.length())
                return;

            // Erase the character
            m_displayedText.erase(m_selEnd, 1);
            m_textFull.setString(m_displayedText);
            m_text.erase(m_selEnd, 1);

            // Set the caret back on the correct position
            setCaretPosition(m_selEnd);

            // If the text can be moved to the right then do so
            const float width = getVisibleEditBoxWidth();
            const float textWidth = getFullTextWidth();
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

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::copySelectedTextToClipboard()
    {
        getBackend()->setClipboard(m_textSelection.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::cutSelectedTextToClipboard()
    {
        getBackend()->setClipboard(m_textSelection.getString());

        if (m_readOnly)
            return;

        deleteSelectedCharacters();

        onTextChange.emit(this, m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::pasteTextFromClipboard()
    {
        if (m_readOnly)
            return;

        // Only continue pasting if you actually have to do something
        const auto clipboardContents = getBackend()->getClipboard();
        if ((m_selChars > 0) || (clipboardContents.length() > 0))
        {
            deleteSelectedCharacters();

            const std::size_t oldCaretPos = m_selEnd;

            if (m_text.length() > m_selEnd)
                setText(m_text.substr(0, m_selEnd) + clipboardContents + m_text.substr(m_selEnd, m_text.length() - m_selEnd));
            else
                setText(m_text + clipboardContents);

            setCaretPosition(oldCaretPos + clipboardContents.length());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretLeft(bool shiftPressed)
    {
        // If text is selected then move to the cursor to the left side of the selected text
        if ((m_selChars > 0) && !shiftPressed)
            updateSelEnd(std::min(m_selStart, m_selEnd));
        else if (m_selEnd > 0)
            updateSelEnd(m_selEnd - 1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretRight(bool shiftPressed)
    {
        // If text is selected then move to the cursor to the right side of the selected text
        if ((m_selChars > 0) && !shiftPressed)
            updateSelEnd(std::max(m_selStart, m_selEnd));
        else if (m_selEnd < m_text.length())
            updateSelEnd(m_selEnd + 1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretWordBegin()
    {
        // Move to the beginning of the word (or to the beginning of the previous word when already at the beginning)
        bool done = false;
        bool skippedWhitespace = false;
        for (std::size_t i = m_selEnd; i > 0; --i)
        {
            if (skippedWhitespace)
            {
                if (isWhitespace(m_text[i-1]))
                {
                    updateSelEnd(i);
                    done = true;
                    break;
                }
            }
            else
            {
                if (!isWhitespace(m_text[i-1]))
                    skippedWhitespace = true;
            }
        }

        if (!done && skippedWhitespace)
            updateSelEnd(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::moveCaretWordEnd()
    {
        // Move to the end of the word (or to the end of the next word when already at the end)
        bool done = false;
        bool skippedWhitespace = false;
        for (std::size_t i = m_selEnd; i < m_text.length(); ++i)
        {
            if (skippedWhitespace)
            {
                if (isWhitespace(m_text[i]))
                {
                    updateSelEnd(i);
                    done = true;
                    break;
                }
            }
            else
            {
                if (!isWhitespace(m_text[i]))
                    skippedWhitespace = true;
            }
        }

        if (!done && skippedWhitespace)
            updateSelEnd(m_text.length());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::updateSelEnd(const std::size_t newValue)
    {
        const bool emit = newValue != m_selEnd;
        m_selEnd = newValue;
        if (emit)
            onCaretPositionChange.emit(this, m_selEnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            if (!m_enabled && m_borderColorDisabledCached.isSet())
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorDisabledCached, m_opacityCached));
            else if (m_mouseHover && m_borderColorHoverCached.isSet())
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorHoverCached, m_opacityCached));
            else if (m_focused && m_borderColorFocusedCached.isSet())
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorFocusedCached, m_opacityCached));
            else
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));

            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_sprite.isSet())
        {
            if (!m_enabled && m_spriteDisabled.isSet())
                target.drawSprite(states, m_spriteDisabled);
            else if (m_mouseHover && m_spriteHover.isSet())
                target.drawSprite(states, m_spriteHover);
            else if (m_focused && m_spriteFocused.isSet())
                target.drawSprite(states, m_spriteFocused);
            else
                target.drawSprite(states, m_sprite);
        }
        else // There is no background texture
        {
            if (!m_enabled && m_backgroundColorDisabledCached.isSet())
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorDisabledCached, m_opacityCached));
            else if (m_mouseHover && m_backgroundColorHoverCached.isSet())
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorHoverCached, m_opacityCached));
            else if (m_focused && m_backgroundColorFocusedCached.isSet())
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorFocusedCached, m_opacityCached));
            else
                target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
        }

        // Draw the suffix
        float suffixSpace = 0;
        if (!m_textSuffix.getString().empty())
        {
            target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()},
                {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}});

            const float textOffset = m_textFull.getExtraHorizontalPadding();
            Vector2f offset{getInnerSize().x - m_paddingCached.getRight() - textOffset - m_textSuffix.getSize().x,
                            m_paddingCached.getTop() + ((getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - m_textSuffix.getSize().y) / 2.f)};

            states.transform.translate(offset);
            target.drawText(states, m_textSuffix);
            states.transform.translate(-offset);

            target.removeClippingLayer();

            suffixSpace = m_textSuffix.getSize().x + textOffset;
        }

        // Draw the text
        {
            target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()},
                {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight() - suffixSpace, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}});

            if (!m_textBeforeSelection.getString().empty() || !m_textSelection.getString().empty())
            {
                target.drawText(states, m_textBeforeSelection);

                if (!m_textSelection.getString().empty())
                {
                    states.transform.translate(m_selectedTextBackground.getPosition());
                    target.drawFilledRect(states, m_selectedTextBackground.getSize(), Color::applyOpacity(m_selectedTextBackgroundColorCached, m_opacityCached));
                    states.transform.translate(-m_selectedTextBackground.getPosition());

                    target.drawText(states, m_textSelection);
                    target.drawText(states, m_textAfterSelection);
                }
            }
            else if (!m_defaultText.getString().empty())
            {
                target.drawText(states, m_defaultText);
            }

            target.removeClippingLayer();
        }

        // Draw the caret
        states.transform.translate(m_caret.getPosition());
        if (m_enabled && m_focused && m_caretVisible)
        {
            if (m_mouseHover && m_caretColorHoverCached.isSet())
                target.drawFilledRect(states, m_caret.getSize(), Color::applyOpacity(m_caretColorHoverCached, m_opacityCached));
            else if (m_caretColorFocusedCached.isSet())
                target.drawFilledRect(states, m_caret.getSize(), Color::applyOpacity(m_caretColorFocusedCached, m_opacityCached));
            else
                target.drawFilledRect(states, m_caret.getSize(), Color::applyOpacity(m_caretColorCached, m_opacityCached));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr EditBox::clone() const
    {
        return std::make_shared<EditBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
