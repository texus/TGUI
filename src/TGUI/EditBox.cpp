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


#include <cmath>

#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/EditBox.hpp>
#include <TGUI/Clipboard.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox() :
    m_selectionPointVisible (true),
    m_limitTextWidth        (false),
    m_displayedText         (""),
    m_text                  (""),
    m_textSize              (0),
    m_textAlignment         (Alignment::Left),
    m_selChars              (0),
    m_selStart              (0),
    m_selEnd                (0),
    m_passwordChar          ('\0'),
    m_maxChars              (0),
    m_textCropPosition      (0),
    m_possibleDoubleClick   (false),
    m_numbersOnly           (false),
    m_separateHoverImage    (false)
    {
        m_callback.widgetType = Type_EditBox;
        m_animatedWidget = true;
        m_draggableWidget = true;
        m_allowFocus = true;

        m_selectionPoint.setSize(sf::Vector2f(1, 0));

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox(const EditBox& copy) :
    ClickableWidget         (copy),
    WidgetBorders           (copy),
    m_loadedConfigFile      (copy.m_loadedConfigFile),
    m_selectionPointVisible (copy.m_selectionPointVisible),
    m_limitTextWidth        (copy.m_limitTextWidth),
    m_displayedText         (copy.m_displayedText),
    m_text                  (copy.m_text),
    m_textSize              (copy.m_textSize),
    m_textAlignment         (copy.m_textAlignment),
    m_selChars              (copy.m_selChars),
    m_selStart              (copy.m_selStart),
    m_selEnd                (copy.m_selEnd),
    m_passwordChar          (copy.m_passwordChar),
    m_maxChars              (copy.m_maxChars),
    m_textCropPosition      (copy.m_textCropPosition),
    m_selectedTextBackground(copy.m_selectedTextBackground),
    m_selectionPoint        (copy.m_selectionPoint),
    m_textBeforeSelection   (copy.m_textBeforeSelection),
    m_textSelection         (copy.m_textSelection),
    m_textAfterSelection    (copy.m_textAfterSelection),
    m_textFull              (copy.m_textFull),
    m_possibleDoubleClick   (copy.m_possibleDoubleClick),
    m_numbersOnly           (copy.m_numbersOnly),
    m_separateHoverImage    (copy.m_separateHoverImage)
    {
        TGUI_TextureManager.copyTexture(copy.m_textureNormal, m_textureNormal);
        TGUI_TextureManager.copyTexture(copy.m_textureHover, m_textureHover);
        TGUI_TextureManager.copyTexture(copy.m_textureFocused, m_textureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::~EditBox()
    {
        if (m_textureNormal.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal);
        if (m_textureHover.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover);
        if (m_textureFocused.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox& EditBox::operator= (const EditBox& right)
    {
        if (this != &right)
        {
            EditBox temp(right);
            this->ClickableWidget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_loadedConfigFile,       temp.m_loadedConfigFile);
            std::swap(m_selectionPointVisible,  temp.m_selectionPointVisible);
            std::swap(m_limitTextWidth,         temp.m_limitTextWidth);
            std::swap(m_displayedText,          temp.m_displayedText);
            std::swap(m_text,                   temp.m_text);
            std::swap(m_textSize,               temp.m_textSize);
            std::swap(m_textAlignment,          temp.m_textAlignment);
            std::swap(m_selChars,               temp.m_selChars);
            std::swap(m_selStart,               temp.m_selStart);
            std::swap(m_selEnd,                 temp.m_selEnd);
            std::swap(m_passwordChar,           temp.m_passwordChar);
            std::swap(m_maxChars,               temp.m_maxChars);
            std::swap(m_textCropPosition,       temp.m_textCropPosition);
            std::swap(m_selectedTextBackground, temp.m_selectedTextBackground);
            std::swap(m_selectionPoint,         temp.m_selectionPoint);
            std::swap(m_textBeforeSelection,    temp.m_textBeforeSelection);
            std::swap(m_textSelection,          temp.m_textSelection);
            std::swap(m_textAfterSelection,     temp.m_textAfterSelection);
            std::swap(m_textFull,               temp.m_textFull);
            std::swap(m_textureNormal,          temp.m_textureNormal);
            std::swap(m_textureHover,           temp.m_textureHover);
            std::swap(m_textureFocused,         temp.m_textureFocused);
            std::swap(m_possibleDoubleClick,    temp.m_possibleDoubleClick);
            std::swap(m_numbersOnly,            temp.m_numbersOnly);
            std::swap(m_separateHoverImage,     temp.m_separateHoverImage);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox* EditBox::clone()
    {
        return new EditBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_loaded = false;

        // Remove the textures when they were loaded before
        if (m_textureNormal.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureNormal);
        if (m_textureHover.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureHover);
        if (m_textureFocused.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureFocused);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_loadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_loadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("EditBox", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_loadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "separatehoverimage")
            {
                m_separateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "textcolor")
            {
                sf::Color color = extractColor(value);
                m_textBeforeSelection.setColor(color);
                m_textAfterSelection.setColor(color);
            }
            else if (property == "selectedtextcolor")
            {
                m_textSelection.setColor(extractColor(value));
            }
            else if (property == "selectedtextbackgroundcolor")
            {
                m_selectedTextBackground.setFillColor(extractColor(value));
            }
            else if (property == "selectionpointcolor")
            {
                m_selectionPoint.setFillColor(extractColor(value));
            }
            else if (property == "selectionpointwidth")
            {
                m_selectionPoint.setSize(sf::Vector2f(tgui::stof(value), m_selectionPoint.getSize().y));
            }
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section EditBox in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section EditBox in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_textureFocused))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section EditBox in " + m_loadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI error: Unrecognized property '" + property + "' in section EditBox in " + m_loadedConfigFile + ".");
        }

        // Make sure the required texture was loaded
        if (m_textureNormal.getData() != nullptr)
        {
            m_loaded = true;
            setSize(m_textureNormal.getSize().x, m_textureNormal.getSize().y);
        }
        else
        {
            TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the EditBox section in " + m_loadedConfigFile + " complete?");
            return false;
        }

        // Auto-size the text
        setTextSize(0);

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& EditBox::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        m_textureHover.setPosition(x, y);
        m_textureNormal.setPosition(x, y);
        m_textureFocused.setPosition(x, y);

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(float width, float height)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Recalculate the text size when auto scaling
        if (m_textSize == 0)
            setText(m_text);

        m_textureHover.setSize(width, height);
        m_textureNormal.setSize(width, height);
        m_textureFocused.setSize(width, height);

        // Set the size of the selection point
        m_selectionPoint.setSize(sf::Vector2f(static_cast<float>(m_selectionPoint.getSize().x),
                                              m_textureNormal.getSize().y - ((m_bottomBorder + m_topBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y))));

        // Recalculate the position of the images and texts
        setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f EditBox::getSize() const
    {
        return m_textureNormal.getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const sf::String& text)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            // Calculate the text size
            m_textFull.setString("kg");
            m_textFull.setCharacterSize(static_cast<unsigned int>(m_textureNormal.getSize().y - ((m_topBorder + m_bottomBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y))));
            m_textFull.setCharacterSize(static_cast<unsigned int>(m_textFull.getCharacterSize() - m_textFull.getLocalBounds().top));
            m_textFull.setString(m_displayedText);

            // Also adjust the character size of the other texts
            m_textBeforeSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textSelection.setCharacterSize(m_textFull.getCharacterSize());
            m_textAfterSelection.setCharacterSize(m_textFull.getCharacterSize());
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_textBeforeSelection.setCharacterSize(m_textSize);
            m_textSelection.setCharacterSize(m_textSize);
            m_textAfterSelection.setCharacterSize(m_textSize);
            m_textFull.setCharacterSize(m_textSize);
        }

        // Change the text
        m_text = text;
        m_displayedText = text;

        // If the edit box only accepts numbers then remove all other characters
        if (m_numbersOnly)
            setNumbersOnly(true);

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
            for (unsigned int i=0; i < m_text.getSize(); ++i)
                m_displayedText[i] = m_passwordChar;
        }

        // Set the texts
        m_textBeforeSelection.setString(m_displayedText);
        m_textSelection.setString("");
        m_textAfterSelection.setString("");
        m_textFull.setString(m_displayedText);

        // Calculate the space inside the edit box
        float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

        // Check if there is a text width limit
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            while (m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x > width)
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

        // Set the selection point behind the last character
        setSelectionPointPosition(m_displayedText.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getText() const
    {
        return m_text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getTextSize() const
    {
        return m_textFull.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextFont(const sf::Font& font)
    {
        m_textBeforeSelection.setFont(font);
        m_textSelection.setFont(font);
        m_textAfterSelection.setFont(font);
        m_textFull.setFont(font);

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* EditBox::getTextFont() const
    {
        return m_textFull.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordCharacter(char passwordChar)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_loaded == false)
            return;

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

        // If there is a character limit then check if it is exeeded
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

            // Set the selection point behind the last character
            setSelectionPointPosition(m_displayedText.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getMaximumCharacters() const
    {
        return m_maxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setBorders(unsigned int borderLeft, unsigned int borderTop, unsigned int borderRight, unsigned int borderBottom)
    {
        // Set the new border size
        m_leftBorder   = borderLeft;
        m_topBorder    = borderTop;
        m_rightBorder  = borderRight;
        m_bottomBorder = borderBottom;

        // Recalculate the text size
        setText(m_text);

        // Set the size of the selection point
        m_selectionPoint.setSize(sf::Vector2f(static_cast<float>(m_selectionPoint.getSize().x),
                                              m_textureNormal.getSize().y - ((m_bottomBorder + m_topBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y))));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::changeColors(const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& selectionPointColor)
    {
        m_textBeforeSelection.setColor(color);
        m_textSelection.setColor(selectedColor);
        m_textAfterSelection.setColor(color);

        m_selectionPoint.setFillColor(selectionPointColor);
        m_selectedTextBackground.setFillColor(selectedBgrColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextColor(const sf::Color& textColor)
    {
        m_textBeforeSelection.setColor(textColor);
        m_textAfterSelection.setColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_textSelection.setColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectedTextBackgroundColor(const sf::Color& selectedTextBackgroundColor)
    {
        m_selectedTextBackground.setFillColor(selectedTextBackgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointColor(const sf::Color& selectionPointColor)
    {
        m_selectionPoint.setFillColor(selectionPointColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getTextColor() const
    {
        return m_textBeforeSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectedTextColor() const
    {
        return m_textSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectedTextBackgroundColor() const
    {
        return m_selectedTextBackground.getFillColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectionPointColor() const
    {
        return m_selectionPoint.getFillColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::limitTextWidth(bool limitWidth)
    {
        m_limitTextWidth = limitWidth;

        // Check if the width is being limited
        if (m_limitTextWidth == true)
        {
            // Calculate the space inside the edit box
            float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

            // Now check if the text fits into the EditBox
            while (m_textBeforeSelection.findCharacterPos(m_displayedText.getSize()).x > width)
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

            // If the selection point was behind the limit, then set it at the end
            if (m_selEnd > m_displayedText.getSize())
                setSelectionPointPosition(m_selEnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointPosition(unsigned int charactersBeforeSelectionPoint)
    {
        // The selection point position has to stay inside the string
        if (charactersBeforeSelectionPoint > m_text.getSize())
            charactersBeforeSelectionPoint = m_text.getSize();

        // Set the selection point to the correct position
        m_selChars = 0;
        m_selStart = charactersBeforeSelectionPoint;
        m_selEnd = charactersBeforeSelectionPoint;

        // Change our texts
        m_textBeforeSelection.setString(m_displayedText);
        m_textSelection.setString("");
        m_textAfterSelection.setString("");
        m_textFull.setString(m_displayedText);

        // Check if scrolling is enabled
        if (m_limitTextWidth == false)
        {
            // Calculate the space inside the edit box
            float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

            // Find out the position of the selection point
            float selectionPointPosition = m_textFull.findCharacterPos(m_selEnd).x;

            if (m_selEnd == m_displayedText.getSize())
                selectionPointPosition += m_textFull.getCharacterSize() / 10.f;

            // If the selection point is too far on the right then adjust the cropping
            if (m_textCropPosition + width < selectionPointPosition)
                m_textCropPosition = static_cast<unsigned int>(selectionPointPosition - width);

            // If the selection point is too far on the left then adjust the cropping
            if (m_textCropPosition > selectionPointPosition)
                m_textCropPosition = static_cast<unsigned int>(selectionPointPosition);
        }

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointWidth(unsigned int width)
    {
        m_selectionPoint.setPosition(m_selectionPoint.getPosition().x + ((m_selectionPoint.getSize().x - width) / 2.0f), m_selectionPoint.getPosition().y);
        m_selectionPoint.setSize(sf::Vector2f(static_cast<float>(width),
                                              m_textureNormal.getSize().y - ((m_bottomBorder + m_topBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y))));

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getSelectionPointWidth() const
    {
        return static_cast<unsigned int>(m_selectionPoint.getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setNumbersOnly(bool numbersOnly)
    {
        m_numbersOnly = numbersOnly;

        // Remove all letters from the edit box if needed
        if (numbersOnly && !m_text.isEmpty())
        {
            sf::String newText;
            bool commaFound = false;

            if ((m_text[0] == '+') || (m_text[0] == '-'))
                newText += m_text[0];

            for (unsigned int i = 0; i < m_text.getSize(); ++i)
            {
                if (!commaFound)
                {
                    if ((m_text[i] == ',') || (m_text[i] == '.'))
                    {
                        newText += m_text[i];
                        commaFound = true;
                    }
                }

                if ((m_text[i] >= '0') && (m_text[i] <= '9'))
                    newText += m_text[i];
            }

            // When the text changed then reposition the text
            if (newText != m_text)
                setText(newText);
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_textureNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureHover.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFocused.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(float x, float y)
    {
        // Calculate the space inside the edit box
        float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

        // Find the selection point position
        float positionX = x - getPosition().x - (m_leftBorder * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y));

        unsigned int selectionPointPosition = findSelectionPointPosition(positionX);

        // When clicking on the left of the first character, move the pointer to the left
        if ((positionX < 0) && (selectionPointPosition > 0))
            --selectionPointPosition;

        // When clicking on the right of the right character, move the pointer to the right
        else if ((positionX > width) && (selectionPointPosition < m_displayedText.getSize()))
            ++selectionPointPosition;

        // Check if this is a double click
        if ((m_possibleDoubleClick) && (m_selChars == 0) && (selectionPointPosition == m_selEnd))
        {
            // The next click is going to be a normal one again
            m_possibleDoubleClick = false;

            // Set the selection point at the end of the text
            setSelectionPointPosition(m_displayedText.getSize());

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
            // Set the new selection point
            setSelectionPointPosition(selectionPointPosition);

            // If the next click comes soon enough then it will be a double click
            m_possibleDoubleClick = true;
        }

        // Set the mouse down flag
        m_mouseDown = true;

        // Add the callback (if the user requested it)
        if (m_callbackFunctions[LeftMousePressed].empty() == false)
        {
            m_callback.trigger = LeftMousePressed;
            m_callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        recalculateTextPositions();

        // The selection point should be visible
        m_selectionPointVisible = true;
        m_animationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(float x, float)
    {
        if (m_mouseHover == false)
            mouseEnteredWidget();

        // Set the mouse hover flag
        m_mouseHover = true;

        // The mouse has moved so a double click is no longer possible
        m_possibleDoubleClick = false;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_mouseDown)
        {
            // Check if there is a text width limit
            if (m_limitTextWidth)
            {
                // Find out between which characters the mouse is standing
                m_selEnd = findSelectionPointPosition(x - getPosition().x - (m_leftBorder * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));
            }
            else // Scrolling is enabled
            {
                float scalingX = m_textureNormal.getSize().y / m_textureNormal.getImageSize().y;
                float width = m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * scalingX);

                // If the width is negative then the edit box is too small to be displayed
                if (width < 0)
                    width = 0;

                // Check if the mouse is on the left of the text
                if (x - getPosition().x < m_leftBorder * scalingX)
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
                else if ((x - getPosition().x > (m_leftBorder * scalingX) + width) && (m_textFull.findCharacterPos(m_displayedText.getSize()).x > width))
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
                m_selEnd = findSelectionPointPosition(x - getPosition().x - (m_leftBorder * scalingX));
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

    void EditBox::keyPressed(sf::Keyboard::Key key)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_loaded == false)
            return;

        // Check if one of the correct keys was pressed
        if (key == sf::Keyboard::Left)
        {
            // Check if we have selected some text
            if (m_selChars > 0)
            {
                // We will not move the selection point, but just undo the selection
                if (m_selStart < m_selEnd)
                    setSelectionPointPosition(m_selStart);
                else
                    setSelectionPointPosition(m_selEnd);
            }
            else // When we didn't select any text
            {
                // Move the selection point to the left
                if (m_selEnd > 0)
                    setSelectionPointPosition(m_selEnd - 1);
            }

            // Our selection point has moved, it should be visible
            m_selectionPointVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Right)
        {
            // Check if we have selected some text
            if (m_selChars > 0)
            {
                // We will not move the selection point, but just undo the selection
                if (m_selStart < m_selEnd)
                    setSelectionPointPosition(m_selEnd);
                else
                    setSelectionPointPosition(m_selStart);
            }
            else // When we didn't select any text
            {
                // Move the selection point to the right
                if (m_selEnd < m_displayedText.getSize())
                    setSelectionPointPosition(m_selEnd + 1);
            }

            // Our selection point has moved, it should be visible
            m_selectionPointVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Home)
        {
            // Set the selection point to the beginning of the text
            setSelectionPointPosition(0);

            // Our selection point has moved, it should be visible
            m_selectionPointVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::End)
        {
            // Set the selection point behind the text
            setSelectionPointPosition(m_text.getSize());

            // Our selection point has moved, it should be visible
            m_selectionPointVisible = true;
            m_animationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_callback.trigger = ReturnKeyPressed;
                m_callback.text    = m_text;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::BackSpace)
        {
            // Make sure that we didn't select any characters
            if (m_selChars == 0)
            {
                // We can't delete any characters when you are at the beginning of the string
                if (m_selEnd == 0)
                    return;

                // Erase the character
                m_text.erase(m_selEnd-1, 1);
                m_displayedText.erase(m_selEnd-1, 1);

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_selEnd - 1);

                // Calculate the space inside the edit box
                float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

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

            // The selection point should be visible again
            m_selectionPointVisible = true;
            m_animationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[TextChanged].empty() == false)
            {
                m_callback.trigger = TextChanged;
                m_callback.text    = m_text;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::Delete)
        {
            // Make sure that no text is selected
            if (m_selChars == 0)
            {
                // When the selection point is at the end of the line then you can't delete anything
                if (m_selEnd == m_text.getSize())
                    return;

                // Erase the character
                m_text.erase(m_selEnd, 1);
                m_displayedText.erase(m_selEnd, 1);

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_selEnd);

                // Calculate the space inside the edit box
                float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

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
            else // You did select some characters, delete them
                deleteSelectedCharacters();

            // The selection point should be visible again
            m_selectionPointVisible = true;
            m_animationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[TextChanged].empty() == false)
            {
                m_callback.trigger = TextChanged;
                m_callback.text    = m_text;
                addCallback();
            }
        }
        else
        {
            // Check if you are copying, pasting or cutting text
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
            {
                if (key == sf::Keyboard::C)
                {
                    TGUI_Clipboard.set(m_textSelection.getString());
                }
                else if (key == sf::Keyboard::V)
                {
                    auto clipboardContents = TGUI_Clipboard.get();

                    // Only continue pasting if you actually have to do something
                    if ((m_selChars > 0) || (clipboardContents.getSize() > 0))
                    {
                        deleteSelectedCharacters();

                        unsigned int oldCaretPos = m_selEnd;

                        if (m_text.getSize() > m_selEnd)
                            setText(m_text.toWideString().substr(0, m_selEnd) + TGUI_Clipboard.get() + m_text.toWideString().substr(m_selEnd, m_text.getSize() - m_selEnd));
                        else
                            setText(m_text + clipboardContents);

                        setSelectionPointPosition(oldCaretPos + clipboardContents.getSize());

                        // Add the callback (if the user requested it)
                        if (m_callbackFunctions[TextChanged].empty() == false)
                        {
                            m_callback.trigger = TextChanged;
                            m_callback.text    = m_text;
                            addCallback();
                        }
                    }
                }
                else if (key == sf::Keyboard::X)
                {
                    TGUI_Clipboard.set(m_textSelection.getString());
                    deleteSelectedCharacters();
                }
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::textEntered(sf::Uint32 key)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_loaded == false)
            return;

        // If only numbers are supported then make sure the input is valid
        if (m_numbersOnly)
        {
            if ((key < '0') || (key > '9'))
            {
                if ((key == '-') || (key == '+'))
                {
                    if ((m_selStart == 0) || (m_selEnd == 0))
                    {
                        if (!m_text.isEmpty())
                        {
                            // You can't have multiple + and - characters after each other
                            if ((m_text[0] == '-') || (m_text[0] == '+'))
                                return;
                        }
                    }
                    else // + and - symbols are only allowed at the beginning of the line
                        return;
                }
                else if ((key == ',') || (key == '.'))
                {
                    // Only one comma is allowed
                    for (auto it = m_text.begin(); it != m_text.end(); ++it)
                    {
                        if ((*it == ',') || (*it == '.'))
                            return;
                    }
                }
                else // Character not accepted
                    return;
            }
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

        // Calculate the space inside the edit box
        float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

        // When there is a text width limit then reverse what we just did
        if (m_limitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (m_textFull.findCharacterPos(m_displayedText.getSize()).x > width)
            {
                // If the text does not fit in the EditBox then delete the added character
                m_text.erase(m_selEnd, 1);
                m_displayedText.erase(m_selEnd, 1);
                return;
            }
        }

        // Move our selection point forward
        setSelectionPointPosition(m_selEnd + 1);

        // The selection point should be visible again
        m_selectionPointVisible = true;
        m_animationTimeElapsed = sf::Time();

        // Add the callback (if the user requested it)
        if (m_callbackFunctions[TextChanged].empty() == false)
        {
            m_callback.trigger = TextChanged;
            m_callback.text    = m_text;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_selChars)
            setSelectionPointPosition(m_selEnd);

        Widget::widgetUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "text")
        {
            setText(value);
        }
        else if (property == "textsize")
        {
            setTextSize(tgui::stoi(value));
        }
        else if (property == "passwordcharacter")
        {
            if (!value.empty())
            {
                if (value.length() == 1)
                    setPasswordCharacter(value[0]);
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'PasswordCharacter' propery.");
            }
            else
                setPasswordCharacter('\0');
        }
        else if (property == "maximumcharacters")
        {
            setMaximumCharacters(tgui::stoi(value));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders.left, borders.top, borders.right, borders.bottom);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "selectedtextcolor")
        {
            setSelectedTextColor(extractColor(value));
        }
        else if (property == "selectedtextbackgroundcolor")
        {
            setSelectedTextBackgroundColor(extractColor(value));
        }
        else if (property == "selectionpointcolor")
        {
            setSelectionPointColor(extractColor(value));
        }
        else if (property == "limittextwidth")
        {
            if ((value == "true") || (value == "True"))
                limitTextWidth(true);
            else if ((value == "false") || (value == "False"))
                limitTextWidth(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'LimitTextWidth' property.");
        }
        else if (property == "selectionpointwidth")
        {
            setSelectionPointWidth(tgui::stoi(value));
        }
        else if (property == "numbersonly")
        {
            if ((value == "true") || (value == "True"))
                setNumbersOnly(true);
            else if ((value == "false") || (value == "False"))
                setNumbersOnly(false);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'NumbersOnly' property.");
        }
        else if (property == "callback")
        {
            ClickableWidget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "TextChanged") || (*it == "textchanged"))
                    bindCallback(TextChanged);
                else if ((*it == "ReturnKeyPressed") || (*it == "returnkeypressed"))
                    bindCallback(ReturnKeyPressed);
            }
        }
        else // The property didn't match
            return ClickableWidget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "text")
            value = getText().toAnsiString();
        else if (property == "textsize")
            value = tgui::to_string(getTextSize());
        else if (property == "passwordcharacter")
        {
            if (getPasswordCharacter())
                value = getPasswordCharacter();
            else
                value = "";
        }
        else if (property == "maximumcharacters")
            value = tgui::to_string(getMaximumCharacters());
        else if (property == "borders")
            value = "(" + tgui::to_string(getBorders().left) + "," + tgui::to_string(getBorders().top) + "," + tgui::to_string(getBorders().right) + "," + tgui::to_string(getBorders().bottom) + ")";
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + tgui::to_string(int(getSelectedTextColor().r)) + "," + tgui::to_string(int(getSelectedTextColor().g))
                    + "," + tgui::to_string(int(getSelectedTextColor().b)) + "," + tgui::to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "selectedtextbackgroundcolor")
            value = "(" + tgui::to_string(int(getSelectedTextBackgroundColor().r)) + "," + tgui::to_string(int(getSelectedTextBackgroundColor().g))
                    + "," + tgui::to_string(int(getSelectedTextBackgroundColor().b)) + "," + tgui::to_string(int(getSelectedTextBackgroundColor().a)) + ")";
        else if (property == "selectionpointcolor")
            value = "(" + tgui::to_string(int(getSelectionPointColor().r)) + "," + tgui::to_string(int(getSelectionPointColor().g))
                    + "," + tgui::to_string(int(getSelectionPointColor().b)) + "," + tgui::to_string(int(getSelectionPointColor().a)) + ")";
        else if (property == "limittextwidth")
            value = m_limitTextWidth ? "true" : "false";
        else if (property == "selectionpointwidth")
            value = tgui::to_string(getSelectionPointWidth());
        else if (property == "numbersonly")
            value = m_numbersOnly ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(TextChanged) != m_callbackFunctions.end()) && (m_callbackFunctions.at(TextChanged).size() == 1) && (m_callbackFunctions.at(TextChanged).front() == nullptr))
                callbacks.push_back("TextChanged");
            if ((m_callbackFunctions.find(ReturnKeyPressed) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ReturnKeyPressed).size() == 1) && (m_callbackFunctions.at(ReturnKeyPressed).front() == nullptr))
                callbacks.push_back("ReturnKeyPressed");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            return ClickableWidget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > EditBox::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("Text", "string"));
        list.push_back(std::pair<std::string, std::string>("TextSize", "uint"));
        list.push_back(std::pair<std::string, std::string>("PasswordCharacter", "char"));
        list.push_back(std::pair<std::string, std::string>("MaximumCharacters", "uint"));
        list.push_back(std::pair<std::string, std::string>("Borders", "borders"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextBackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectionPointColor", "color"));
        list.push_back(std::pair<std::string, std::string>("LimitTextWidth", "bool"));
        list.push_back(std::pair<std::string, std::string>("SelectionPointWidth", "uint"));
        list.push_back(std::pair<std::string, std::string>("NumbersOnly", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::findSelectionPointPosition(float posX)
    {
        // This code will crash when the editbox is empty. We need to avoid this.
        if (m_displayedText.isEmpty())
            return 0;

        // Find out what the first visible character is
        unsigned int firstVisibleChar;
        if (m_textCropPosition)
        {
            // Start searching near the selection point to quickly find the character even in a very long string
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
        unsigned int lastVisibleChar;

        // Calculate the space inside the edit box
        float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

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

        // Find out what the last visible character is, starting from the selection point
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
        for (unsigned int i = firstVisibleChar; i < lastVisibleChar; ++i)
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

            // Set the selection point back on the correct position
            setSelectionPointPosition(m_selStart);
        }
        else // When the text is selected from right to left
        {
            // Erase the characters
            m_text.erase(m_selEnd, m_selChars);
            m_displayedText.erase(m_selEnd, m_selChars);

            // Set the selection point back on the correct position
            setSelectionPointPosition(m_selEnd);
        }

        // Calculate the space inside the edit box
        float width = std::max(0.f, m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * (m_textureNormal.getSize().y / m_textureNormal.getImageSize().y)));

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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::recalculateTextPositions()
    {
        float textX = getPosition().x;
        float textY = getPosition().y;

        // Calculate the scaling
        sf::Vector2f scaling(m_textureNormal.getSize().x / m_textureNormal.getImageSize().x, m_textureNormal.getSize().y / m_textureNormal.getImageSize().y);

        // Calculate the scale of the left and right border
        float borderScale = scaling.y;

        textX += m_leftBorder * borderScale - m_textCropPosition;
        textY += m_topBorder * scaling.y;

        // Check if the layout wasn't left
        if (m_textAlignment != Alignment::Left)
        {
            // Calculate the space inside the edit box
            float width = m_textureNormal.getSize().x - ((m_leftBorder + m_rightBorder) * borderScale);

            // Calculate the text width
            float textWidth = m_textFull.findCharacterPos(m_displayedText.getSize()).x;

            // Check if a layout would make sense
            if (textWidth < width)
            {
                // Put the text on the correct position
                if (m_textAlignment == Alignment::Center)
                    textX += (width - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    textX += width - textWidth;
            }
        }

        float selectionPointLeft = textX;

        // Set the position of the text
        sf::Text tempText(m_textFull);
        tempText.setString("kg");
        textY += (((m_textureNormal.getSize().y - ((m_topBorder + m_bottomBorder) * scaling.y)) - tempText.getLocalBounds().height) * 0.5f) - tempText.getLocalBounds().top;

        // Set the text before the selection on the correct position
        m_textBeforeSelection.setPosition(std::floor(textX + 0.5f), std::floor(textY + 0.5f));

        // Check if there is a selection
        if (m_selChars != 0)
        {
            // Watch out for the kerning
            if (m_textBeforeSelection.getString().getSize() > 0)
                textX += m_textBeforeSelection.getFont()->getKerning(m_displayedText[m_textBeforeSelection.getString().getSize() - 1], m_displayedText[m_textBeforeSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            textX += m_textBeforeSelection.findCharacterPos(m_textBeforeSelection.getString().getSize()).x - m_textBeforeSelection.getPosition().x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_selectedTextBackground.setSize(sf::Vector2f(m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x - m_textSelection.getPosition().x,
                                                          (m_textureNormal.getSize().y - ((m_topBorder + m_bottomBorder) * scaling.y))));
            m_selectedTextBackground.setPosition(std::floor(textX + 0.5f), std::floor(getPosition().y + (m_topBorder * scaling.y) + 0.5f));

            // Set the text selected text on the correct position
            m_textSelection.setPosition(std::floor(textX + 0.5f), std::floor(textY + 0.5f));

            // Watch out for kerning
            if (m_displayedText.getSize() > m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize())
                textX += m_textBeforeSelection.getFont()->getKerning(m_displayedText[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize() - 1], m_displayedText[m_textBeforeSelection.getString().getSize() + m_textSelection.getString().getSize()], m_textBeforeSelection.getCharacterSize());

            // Set the text selected text on the correct position
            textX += m_textSelection.findCharacterPos(m_textSelection.getString().getSize()).x  - m_textSelection.getPosition().x;
            m_textAfterSelection.setPosition(std::floor(textX + 0.5f), std::floor(textY + 0.5f));
        }

        // Set the position of the selection point
        selectionPointLeft += m_textFull.findCharacterPos(m_selEnd).x - (m_selectionPoint.getSize().x * 0.5f);
        m_selectionPoint.setPosition(std::floor(selectionPointLeft + 0.5f), std::floor((m_topBorder * scaling.y) + getPosition().y + 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::initialize(Container *const parent)
    {
        m_parent = parent;
        setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::update()
    {
        // Only show/hide the selection point every half second
        if (m_animationTimeElapsed < sf::milliseconds(500))
            return;

        // Reset the elapsed time
        m_animationTimeElapsed = sf::Time();

        // Only update when the editbox is visible
        if (m_visible == false)
            return;

        // Switch the value of the visible flag
        m_selectionPointVisible = !m_selectionPointVisible;

        // Too slow for double clicking
        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the edit box was not loaded correctly
        if (m_loaded == false)
            return;

        if (m_separateHoverImage)
        {
            if (m_mouseHover && m_textureHover.getData())
                target.draw(m_textureHover, states);
            else
                target.draw(m_textureNormal, states);
        }
        else // The hover image is drawn on top of the normal one
        {
            target.draw(m_textureNormal, states);

            // When the mouse is on top of the edit box then draw an extra image
            if (m_mouseHover && m_textureHover.getData())
                target.draw(m_textureHover, states);
        }

        // When the edit box is focused then draw an extra image
        if (m_focused && m_textureFocused.getData())
            target.draw(m_textureFocused, states);

        // Calculate the scaling
        sf::Vector2f scaling(m_textureNormal.getSize().x / m_textureNormal.getImageSize().x, m_textureNormal.getSize().y / m_textureNormal.getImageSize().y);

        // Calculate the scale of the left and right border
        float borderScale = scaling.y;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        sf::Vector2f viewPosition = (target.getView().getSize() / 2.f) - target.getView().getCenter();

        // Get the global position
        sf::Vector2f topLeftPosition = states.transform.transformPoint(getPosition().x + (m_leftBorder * borderScale) + viewPosition.x,
                                                                       getPosition().y + (m_topBorder * scaling.y) + viewPosition.y);
        sf::Vector2f bottomRightPosition = states.transform.transformPoint(getPosition().x + (m_textureNormal.getSize().x - (m_rightBorder * borderScale)) + viewPosition.x,
                                                                           getPosition().y + (m_textureNormal.getSize().y - (m_bottomBorder * scaling.y)) + viewPosition.y);

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        target.draw(m_textBeforeSelection, states);

        if (m_textSelection.getString().isEmpty() == false)
        {
            target.draw(m_selectedTextBackground, states);

            target.draw(m_textSelection, states);
            target.draw(m_textAfterSelection, states);
        }

        // Draw the selection point
        if ((m_focused) && (m_selectionPointVisible))
            target.draw(m_selectionPoint, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
