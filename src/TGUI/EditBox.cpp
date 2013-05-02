/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/ClickableObject.hpp>
#include <TGUI/EditBox.hpp>

#include <SFML/OpenGL.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox() :
    m_SelectionPointVisible (true),
    m_SelectionPointWidth   (1),
    m_LimitTextWidth        (false),
    m_DisplayedText         (""),
    m_Text                  (""),
    m_TextSize              (0),
    m_TextAlignment         (Alignment::Left),
    m_SelChars              (0),
    m_SelStart              (0),
    m_SelEnd                (0),
    m_PasswordChar          ('\0'),
    m_MaxChars              (0),
    m_SplitImage            (false),
    m_TextCropPosition      (0),
    m_PossibleDoubleClick   (false),
    m_NumbersOnly           (false)
    {
        m_Callback.objectType = Type_EditBox;
        m_AnimatedObject = true;
        m_DraggableObject = true;

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox(const EditBox& copy) :
    ClickableObject         (copy),
    ObjectBorders           (copy),
    m_SelectionPointVisible (copy.m_SelectionPointVisible),
    m_SelectionPointColor   (copy.m_SelectionPointColor),
    m_SelectionPointWidth   (copy.m_SelectionPointWidth),
    m_LimitTextWidth        (copy.m_LimitTextWidth),
    m_DisplayedText         (copy.m_DisplayedText),
    m_Text                  (copy.m_Text),
    m_TextSize              (copy.m_TextSize),
    m_TextAlignment         (copy.m_TextAlignment),
    m_SelChars              (copy.m_SelChars),
    m_SelStart              (copy.m_SelStart),
    m_SelEnd                (copy.m_SelEnd),
    m_PasswordChar          (copy.m_PasswordChar),
    m_MaxChars              (copy.m_MaxChars),
    m_SplitImage            (copy.m_SplitImage),
    m_TextCropPosition      (copy.m_TextCropPosition),
    m_SelectedTextBgrColor  (copy.m_SelectedTextBgrColor),
    m_TextBeforeSelection   (copy.m_TextBeforeSelection),
    m_TextSelection         (copy.m_TextSelection),
    m_TextAfterSelection    (copy.m_TextAfterSelection),
    m_TextFull              (copy.m_TextFull),
    m_PossibleDoubleClick   (copy.m_PossibleDoubleClick),
    m_NumbersOnly           (copy.m_NumbersOnly)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover_L, m_TextureHover_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover_M, m_TextureHover_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureHover_R, m_TextureHover_R);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused_L, m_TextureFocused_L);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused_M, m_TextureFocused_M);
        TGUI_TextureManager.copyTexture(copy.m_TextureFocused_R, m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::~EditBox()
    {
        if (m_TextureNormal_L.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureHover_L.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureHover_R);

        if (m_TextureFocused_L.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox& EditBox::operator= (const EditBox& right)
    {
        if (this != &right)
        {
            EditBox temp(right);
            this->ClickableObject::operator=(right);
            this->ObjectBorders::operator=(right);

            std::swap(m_SelectionPointVisible,  temp.m_SelectionPointVisible);
            std::swap(m_SelectionPointColor,    temp.m_SelectionPointColor);
            std::swap(m_SelectionPointWidth,    temp.m_SelectionPointWidth);
            std::swap(m_LimitTextWidth,         temp.m_LimitTextWidth);
            std::swap(m_DisplayedText,          temp.m_DisplayedText);
            std::swap(m_Text,                   temp.m_Text);
            std::swap(m_TextSize,               temp.m_TextSize);
            std::swap(m_TextAlignment,          temp.m_TextAlignment);
            std::swap(m_SelChars,               temp.m_SelChars);
            std::swap(m_SelStart,               temp.m_SelStart);
            std::swap(m_SelEnd,                 temp.m_SelEnd);
            std::swap(m_PasswordChar,           temp.m_PasswordChar);
            std::swap(m_MaxChars,               temp.m_MaxChars);
            std::swap(m_SplitImage,             temp.m_SplitImage);
            std::swap(m_TextCropPosition,       temp.m_TextCropPosition);
            std::swap(m_SelectedTextBgrColor,   temp.m_SelectedTextBgrColor);
            std::swap(m_TextBeforeSelection,    temp.m_TextBeforeSelection);
            std::swap(m_TextSelection,          temp.m_TextSelection);
            std::swap(m_TextAfterSelection,     temp.m_TextAfterSelection);
            std::swap(m_TextFull,               temp.m_TextFull);
            std::swap(m_TextureNormal_L,        temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,        temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,        temp.m_TextureNormal_R);
            std::swap(m_TextureHover_L,         temp.m_TextureHover_L);
            std::swap(m_TextureHover_M,         temp.m_TextureHover_M);
            std::swap(m_TextureHover_R,         temp.m_TextureHover_R);
            std::swap(m_TextureFocused_L,       temp.m_TextureFocused_L);
            std::swap(m_TextureFocused_M,       temp.m_TextureFocused_M);
            std::swap(m_TextureFocused_R,       temp.m_TextureFocused_R);
            std::swap(m_PossibleDoubleClick,    temp.m_PossibleDoubleClick);
            std::swap(m_NumbersOnly,            temp.m_NumbersOnly);
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
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;
        m_Size.x = 0;
        m_Size.y = 0;

        // Remove the textures when they were loaded before
        if (m_TextureNormal_L.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureHover_L.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_R);
        if (m_TextureFocused_L.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureFocused_R);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("EditBox", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + configFileFilename + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "textcolor")
            {
                sf::Color color = extractColor(value);
                m_TextBeforeSelection.setColor(color);
                m_TextAfterSelection.setColor(color);
            }
            else if (property == "selectedtextcolor")
            {
                m_TextSelection.setColor(extractColor(value));
            }
            else if (property == "selectedtextbackgroundcolor")
            {
                m_SelectedTextBgrColor = extractColor(value);
            }
            else if (property == "selectionpointcolor")
            {
                m_SelectionPointColor = extractColor(value);
            }
            else if (property == "selectionpointwidth")
            {
                m_SelectionPointWidth = static_cast<unsigned int>(abs(atoi(value.c_str())));
            }
            else if (property.compare("borders") == 0)
            {
                Vector4u borders;
                if (extractVector4u(value, borders))
                    setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section EditBox in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section EditBox in " + configFileFilename + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_L in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_M in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_R in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_L in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_M in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_R in section EditBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI error: Unrecognized property '" + property + "' in section EditBox in " + configFileFilename + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures were loaded
            if ((m_TextureNormal_L.data != NULL) && (m_TextureNormal_M.data != NULL) && (m_TextureNormal_R.data != NULL))
            {
                // Set the size of the edit box
                m_Size.x = static_cast<float>(m_TextureNormal_L.getSize().x + m_TextureNormal_M.getSize().x + m_TextureNormal_R.getSize().x);
                m_Size.y = static_cast<float>(m_TextureNormal_M.getSize().y);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the edit box. Is the EditBox section in " + configFileFilename + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureFocused_L.data != NULL) && (m_TextureFocused_M.data != NULL) && (m_TextureFocused_R.data != NULL))
            {
                m_AllowFocus = true;
                m_ObjectPhase |= ObjectPhase_Focused;
            }
            if ((m_TextureHover_L.data != NULL) && (m_TextureHover_M.data != NULL) && (m_TextureHover_R.data != NULL))
            {
                m_ObjectPhase |= ObjectPhase_Hover;
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_TextureNormal_M.data != NULL)
            {
                m_Size = Vector2f(m_TextureNormal_M.getSize());
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the EditBox section in " + configFileFilename + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_TextureFocused_M.data != NULL)
            {
                m_AllowFocus = true;
                m_ObjectPhase |= ObjectPhase_Focused;
            }
            if (m_TextureHover_M.data != NULL)
            {
                m_ObjectPhase |= ObjectPhase_Hover;
            }
        }

        // Auto-size the text
        setTextSize(0);

        // When there is no error we will return true
        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(float width, float height)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Store the new size
        m_Size.x = width;
        m_Size.y = height;

        // A negative size is not allowed for this object
        if (m_Size.x  < 0) m_Size.x  = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;

        // When using splitimage, make sure that the width isn't too small
        if (m_SplitImage)
        {
            if ((m_Size.y / m_TextureNormal_M.getSize().y) * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) > m_Size.x)
                m_Size.x = (m_Size.y / m_TextureNormal_M.getSize().y) * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x);
        }

        // Recalculate the text size when auto scaling
        if (m_TextSize == 0)
            setText(m_Text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const sf::String& text)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate the text size
            m_TextFull.setString("kg");
            m_TextFull.setCharacterSize(static_cast<unsigned int>(m_Size.y - ((m_TopBorder + m_BottomBorder) * (m_Size.y / m_TextureNormal_M.getSize().y))));
            m_TextFull.setCharacterSize(static_cast<unsigned int>(m_TextFull.getCharacterSize() - m_TextFull.getLocalBounds().top));
            m_TextFull.setString(m_DisplayedText);

            // Also adjust the character size of the other texts
            m_TextBeforeSelection.setCharacterSize(m_TextFull.getCharacterSize());
            m_TextSelection.setCharacterSize(m_TextFull.getCharacterSize());
            m_TextAfterSelection.setCharacterSize(m_TextFull.getCharacterSize());
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_TextBeforeSelection.setCharacterSize(m_TextSize);
            m_TextSelection.setCharacterSize(m_TextSize);
            m_TextAfterSelection.setCharacterSize(m_TextSize);
            m_TextFull.setCharacterSize(m_TextSize);
        }

        // Change the text
        m_Text = text;
        m_DisplayedText = text;

        // If there is a character limit then check if it is exeeded
        if ((m_MaxChars > 0) && (m_DisplayedText.getSize() > m_MaxChars))
        {
            // Remove all the excess characters
            m_Text.erase(m_MaxChars, sf::String::InvalidPos);
            m_DisplayedText.erase(m_MaxChars, sf::String::InvalidPos);
        }

        // Check if there is a password character
        if (m_PasswordChar != '\0')
        {
            // Loop every character and change it
            for (unsigned int i=0; i < m_Text.getSize(); ++i)
                m_DisplayedText[i] = m_PasswordChar;
        }

        // Set the texts
        m_TextBeforeSelection.setString(m_DisplayedText);
        m_TextSelection.setString("");
        m_TextAfterSelection.setString("");
        m_TextFull.setString(m_DisplayedText);

        // Calculate the space inside the edit box
        float width;
        if (m_SplitImage)
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
        else
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

        // If the width is negative then the editBox is too small to be displayed
        if (width < 0)
            width = 0;

        // Check if there is a text width limit
        if (m_LimitTextWidth)
        {
            // Now check if the text fits into the EditBox
            while (m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x > width)
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                m_Text.erase(m_Text.getSize()-1);
                m_DisplayedText.erase(m_DisplayedText.getSize()-1);

                // Set the new text
                m_TextBeforeSelection.setString(m_DisplayedText);
            }

            // Set the full text again
            m_TextFull.setString(m_DisplayedText);
        }
        else // There is no text cropping
        {
            // Calculate the text width
            float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

            // If the text can be moved to the right then do so
            if (textWidth > width)
            {
                if (textWidth - m_TextCropPosition < width)
                    m_TextCropPosition = static_cast<unsigned int>(textWidth - width);
            }
            else
                m_TextCropPosition = 0;
        }

        // Set the selection point behind the last character
        setSelectionPointPosition(m_DisplayedText.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String EditBox::getText() const
    {
        return m_Text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getTextSize() const
    {
        return m_TextFull.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextFont(const sf::Font& font)
    {
        m_TextBeforeSelection.setFont(font);
        m_TextSelection.setFont(font);
        m_TextAfterSelection.setFont(font);
        m_TextFull.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* EditBox::getTextFont() const
    {
        return m_TextFull.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordChar(char passwordChar)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Change the password character
        m_PasswordChar = passwordChar;

        // Recalculate the text position
        setText(m_Text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char EditBox::getPasswordChar() const
    {
        return m_PasswordChar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setMaximumCharacters(unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_MaxChars = maxChars;

        // If there is a character limit then check if it is exeeded
        if ((m_MaxChars > 0) && (m_DisplayedText.getSize() > m_MaxChars))
        {
            // Remove all the excess characters
            m_Text.erase(m_MaxChars, sf::String::InvalidPos);
            m_DisplayedText.erase(m_MaxChars, sf::String::InvalidPos);

            // If we passed here then the text has changed.
            m_TextBeforeSelection.setString(m_DisplayedText);
            m_TextSelection.setString("");
            m_TextAfterSelection.setString("");
            m_TextFull.setString(m_DisplayedText);

            // Set the selection point behind the last character
            setSelectionPointPosition(m_DisplayedText.getSize());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getMaximumCharacters() const
    {
        return m_MaxChars;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setBorders(unsigned int borderLeft, unsigned int borderTop, unsigned int borderRight, unsigned int borderBottom)
    {
        // Set the new border size
        m_LeftBorder   = borderLeft;
        m_TopBorder    = borderTop;
        m_RightBorder  = borderRight;
        m_BottomBorder = borderBottom;

        // Recalculate the text size
        setText(m_Text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::changeColors(const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& newSelectionPointColor)
    {
        m_TextBeforeSelection.setColor(color);
        m_TextSelection.setColor(selectedColor);
        m_TextAfterSelection.setColor(color);

        m_SelectionPointColor = newSelectionPointColor;
        m_SelectedTextBgrColor = selectedBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextColor(const sf::Color& textColor)
    {
        m_TextBeforeSelection.setColor(textColor);
        m_TextAfterSelection.setColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_TextSelection.setColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectedTextBackgroundColor(const sf::Color& selectedTextBackgroundColor)
    {
        m_SelectedTextBgrColor = selectedTextBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointColor(const sf::Color& selectionPointColor)
    {
        m_SelectionPointColor = selectionPointColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getTextColor() const
    {
        return m_TextBeforeSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectedTextColor() const
    {
        return m_TextSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectedTextBackgroundColor() const
    {
        return m_SelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectionPointColor() const
    {
        return m_SelectionPointColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::limitTextWidth(bool limitWidth)
    {
        m_LimitTextWidth = limitWidth;

        // Check if the width is being limited
        if (m_LimitTextWidth == true)
        {
            // Calculate the space inside the edit box
            float width;
            if (m_SplitImage)
                width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
            else
                width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

            // If the width is negative then the editBox is too small to be displayed
            if (width < 0)
                width = 0;

            // Now check if the text fits into the EditBox
            while (m_TextBeforeSelection.findCharacterPos(m_DisplayedText.getSize()).x > width)
            {
                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                m_Text.erase(m_Text.getSize()-1);
                m_DisplayedText.erase(m_DisplayedText.getSize()-1);
                m_TextBeforeSelection.setString(m_DisplayedText);
            }

            // The full text might have changed
            m_TextFull.setString(m_DisplayedText);

            // There is no clipping
            m_TextCropPosition = 0;

            // If the selection point was behind the limit, then set it at the end
            if (m_SelEnd > m_DisplayedText.getSize())
                setSelectionPointPosition(m_SelEnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointPosition(unsigned int charactersBeforeSelectionPoint)
    {
        // The selection point position has to stay inside the string
        if (charactersBeforeSelectionPoint > m_Text.getSize())
            charactersBeforeSelectionPoint = m_Text.getSize();

        // Set the selection point to the correct position
        m_SelChars = 0;
        m_SelStart = charactersBeforeSelectionPoint;
        m_SelEnd = charactersBeforeSelectionPoint;

        // Change our texts
        m_TextBeforeSelection.setString(m_DisplayedText);
        m_TextSelection.setString("");
        m_TextAfterSelection.setString("");
        m_TextFull.setString(m_DisplayedText);

        // Check if scrolling is enabled
        if (m_LimitTextWidth == false)
        {
            // Calculate the space inside the edit box
            float width;
            if (m_SplitImage)
                width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
            else
                width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

            // If the width is negative then the editBox is too small to be displayed
            if (width < 0)
                width = 0;

            // Find out the position of the selection point
            float selectionPointPosition = m_TextFull.findCharacterPos(m_SelEnd).x;

            if (m_SelEnd == m_DisplayedText.getSize())
                selectionPointPosition += m_TextFull.getCharacterSize() / 10.f;

            // If the selection point is too far on the right then adjust the cropping
            if (m_TextCropPosition + width < selectionPointPosition)
                m_TextCropPosition = static_cast<unsigned int>(selectionPointPosition - width);

            // If the selection point is too far on the left then adjust the cropping
            if (m_TextCropPosition > selectionPointPosition)
                m_TextCropPosition = static_cast<unsigned int>(selectionPointPosition);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointWidth(unsigned int width)
    {
        m_SelectionPointWidth = width;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getSelectionPointWidth() const
    {
        return m_SelectionPointWidth;
    }

    void EditBox::setNumbersOnly(bool numbersOnly)
    {
        m_NumbersOnly = numbersOnly;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(float x, float y)
    {
        // Calculate the space inside the edit box
        float width;
        if (m_SplitImage)
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
        else
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

        // If the width is negative then the editBox is too small to be displayed
        if (width < 0)
            width = 0;

        // Find the selection point position
        float positionX;
        if (m_SplitImage)
            positionX = x - getPosition().x - (m_LeftBorder * (m_Size.y / m_TextureNormal_M.getSize().y));
        else
            positionX = x - getPosition().x - (m_LeftBorder * (m_Size.x / m_TextureNormal_M.getSize().x));

        unsigned int selectionPointPosition = findSelectionPointPosition(positionX);

        // When clicking on the left of the first character, move the pointer to the left
        if ((positionX < 0) && (selectionPointPosition > 0))
            --selectionPointPosition;

        // When clicking on the right of the right character, move the pointer to the right
        else if ((positionX > width) && (selectionPointPosition < m_DisplayedText.getSize()))
            ++selectionPointPosition;

        // Check if this is a double click
        if ((m_PossibleDoubleClick) && (m_SelChars == 0) && (selectionPointPosition == m_SelEnd))
        {
            // The next click is going to be a normal one again
            m_PossibleDoubleClick = false;

            // Set the selection point at the end of the text
            setSelectionPointPosition(m_DisplayedText.getSize());

            // Select the whole text
            m_SelStart = 0;
            m_SelEnd = m_Text.getSize();
            m_SelChars = m_Text.getSize();

            // Change the texts
            m_TextBeforeSelection.setString("");
            m_TextSelection.setString(m_DisplayedText);
            m_TextAfterSelection.setString("");
        }
        else // No double clicking
        {
            // Set the new selection point
            setSelectionPointPosition(selectionPointPosition);

            // If the next click comes soon enough then it will be a double click
            m_PossibleDoubleClick = true;
        }

        // Set the mouse down flag
        m_MouseDown = true;

        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[LeftMousePressed].empty() == false)
        {
            m_Callback.trigger = LeftMousePressed;
            m_Callback.mouse.x = x - getPosition().x;
            m_Callback.mouse.y = y - getPosition().y;
            addCallback();
        }

        // The selection point should be visible
        m_SelectionPointVisible = true;
        m_AnimationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(float x, float)
    {
        if (m_MouseHover == false)
            mouseEnteredObject();

        // Set the mouse hover flag
        m_MouseHover = true;

        // The mouse has moved so a double click is no longer possible
        m_PossibleDoubleClick = false;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_MouseDown)
        {
            // Check if there is a text width limit
            if (m_LimitTextWidth)
            {
                // Find out between which characters the mouse is standing
                if (m_SplitImage)
                    m_SelEnd = findSelectionPointPosition(x - getPosition().x - (m_LeftBorder * (m_Size.y / m_TextureNormal_M.getSize().y)));
                else
                    m_SelEnd = findSelectionPointPosition(x - getPosition().x - (m_LeftBorder * (m_Size.x / m_TextureNormal_M.getSize().x)));
            }
            else // Scrolling is enabled
            {
                float width;
                float scalingX;

                if (m_SplitImage)
                {
                    scalingX = m_Size.y / m_TextureNormal_M.getSize().y;
                    width = m_Size.x - ((m_LeftBorder + m_RightBorder) * scalingX);
                }
                else
                {
                    scalingX = m_Size.x / m_TextureNormal_M.getSize().x;
                    width = m_Size.x - ((m_LeftBorder + m_RightBorder) * scalingX);
                }

                // If the width is negative then the edit box is too small to be displayed
                if (width < 0)
                    width = 0;

                // Check if the mouse is on the left of the text
                if (x - getPosition().x < m_LeftBorder * scalingX * getScale().x)
                {
                    // Move the text by a few pixels
                    if (m_TextFull.getCharacterSize() > 10)
                    {
                        if (m_TextCropPosition > m_TextFull.getCharacterSize() / 10)
                            m_TextCropPosition -= static_cast<unsigned int>(std::floor(m_TextFull.getCharacterSize() / 10.f + 0.5f));
                        else
                            m_TextCropPosition = 0;
                    }
                    else
                    {
                        if (m_TextCropPosition)
                            --m_TextCropPosition;
                    }
                }
                // Check if the mouse is on the right of the text AND there is a possibility to scroll
                else if ((x - getPosition().x > ((m_LeftBorder * scalingX) + width) * getScale().x) && (m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x > width))
                {
                    // Move the text by a few pixels
                    if (m_TextFull.getCharacterSize() > 10)
                    {
                        if (m_TextCropPosition + width < m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x + (m_TextFull.getCharacterSize() / 10))
                            m_TextCropPosition += static_cast<unsigned int>(std::floor(m_TextFull.getCharacterSize() / 10.f + 0.5f));
                        else
                            m_TextCropPosition = static_cast<unsigned int>(m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x + (m_TextFull.getCharacterSize() / 10) - width);
                    }
                    else
                    {
                        if (m_TextCropPosition + width < m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x)
                            ++m_TextCropPosition;
                    }
                }

                // Find out between which characters the mouse is standing
                m_SelEnd = findSelectionPointPosition(x - getPosition().x - (m_LeftBorder * scalingX));
            }

            // Check if we are selecting text from left to right
            if (m_SelEnd > m_SelStart)
            {
                // There is no need to redo everything when nothing changed
                if (m_SelChars != (m_SelEnd - m_SelStart))
                {
                    // Adjust the number of characters that are selected
                    m_SelChars = m_SelEnd - m_SelStart;

                    // Change our three texts
                    m_TextBeforeSelection.setString(m_DisplayedText.toWideString().substr(0, m_SelStart));
                    m_TextSelection.setString(m_DisplayedText.toWideString().substr(m_SelStart, m_SelChars));
                    m_TextAfterSelection.setString(m_DisplayedText.toWideString().substr(m_SelEnd));
                }
            }
            else if (m_SelEnd < m_SelStart)
            {
                // There is no need to redo everything when nothing changed
                if (m_SelChars != (m_SelStart - m_SelEnd))
                {
                    // Adjust the number of characters that are selected
                    m_SelChars = m_SelStart - m_SelEnd;

                    // Change our three texts
                    m_TextBeforeSelection.setString(m_DisplayedText.toWideString().substr(0, m_SelEnd));
                    m_TextSelection.setString(m_DisplayedText.toWideString().substr(m_SelEnd, m_SelChars));
                    m_TextAfterSelection.setString(m_DisplayedText.toWideString().substr(m_SelStart));
                }
            }
            else
            {
                // Adjust the number of characters that are selected
                m_SelChars = 0;

                // Change our three texts
                m_TextBeforeSelection.setString(m_DisplayedText);
                m_TextSelection.setString("");
                m_TextAfterSelection.setString("");
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(sf::Keyboard::Key key)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if one of the correct keys was pressed
        if (key == sf::Keyboard::Left)
        {
            // Check if we have selected some text
            if (m_SelChars > 0)
            {
                // We will not move the selection point, but just undo the selection
                if (m_SelStart < m_SelEnd)
                    setSelectionPointPosition(m_SelStart);
                else
                    setSelectionPointPosition(m_SelEnd);
            }
            else // When we didn't select any text
            {
                // Move the selection point to the left
                if (m_SelEnd > 0)
                    setSelectionPointPosition(m_SelEnd - 1);
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Right)
        {
            // Check if we have selected some text
            if (m_SelChars > 0)
            {
                // We will not move the selection point, but just undo the selection
                if (m_SelStart < m_SelEnd)
                    setSelectionPointPosition(m_SelEnd);
                else
                    setSelectionPointPosition(m_SelStart);
            }
            else // When we didn't select any text
            {
                // Move the selection point to the right
                if (m_SelEnd < m_DisplayedText.getSize())
                    setSelectionPointPosition(m_SelEnd + 1);
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Home)
        {
            // Set the selection point to the beginning of the text
            setSelectionPointPosition(0);

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::End)
        {
            // Set the selection point behind the text
            setSelectionPointPosition(m_Text.getSize());

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_Callback.trigger = ReturnKeyPressed;
                m_Callback.text    = m_Text;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::BackSpace)
        {
            // Make sure that we didn't select any characters
            if (m_SelChars == 0)
            {
                // We can't delete any characters when you are at the beginning of the string
                if (m_SelEnd == 0)
                    return;

                // Erase the character
                m_Text.erase(m_SelEnd-1, 1);
                m_DisplayedText.erase(m_SelEnd-1, 1);

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelEnd - 1);

                // Calculate the space inside the edit box
                float width;
                if (m_SplitImage)
                    width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
                else
                    width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

                // If the width is negative then the edit box is too small to be displayed
                if (width < 0)
                    width = 0;

                // Calculate the text width
                float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

                // If the text can be moved to the right then do so
                if (textWidth > width)
                {
                    if (textWidth - m_TextCropPosition < width)
                        m_TextCropPosition = static_cast<unsigned int>(textWidth - width);
                }
                else
                    m_TextCropPosition = 0;
            }
            else // When you did select some characters, delete them
                deleteSelectedCharacters();

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[TextChanged].empty() == false)
            {
                m_Callback.trigger = TextChanged;
                m_Callback.text    = m_Text;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::Delete)
        {
            // Make sure that no text is selected
            if (m_SelChars == 0)
            {
                // When the selection point is at the end of the line then you can't delete anything
                if (m_SelEnd == m_Text.getSize())
                    return;

                // Erase the character
                m_Text.erase(m_SelEnd, 1);
                m_DisplayedText.erase(m_SelEnd, 1);

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelEnd);

                // Calculate the space inside the edit box
                float width;
                if (m_SplitImage)
                    width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
                else
                    width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

                // If the width is negative then the edit box is too small to be displayed
                if (width < 0)
                    width = 0;

                // Calculate the text width
                float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

                // If the text can be moved to the right then do so
                if (textWidth > width)
                {
                    if (textWidth - m_TextCropPosition < width)
                        m_TextCropPosition = static_cast<unsigned int>(textWidth - width);
                }
                else
                    m_TextCropPosition = 0;
            }
            else // You did select some characters, delete them
                deleteSelectedCharacters();

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[TextChanged].empty() == false)
            {
                m_Callback.trigger = TextChanged;
                m_Callback.text    = m_Text;
                addCallback();
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::textEntered(sf::Uint32 key)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // If only numbers are supported then make sure the input is valid
        if (m_NumbersOnly)
        {
            if ((key < '1') || (key > '9'))
                return;
        }

        // If there are selected characters then delete them first
        if (m_SelChars > 0)
            deleteSelectedCharacters();

        // Make sure we don't exceed our maximum characters limit
        if ((m_MaxChars > 0) && (m_Text.getSize() + 1 > m_MaxChars))
            return;

        // Insert our character
        m_Text.insert(m_SelEnd, key);

        // Change the displayed text
        if (m_PasswordChar != '\0')
            m_DisplayedText.insert(m_SelEnd, m_PasswordChar);
        else
            m_DisplayedText.insert(m_SelEnd, key);

        // Append the character to the text
        m_TextFull.setString(m_DisplayedText);

        // Calculate the space inside the edit box
        float width;
        if (m_SplitImage)
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
        else
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

        // When there is a text width limit then reverse what we just did
        if (m_LimitTextWidth)
        {
            // Now check if the text fits into the EditBox
            if (m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x > width)
            {
                // If the text does not fit in the EditBox then delete the added character
                m_Text.erase(m_SelEnd, 1);
                m_DisplayedText.erase(m_SelEnd, 1);
                return;
            }
        }

        // Move our selection point forward
        setSelectionPointPosition(m_SelEnd + 1);

        // The selection point should be visible again
        m_SelectionPointVisible = true;
        m_AnimationTimeElapsed = sf::Time();

        // Add the callback (if the user requested it)
        if (m_CallbackFunctions[TextChanged].empty() == false)
        {
            m_Callback.trigger = TextChanged;
            m_Callback.text    = m_Text;
            addCallback();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::objectUnfocused()
    {
        // If there is a selection then undo it now
        if (m_SelChars)
            setSelectionPointPosition(m_SelEnd);

        Object::objectUnfocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::findSelectionPointPosition(float posX)
    {
        // Take the scaling into account
        posX /= getScale().x;

        // This code will crash when the editbox is empty. We need to avoid this.
        if (m_DisplayedText.isEmpty())
            return 0;

        // Find out what the first visible character is
        unsigned int firstVisibleChar;
        if (m_TextCropPosition)
        {
            // Start searching near the selection point to quickly find the character even in a very long string
            firstVisibleChar = m_SelEnd;

            // Go backwards to find the character
            while (m_TextFull.findCharacterPos(firstVisibleChar-1).x > m_TextCropPosition)
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
        float width;
        if (m_SplitImage)
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
        else
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

        // If the width is negative then the editBox is too small to be displayed
        if (width < 0)
            width = 0;

        // Find out how many pixels the text is moved
        float pixelsToMove = 0;
        if (m_TextAlignment != Alignment::Left)
        {
            // Calculate the text width
            float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

            // Check if a layout would make sense
            if (textWidth < width)
            {
                // Set the number of pixels to move
                if (m_TextAlignment == Alignment::Center)
                    pixelsToMove = (width - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    pixelsToMove = width - textWidth;
            }
        }

        // Find out what the last visible character is, starting from the selection point
        lastVisibleChar = m_SelEnd;

        // Go forward to find the character
        while (m_TextFull.findCharacterPos(lastVisibleChar+1).x < m_TextCropPosition + width)
        {
            if (lastVisibleChar == m_DisplayedText.getSize())
                break;

            ++lastVisibleChar;
        }

        // Set the first part of the text
        tempString = m_DisplayedText.toWideString().substr(0, firstVisibleChar);
        m_TextFull.setString(tempString);

        // Calculate the first position
        fullTextWidth = m_TextFull.findCharacterPos(firstVisibleChar).x;

        // for all the other characters, check where you have clicked.
        for (unsigned int i = firstVisibleChar; i < lastVisibleChar; ++i)
        {
            // Add the next character to the temporary string
            tempString += m_DisplayedText[i];
            m_TextFull.setString(tempString);

            // Make some calculations
            textWidthWithoutLastChar = fullTextWidth;
            fullTextWidth = m_TextFull.findCharacterPos(i + 1).x;
            halfOfLastCharWidth = (fullTextWidth - textWidthWithoutLastChar) / 2.0f;

            // Check if you have clicked on the first halve of that character
            if (posX < textWidthWithoutLastChar + pixelsToMove + halfOfLastCharWidth - m_TextCropPosition)
            {
                m_TextFull.setString(m_DisplayedText);
                return i;
            }
        }

        // If you pass here then you clicked behind all the characters
        m_TextFull.setString(m_DisplayedText);
        return lastVisibleChar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::deleteSelectedCharacters()
    {
        // Check if the characters were selected from left to right
        if (m_SelStart < m_SelEnd)
        {
            // Erase the characters
            m_Text.erase(m_SelStart, m_SelChars);
            m_DisplayedText.erase(m_SelStart, m_SelChars);

            // Set the selection point back on the correct position
            setSelectionPointPosition(m_SelStart);
        }
        else // When the text is selected from right to left
        {
            // Erase the characters
            m_Text.erase(m_SelEnd, m_SelChars);
            m_DisplayedText.erase(m_SelEnd, m_SelChars);

            // Set the selection point back on the correct position
            setSelectionPointPosition(m_SelEnd);
        }

        // Calculate the space inside the edit box
        float width;
        if (m_SplitImage)
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.y / m_TextureNormal_M.getSize().y));
        else
            width = m_Size.x - ((m_LeftBorder + m_RightBorder) * (m_Size.x / m_TextureNormal_M.getSize().x));

        // If the width is negative then the edit box is too small to be displayed
        if (width < 0)
            width = 0;

        // Calculate the text width
        float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

        // If the text can be moved to the right then do so
        if (textWidth > width)
        {
            if (textWidth - m_TextCropPosition < width)
                m_TextCropPosition = static_cast<unsigned int>(textWidth - width);
        }
        else
            m_TextCropPosition = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::update()
    {
        // Only show/hide the selection point every half second
        if (m_AnimationTimeElapsed < sf::milliseconds(500))
            return;

        // Reset the elapsed time
        m_AnimationTimeElapsed = sf::Time();

        // Only update when the editbox is visible
        if (m_Visible == false)
            return;

        // Switch the value of the visible flag
        m_SelectionPointVisible = !m_SelectionPointVisible;

        // Too slow for double clicking
        m_PossibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Calculate the scaling
        Vector2f scaling(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);

        // Calculate the scale of the left and right border
        float borderScale;
        if (m_SplitImage)
            borderScale = scaling.y;
        else
            borderScale = scaling.x;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        Vector2f viewPosition = (target.getView().getSize() / 2.f) - target.getView().getCenter();

        // Get the global position
        Vector2f topLeftPosition = states.transform.transformPoint(getPosition().x + (m_LeftBorder * borderScale * getScale().x) + viewPosition.x,
                                                                   getPosition().y + (m_TopBorder * scaling.y * getScale().y) + viewPosition.y);
        Vector2f bottomRightPosition = states.transform.transformPoint(getPosition().x + ((m_Size.x - (m_RightBorder * borderScale)) * getScale().x) + viewPosition.x,
                                                                       getPosition().y + ((m_Size.y - (m_BottomBorder * scaling.y)) * getScale().y) + viewPosition.y);

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the object outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Apply the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Drawing the edit box will be different when the image is split
        if (m_SplitImage)
        {
            // Set the scaling for the left image
            states.transform.scale(scaling.y, scaling.y);

            // Draw the left image
            {
                // Draw the normal image
                target.draw(m_TextureNormal_L, states);

                // When the edit box is focused then draw an extra image
                if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                    target.draw(m_TextureFocused_L, states);

                // When the mouse is on top of the edit box then draw an extra image
                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                    target.draw(m_TextureHover_L, states);
            }

            // Check if the middle image may be drawn
            if ((scaling.y * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x)) < m_Size.x)
            {
                // Calculate the scale for our middle image
                float scaleX = scaling.x - (((m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * scaling.y) / m_TextureNormal_M.getSize().x);

                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L.getSize().x), 0);

                // Set the scale for the middle image
                states.transform.scale(scaleX / scaling.y, 1);

                // Draw the middle image
                {
                    // Draw the normal image
                    target.draw(m_TextureNormal_M, states);

                    // When the edit box is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                        target.draw(m_TextureFocused_M, states);

                    // When the mouse is on top of the edit box then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_TextureHover_M, states);
                }

                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_M.getSize().x), 0);

                // Set the scale for the right image
                states.transform.scale(scaling.y / scaleX, 1);

                // Draw the right image
                {
                    // Draw the normal image
                    target.draw(m_TextureNormal_R, states);

                    // When the edit box is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                        target.draw(m_TextureFocused_R, states);

                    // When the mouse is on top of the edit box then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_TextureHover_R, states);
                }
            }
            else // The edit box isn't width enough, we will draw it at minimum size
            {
                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L.getSize().x), 0);

                // Draw the right image
                {
                    // Draw the normal image
                    target.draw(m_TextureNormal_R, states);

                    // When the edit box is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                        target.draw(m_TextureFocused_R, states);

                    // When the mouse is on top of the edit box then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_TextureHover_R, states);
                }
            }
        }
        else // The image is not split
        {
            // Set the scaling
            states.transform.scale(scaling);

            // Draw the edit box
            target.draw(m_TextureNormal_M, states);

            // When the edit box is focused then draw an extra image
            if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                target.draw(m_TextureFocused_M, states);

            // When the mouse is on top of the edit boc then draw an extra image
            if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                target.draw(m_TextureHover_M, states);
        }

        // Reset the transformation to draw the text
        states.transform = oldTransform;
        states.transform.translate(m_LeftBorder * borderScale - m_TextCropPosition, m_TopBorder * scaling.y);

        // Check if the layout wasn't left
        if (m_TextAlignment != Alignment::Left)
        {
            // Calculate the space inside the edit box
            float width = m_Size.x - ((m_LeftBorder + m_RightBorder) * borderScale);

            // Calculate the text width
            float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

            // Check if a layout would make sense
            if (textWidth < width)
            {
                // Put the text on the correct position
                if (m_TextAlignment == Alignment::Center)
                    states.transform.translate((width - textWidth) / 2.f, 0);
                else // if (textAlignment == Alignment::Right)
                    states.transform.translate(width - textWidth, 0);
            }
        }

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the background when a text is selected
        if (m_TextSelection.getString().getSize() > 0)
        {
            // Watch out for the kerning
            if (m_TextBeforeSelection.getString().getSize() > 0)
                states.transform.translate(static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[m_TextBeforeSelection.getString().getSize() - 1], m_DisplayedText[m_TextBeforeSelection.getString().getSize()], m_TextBeforeSelection.getCharacterSize())), 0);

            // Create and draw the rectangle
            sf::RectangleShape TextBgr(Vector2f(m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x, (m_Size.y - ((m_TopBorder + m_BottomBorder) * scaling.y))));
            TextBgr.setFillColor(m_SelectedTextBgrColor);
            TextBgr.setPosition(m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x, 0);
            target.draw(TextBgr, states);

            // Undo the translation that was done to fix the kerning
            if (m_TextBeforeSelection.getString().getSize() > 0)
                states.transform.translate(-static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[m_TextBeforeSelection.getString().getSize() - 1], m_DisplayedText[m_TextBeforeSelection.getString().getSize()], m_TextBeforeSelection.getCharacterSize())), 0);
        }

        // Set the position of the text
        sf::Text tempText(m_TextFull);
        tempText.setString("kg");
        states.transform.translate(0, std::floor((((m_Size.y - ((m_TopBorder + m_BottomBorder) * scaling.y)) - tempText.getLocalBounds().height) * 0.5f) - tempText.getLocalBounds().top + 0.5f));

        // Draw the text before the selection
        target.draw(m_TextBeforeSelection, states);

        // Check if there is a selection
        if (m_SelChars != 0)
        {
            // Watch out for the kerning
            if (m_TextBeforeSelection.getString().getSize() > 0)
                states.transform.translate(static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[m_TextBeforeSelection.getString().getSize() - 1], m_DisplayedText[m_TextBeforeSelection.getString().getSize()], m_TextBeforeSelection.getCharacterSize())), 0);

            // Draw the selected text
            states.transform.translate(m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x, 0);
            target.draw(m_TextSelection, states);

            // Watch out for kerning
            if (m_DisplayedText.getSize() > m_TextBeforeSelection.getString().getSize() + m_TextSelection.getString().getSize() - 1)
                states.transform.translate(static_cast<float>(m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[m_TextBeforeSelection.getString().getSize() + m_TextSelection.getString().getSize() - 1], m_DisplayedText[m_TextBeforeSelection.getString().getSize() + m_TextSelection.getString().getSize()], m_TextBeforeSelection.getCharacterSize())), 0);

            // Draw the text behind the selected text
            states.transform.translate(m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x, 0);
            target.draw(m_TextAfterSelection, states);
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Reset the transformation to draw the selection point
        states.transform = oldTransform;
        states.transform.translate(m_LeftBorder * borderScale - m_TextCropPosition + m_TextFull.findCharacterPos(m_SelEnd).x - (m_SelectionPointWidth*0.5f), m_TopBorder * scaling.y);

        // Also draw the selection point (if needed)
        if ((m_Focused) && (m_SelectionPointVisible))
        {
            // Check if the layout wasn't left
            if (m_TextAlignment != Alignment::Left)
            {
                // Calculate the space inside the edit box
                float width = m_Size.x - ((m_LeftBorder + m_RightBorder) * borderScale);

                // Calculate the text width
                float textWidth = m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x;

                // Check if a layout would make sense
                if (textWidth < width)
                {
                    // Put the selection point on the correct position
                    if (m_TextAlignment == Alignment::Center)
                        states.transform.translate((width - textWidth) / 2.f, 0);
                    else // if (textAlignment == Alignment::Right)
                        states.transform.translate(width - textWidth, 0);
                }
            }

            // Draw the selection point
            sf::RectangleShape SelectionPoint(Vector2f(static_cast<float>(m_SelectionPointWidth), m_Size.y - ((m_BottomBorder + m_TopBorder) * scaling.y)));
            SelectionPoint.setFillColor(m_SelectionPointColor);
            target.draw(SelectionPoint, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
