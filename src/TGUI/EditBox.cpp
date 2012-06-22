/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox() :
    selectionPointColor     (110, 110, 255),
    selectionPointWidth     (2),
    m_SelectionPointVisible (true),
    m_SelectionPointPosition(0),
    m_LimitTextWidth        (false),
    m_DisplayedText         (""),
    m_SelText               (""),
    m_Text                  (""),
    m_TextSize              (0),
    m_SelChars              (0),
    m_SelStart              (0),
    m_SelEnd                (0),
    m_PasswordChar          ('\0'),
    m_MaxChars              (0),
    m_SplitImage            (false),
    m_LeftTextCrop          (0),
    m_RightTextCrop         (0),
    m_TextureNormal_L       (NULL),
    m_TextureNormal_M       (NULL),
    m_TextureNormal_R       (NULL),
    m_TextureHover_L        (NULL),
    m_TextureHover_M        (NULL),
    m_TextureHover_R        (NULL),
    m_TextureFocused_L        (NULL),
    m_TextureFocused_M        (NULL),
    m_TextureFocused_R        (NULL)
    {
        m_ObjectType = editBox;

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox(const EditBox& copy) :
    OBJECT                         (copy),
    selectionPointColor            (copy.selectionPointColor),
    selectionPointWidth            (copy.selectionPointWidth),
    m_SelectionPointVisible        (copy.m_SelectionPointVisible),
    m_SelectionPointPosition       (copy.m_SelectionPointPosition),
    m_LimitTextWidth               (copy.m_LimitTextWidth),
    m_DisplayedText                (copy.m_DisplayedText),
    m_SelText                      (copy.m_SelText),
    m_Text                         (copy.m_Text),
    m_TextSize                     (copy.m_TextSize),
    m_SelChars                     (copy.m_SelChars),
    m_SelStart                     (copy.m_SelStart),
    m_SelEnd                       (copy.m_SelEnd),
    m_PasswordChar                 (copy.m_PasswordChar),
    m_MaxChars                     (copy.m_MaxChars),
    m_SplitImage                   (copy.m_SplitImage),
    m_LeftTextCrop                 (copy.m_LeftTextCrop),
    m_RightTextCrop                (copy.m_RightTextCrop),
    m_SelectedTextBgrColor         (copy.m_SelectedTextBgrColor),
    m_UnfocusedSelectedTextBgrColor(copy.m_UnfocusedSelectedTextBgrColor),
    m_TextBeforeSelection          (copy.m_TextBeforeSelection),
    m_TextSelection                (copy.m_TextSelection),
    m_TextAfterSelection           (copy.m_TextAfterSelection)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L))       m_SpriteNormal_L.setTexture(*m_TextureNormal_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M))       m_SpriteNormal_M.setTexture(*m_TextureNormal_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R))       m_SpriteNormal_R.setTexture(*m_TextureNormal_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureHover_L, m_TextureHover_L))         m_SpriteHover_L.setTexture(*m_TextureHover_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureHover_M, m_TextureHover_M))         m_SpriteHover_M.setTexture(*m_TextureHover_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureHover_R, m_TextureHover_R))         m_SpriteHover_R.setTexture(*m_TextureHover_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_L, m_TextureFocused_L))     m_SpriteFocused_L.setTexture(*m_TextureFocused_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_M, m_TextureFocused_M))     m_SpriteFocused_M.setTexture(*m_TextureFocused_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_R, m_TextureFocused_R))     m_SpriteFocused_R.setTexture(*m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::~EditBox()
    {
        // Remove all the textures

        if (m_TextureNormal_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureHover_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_R);

        if (m_TextureFocused_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox& EditBox::operator= (const EditBox& right)
    {
        if (this != &right)
        {
            EditBox temp(right);
            this->OBJECT::operator=(right);

            std::swap(selectionPointColor,             temp.selectionPointColor);
            std::swap(selectionPointWidth,             temp.selectionPointWidth);
            std::swap(m_SelectionPointVisible,         temp.m_SelectionPointVisible);
            std::swap(m_SelectionPointPosition,        temp.m_SelectionPointPosition);
            std::swap(m_LimitTextWidth,                temp.m_LimitTextWidth);
            std::swap(m_DisplayedText,                 temp.m_DisplayedText);
            std::swap(m_SelText,                       temp.m_SelText);
            std::swap(m_Text,                          temp.m_Text);
            std::swap(m_TextSize,                      temp.m_TextSize);
            std::swap(m_SelChars,                      temp.m_SelChars);
            std::swap(m_SelStart,                      temp.m_SelStart);
            std::swap(m_SelEnd,                        temp.m_SelEnd);
            std::swap(m_PasswordChar,                  temp.m_PasswordChar);
            std::swap(m_MaxChars,                      temp.m_MaxChars);
            std::swap(m_SplitImage,                    temp.m_SplitImage);
            std::swap(m_LeftTextCrop,                  temp.m_LeftTextCrop);
            std::swap(m_RightTextCrop,                 temp.m_RightTextCrop);
            std::swap(m_SelectedTextBgrColor,          temp.m_SelectedTextBgrColor);
            std::swap(m_UnfocusedSelectedTextBgrColor, temp.m_UnfocusedSelectedTextBgrColor);
            std::swap(m_TextBeforeSelection,           temp.m_TextBeforeSelection);
            std::swap(m_TextSelection,                 temp.m_TextSelection);
            std::swap(m_TextAfterSelection,            temp.m_TextAfterSelection);
            std::swap(m_TextureNormal_L,               temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,               temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,               temp.m_TextureNormal_R);
            std::swap(m_TextureHover_L,                temp.m_TextureHover_L);
            std::swap(m_TextureHover_M,                temp.m_TextureHover_M);
            std::swap(m_TextureHover_R,                temp.m_TextureHover_R);
            std::swap(m_TextureFocused_L,              temp.m_TextureFocused_L);
            std::swap(m_TextureFocused_M,              temp.m_TextureFocused_M);
            std::swap(m_TextureFocused_R,              temp.m_TextureFocused_R);
            std::swap(m_SpriteNormal_L,                temp.m_SpriteNormal_L);
            std::swap(m_SpriteNormal_M,                temp.m_SpriteNormal_M);
            std::swap(m_SpriteNormal_R,                temp.m_SpriteNormal_R);
            std::swap(m_SpriteHover_L,                 temp.m_SpriteHover_L);
            std::swap(m_SpriteHover_M,                 temp.m_SpriteHover_M);
            std::swap(m_SpriteHover_R,                 temp.m_SpriteHover_R);
            std::swap(m_SpriteFocused_L,               temp.m_SpriteFocused_L);
            std::swap(m_SpriteFocused_M,               temp.m_SpriteFocused_M);
            std::swap(m_SpriteFocused_R,               temp.m_SpriteFocused_R);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::load(const std::string pathname)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Make sure that the pathname isn't empty
        if (pathname.empty())
            return false;

        // Store the pathname
        m_LoadedPathname = pathname;

        // When the pathname does not end with a "/" then we will add it
        if (m_LoadedPathname.at(m_LoadedPathname.length()-1) != '/')
            m_LoadedPathname.push_back('/');

        // Open the info file
        InfoFileParser infoFile;
        if (infoFile.openFile(m_LoadedPathname + "info.txt") == false)
        {
            TGUI_OUTPUT((((std::string("TGUI: Failed to open ")).append(m_LoadedPathname)).append("info.txt")).c_str());
            return false;
        }

        std::string property;
        std::string value;

        // Set some default settings
        m_SplitImage = false;
        std::string imageExtension = "png";

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            // Check what the property is
            if (property.compare("splitimage") == 0)
            {
                // Find out if it is split or not
                if (value.compare("true") == 0)
                    m_SplitImage = true;
                else if (value.compare("false") == 0)
                    m_SplitImage = false;
            }
            else if (property.compare("phases") == 0)
            {
                // Get and store the different phases
                extractPhases(value);
            }
            else if (property.compare("extension") == 0)
            {
                imageExtension = value;
            }
            else if (property.compare("borders") == 0)
            {
                // Get the borders
                Vector4u borders;
                if (extractVector4u(value, borders))
                    setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
            }
            else if (property.compare("textcolor") == 0)
            {
                sf::Color color = extractColor(value);
                m_TextBeforeSelection.setColor(color);
                m_TextAfterSelection.setColor(color);
            }
            else if (property.compare("selecetedtextcolor") == 0)
            {
                m_TextSelection.setColor(extractColor(value));
            }
            else if (property.compare("selectedtextbackgroundcolor") == 0)
            {
                m_SelectedTextBgrColor = extractColor(value);
            }
            else if (property.compare("unfocusedselectedtextbackgroundcolor") == 0)
            {
                m_UnfocusedSelectedTextBgrColor = extractColor(value);
            }
            else if (property.compare("selectionpointcolor") == 0)
            {
                selectionPointColor = extractColor(value);
            }
        }

        // Close the info file
        infoFile.closeFile();

        // Remove the textures when they were loaded before
        if (m_TextureNormal_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureHover_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureHover_R);
        if (m_TextureFocused_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureFocused_R);

        bool error = false;

        // Check if the image is split
        if (m_SplitImage)
        {
            // load the required texture
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Normal." + imageExtension, m_TextureNormal_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Normal." + imageExtension, m_TextureNormal_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Normal." + imageExtension, m_TextureNormal_R)))
            {
                m_SpriteNormal_L.setTexture(*m_TextureNormal_L, true);
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
                m_SpriteNormal_R.setTexture(*m_TextureNormal_R, true);
            }
            else
                return false;

            // load the optional textures
            if (m_ObjectPhase & objectPhase::focused)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Focus." + imageExtension, m_TextureFocused_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Focus." + imageExtension, m_TextureFocused_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Focus." + imageExtension, m_TextureFocused_R)))
                {
                    m_SpriteFocused_L.setTexture(*m_TextureFocused_L, true);
                    m_SpriteFocused_M.setTexture(*m_TextureFocused_M, true);
                    m_SpriteFocused_R.setTexture(*m_TextureFocused_R, true);

                    m_AllowFocus = true;
                }
                else
                    error = true;
            }

            if (m_ObjectPhase & objectPhase::hover)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Hover." + imageExtension, m_TextureHover_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Hover." + imageExtension, m_TextureHover_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Hover." + imageExtension, m_TextureHover_R)))
                {
                    m_SpriteHover_L.setTexture(*m_TextureHover_L, true);
                    m_SpriteHover_M.setTexture(*m_TextureHover_M, true);
                    m_SpriteHover_R.setTexture(*m_TextureHover_R, true);
                }
                else
                    error = true;
            }
        }
        else // The image isn't split
        {
            // load the required texture
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Normal." + imageExtension, m_TextureNormal_M))
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
            else
                return false;

            // load the optional textures
            if (m_ObjectPhase & objectPhase::focused)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Focus." + imageExtension, m_TextureFocused_M))
                {
                    m_SpriteFocused_M.setTexture(*m_TextureFocused_M, true);
                    m_AllowFocus = true;
                }
                else
                    error = true;
            }

            if (m_ObjectPhase & objectPhase::hover)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Hover." + imageExtension, m_TextureHover_M))
                    m_SpriteHover_M.setTexture(*m_TextureHover_M, true);
                else
                    error = true;
            }
        }

        // Check if we are auto scaling the text
        if (m_TextSize == 0)
        {
            unsigned int size = static_cast<unsigned int>((m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * getScale().y);

            // Set the text size
            m_TextBeforeSelection.setCharacterSize(size);
            m_TextSelection.setCharacterSize(size);
            m_TextAfterSelection.setCharacterSize(size);
        }

        // When there is no error we will return true
        m_Loaded = !error;
        return !error;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSize(float width, float height)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if the image is split
        if (m_SplitImage)
        {
            setScale(width / (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x),
                     height / m_TextureNormal_M->getSize().y);
        }
        else // The image is not split
        {
            setScale(width / m_TextureNormal_M->getSize().x,
                     height / m_TextureNormal_M->getSize().y);
        }

        // Store the whole string in one text object to make the calculations
        sf::Text tempText(m_TextBeforeSelection);
        tempText.setString(m_DisplayedText);

        // When there is a text width limit then we have to check if the text stays within these limits
        if (m_LimitTextWidth)
        {
            // Remember the current text length
            unsigned int textLength = m_DisplayedText.length();

            // Check if the text fits into the EditBox
//            while (tempText.getGlobalBounds().width > width)
            while (tempText.findCharacterPos(tempText.getString().getSize()).x > width)
            {
                // If the text does not fit in the EditBox then delete the last character
                m_Text.erase(m_Text.length()-1);
                m_DisplayedText.erase(m_DisplayedText.length()-1);

                // Refresh the text
                tempText.setString(m_DisplayedText);
            }

            // Check if the text changed
            if (m_DisplayedText.length() < textLength)
            {
                // Change the texts that are drawn on the screen
                m_TextBeforeSelection.setString(
                                                m_DisplayedText.substr(
                                                                       0,
                                                                       m_TextBeforeSelection.getString().getSize()
                                                                      )
                                              );

                m_TextSelection.setString(
                                          m_DisplayedText.substr(
                                                                 m_TextBeforeSelection.getString().getSize(),
                                                                 m_TextSelection.getString().getSize()
                                                                )
                                        );

                m_TextAfterSelection.setString(
                                               m_DisplayedText.substr(
                                                                      m_TextBeforeSelection.getString().getSize()
                                                                       + m_TextSelection.getString().getSize(),
                                                                      m_TextAfterSelection.getString().getSize()
                                                                     )
                                             );
            }
        }
        else // Scrolling is enabled
        {
            float editBoxWidth;

            // Calculate the width of the edit box
            if (m_SplitImage)
                editBoxWidth = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                         - ((m_LeftBorder + m_RightBorder) * getScale().y);
            else
                editBoxWidth = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

            // Now check if the text fits into the EditBox
            if (m_TextBeforeSelection.findCharacterPos(m_DisplayedText.length()).x > editBoxWidth)
            {
                // Reset the left crop position
                m_LeftTextCrop = 0;

                // The text is too long to fit inside the EditBox
                while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > editBoxWidth)
                {
                    // Check if the last character can be dropped
                    if (m_SelEnd < m_RightTextCrop)
                        --m_RightTextCrop;
                    else
                        ++m_LeftTextCrop;
                }

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelectionPointPosition);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u EditBox::getSize() const
    {
        // Don't continue when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2u(0, 0);

        // Check if the image is split
        if (m_SplitImage)
        {
            // Return the size of the three images
            return Vector2u(m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x, m_TextureNormal_M->getSize().y);
        }
        else // The image is not split
        {
            // Return the size of the image
            return Vector2u(m_TextureNormal_M->getSize().x, m_TextureNormal_M->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f EditBox::getScaledSize() const
    {
        // Don't continue when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return Vector2f(0, 0);

        // Check if the image is split
        if (m_SplitImage)
        {
            // Return the size of the three images
            return Vector2f((m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x) * getScale().x,
                            m_TextureNormal_M->getSize().y * getScale().y);
        }
        else // The image is not split
        {
            // Return the size of the image
            return Vector2f(m_TextureNormal_M->getSize().x * getScale().x, m_TextureNormal_M->getSize().y * getScale().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string EditBox::getLoadedPathname()
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setText(const std::string text)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Get the current scale
        Vector2f curScale = getScale();

        // Change the text size
        if (m_TextSize > 0)
        {
            unsigned int size = static_cast<unsigned int>(m_TextSize * curScale.y);

            m_TextBeforeSelection.setCharacterSize(size);
            m_TextSelection.setCharacterSize(size);
            m_TextAfterSelection.setCharacterSize(size);
        }
        else // The text size must be calculated
        {
            unsigned int size = static_cast<unsigned int>((m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * curScale.y);

            m_TextBeforeSelection.setCharacterSize(size);
            m_TextSelection.setCharacterSize(size);
            m_TextAfterSelection.setCharacterSize(size);
        }

        // Change the text
        m_Text = text;
        m_DisplayedText = text;

        // If there is a character limit then check if it is exeeded
        if ((m_MaxChars > 0) && (m_DisplayedText.length() > m_MaxChars))
        {
            // Remove all the excess characters
            while (m_DisplayedText.length() > m_MaxChars)
            {
                m_Text.erase(m_Text.length()-1, 1);
                m_DisplayedText.erase(m_Text.length()-1, 1);
            }
        }

        // Check if there is a password character
        if (m_PasswordChar != '\0')
        {
            // Loop every character and change it
            for (unsigned int x=0; x < m_Text.length(); ++x)
                m_DisplayedText.at(x) = m_PasswordChar;
        }

        // Change the text that will be displayed
        m_TextBeforeSelection.setString(m_DisplayedText);

        // The text has comlpetely changed, so there will no longer be selected text
        m_TextSelection.setString("");
        m_TextAfterSelection.setString("");


        float width;

        // Calculate the width of the edit box
        if (m_SplitImage)
            width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * curScale.x)
                     - ((m_LeftBorder + m_RightBorder) * curScale.y);
        else
            width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * curScale.x;

        // When there is a text width limit then we have to check that one too
        if (m_LimitTextWidth)
        {
            // Now check if the text fits into the EditBox
//            while (m_TextBeforeSelection.getGlobalBounds().width > width)
            while (m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x > width)
            {
                // Make sure that you are not trying to erase if it is already empty
                if (m_DisplayedText.empty())
                    break;

                // The text doesn't fit inside the EditBox, so the last character must be deleted.
                m_Text.erase(m_Text.length()-1);
                m_DisplayedText.erase(m_DisplayedText.length()-1);

                // Set the new text
                m_TextBeforeSelection.setString(m_DisplayedText);
            }
        }
        else // Scrolling is enabled
        {
            // Set the text crop
            m_LeftTextCrop = 0;
            m_RightTextCrop = m_DisplayedText.length();

            // Now check if the text fits into the EditBox
            if (m_TextBeforeSelection.findCharacterPos(m_DisplayedText.length()).x > width)
            {
                // The text is too long to fit inside the EditBox
                while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > width)
                {
                    // Drop the first character
                    ++m_LeftTextCrop;
                }
            }
        }

        // Set the selection point behind the last character
        setSelectionPointPosition(m_DisplayedText.length());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string EditBox::getText()
    {
        return m_Text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextSize(const unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getTextSize()
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTextFont(const sf::Font& font)
    {
        m_TextBeforeSelection.setFont(font);
        m_TextSelection.setFont(font);
        m_TextAfterSelection.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& EditBox::getTextFont()
    {
        return m_TextBeforeSelection.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordChar(const char passwordChar)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Change the password character
        m_PasswordChar = passwordChar;

        // Check f the password character was 0
        if (m_PasswordChar == '\0')
        {
            // If it was 0 then just copy the text
            m_DisplayedText = m_Text;
        }
        else
        {
            // Set the password character
            for (unsigned int x=0; x < m_DisplayedText.length(); ++x)
                m_DisplayedText.at(x) = m_PasswordChar;
        }


        float width;

        // Calculate the width of the edit box
        if (m_SplitImage)
            width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                     - ((m_LeftBorder + m_RightBorder) * getScale().y);
        else
            width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

        // When there is a text width limit then we have to check if the text stays within these limits
        if (m_LimitTextWidth)
        {
            // Check if the text fits into the EditBox
//            while (m_TextBeforeSelection.getGlobalBounds().width > width)
            while (m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x > width)
            {
                // If the text does not fit in the EditBox then delete the last character
                m_Text.erase(m_Text.length()-1);
                m_DisplayedText.erase(m_DisplayedText.length()-1);
            }
        }
        else // Scrolling is enabled
        {
            // Check if we can't just keep the current cropping
            while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > width)
            {
                // Check if the last character can be dropped
                if (m_SelEnd < m_RightTextCrop)
                    --m_RightTextCrop;
                else
                    ++m_LeftTextCrop;
            }
        }

        // The internal text has changed, so the text to display also has to change (the length stays the same)
        m_TextBeforeSelection.setString(
                                        m_DisplayedText.substr(
                                                               0,
                                                               m_TextBeforeSelection.getString().getSize()
                                                              )
                                      );

        m_TextSelection.setString(
                                  m_DisplayedText.substr(
                                                         m_TextBeforeSelection.getString().getSize(),
                                                         m_TextSelection.getString().getSize()
                                                        )
                                );

        m_TextAfterSelection.setString(
                                       m_DisplayedText.substr(
                                                              m_TextBeforeSelection.getString().getSize()
                                                               + m_TextSelection.getString().getSize(),
                                                              m_TextAfterSelection.getString().getSize()
                                                             )
                                     );

        // The position of the selection point must also be changed
        if (m_SplitImage)
        {
            if (m_SelChars == 0)
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().y) + m_TextBeforeSelection.findCharacterPos(m_SelEnd).x);
            else
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().y)
                                            + m_TextSelection.findCharacterPos(m_SelEnd - m_TextBeforeSelection.getString().getSize()).x
//                                            + m_TextBeforeSelection.getGlobalBounds().width);
                                            + m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x);
        }
        else
        {
            if (m_SelChars == 0)
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().x) + m_TextBeforeSelection.findCharacterPos(m_SelEnd).x);
            else
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().x)
                                            + m_TextSelection.findCharacterPos(m_SelEnd - m_TextBeforeSelection.getString().getSize()).x
//                                              + m_TextBeforeSelection.getGlobalBounds().width);
                                            + m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    char EditBox::getPasswordChar()
    {
        return m_PasswordChar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setMaximumCharacters(const unsigned int maxChars)
    {
        // Set the new character limit ( 0 to disable the limit )
        m_MaxChars = maxChars;

        // If there is a character limit then check if it is exeeded
        if ((m_MaxChars > 0) && (m_DisplayedText.length() > m_MaxChars))
        {
            // Remove all the excess characters
            while (m_DisplayedText.length() > m_MaxChars)
            {
                m_Text.erase(m_Text.length()-1, 1);
                m_DisplayedText.erase(m_DisplayedText.length()-1, 1);
            }

            // If we passed here then the internal text has changed. We also need to change the one to display.
            m_TextBeforeSelection.setString(m_DisplayedText);
            m_TextSelection.setString(std::string(""));
            m_TextAfterSelection.setString(std::string(""));

            // Check if scrolling is enabled
            if (m_LimitTextWidth == false)
            {
                // Check if the right crop position lies behind the character limit
                if (m_RightTextCrop > m_MaxChars)
                {
                    float width;

                    // Calculate the width of the edit box
                    if (m_SplitImage)
                        width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                                 - ((m_LeftBorder + m_RightBorder) * getScale().y);
                    else
                        width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                    // Check if the right crop position lies behind the character limit
                    while (m_RightTextCrop > m_MaxChars)
                    {
                        // Lower the right crop position
                        --m_RightTextCrop;

                        // Check if the left crop position can be changed
                        while (m_LeftTextCrop > 0)
                        {
                            // Check if it may be changed
                            if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop - 1).x) <= width)
                            {
                                // Change the left crop position
                                --m_LeftTextCrop;
                            }
                            else // The left crop position may no longer be changed
                                break;
                        }
                    }
                }
            }
        }

        // Set the selection point behind the last character
        setSelectionPointPosition(m_DisplayedText.length());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getMaximumCharacters()
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

        // Nothing can be calculated when the load function failed
        if (m_Loaded == false)
            return;

        // The borders have changed, so the selection point position has to change too.
        if (m_SplitImage)
        {
            if (m_SelChars == 0)
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().y) + m_TextBeforeSelection.findCharacterPos(m_SelEnd).x);
            else
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().y)
                                            + m_TextSelection.findCharacterPos(m_SelEnd - m_TextBeforeSelection.getString().getSize()).x
//                                            + m_TextBeforeSelection.getGlobalBounds().width);
                                            + m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x);
        }
        else
        {
            if (m_SelChars == 0)
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().x) + m_TextBeforeSelection.findCharacterPos(m_SelEnd).x);
            else
                m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().x)
                                            + m_TextSelection.findCharacterPos(m_SelEnd - m_TextBeforeSelection.getString().getSize()).x
//                                              + m_TextBeforeSelection.getGlobalBounds().width);
                                            + m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x);
        }

        // Recalculate the text size
        if (m_TextSize > 0)
        {
            unsigned int size = static_cast<unsigned int>(m_TextSize * getScale().y);

            m_TextBeforeSelection.setCharacterSize(size);
            m_TextSelection.setCharacterSize(size);
            m_TextAfterSelection.setCharacterSize(size);
        }
        else // The text size must be calculated
        {
            unsigned int size = static_cast<unsigned int>((m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * getScale().y);

            m_TextBeforeSelection.setCharacterSize(size);
            m_TextSelection.setCharacterSize(size);
            m_TextAfterSelection.setCharacterSize(size);
        }

        float width;

        // Calculate the width of the edit box
        if (m_SplitImage)
            width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                     - ((m_LeftBorder + m_RightBorder) * getScale().y);
        else
            width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

        // Store the whole string in one text object to make the calculations
        sf::Text tempText(m_TextBeforeSelection);
        tempText.setString(m_DisplayedText);

        // When there is a text width limit then we have to check if the text stays within these limits
        if (m_LimitTextWidth)
        {
            // Remember the current text length
            unsigned int textLength = m_DisplayedText.length();

            // Check if the text fits into the EditBox
//            while (tempText.getGlobalBounds().width > width)
            while (tempText.findCharacterPos(tempText.getString().getSize()).x > width)
            {
                // If the text does not fit in the EditBox then delete the last character
                m_Text.erase(m_Text.length()-1);
                m_DisplayedText.erase(m_DisplayedText.length()-1);

                // Refresh the text
                tempText.setString(m_DisplayedText);
            }

            // Check if the text changed
            if (m_DisplayedText.length() < textLength)
            {
                // Change the texts that are drawn on the screen
                m_TextBeforeSelection.setString(
                                                m_DisplayedText.substr(
                                                                       0,
                                                                       m_TextBeforeSelection.getString().getSize()
                                                                      )
                                              );

                m_TextSelection.setString(
                                          m_DisplayedText.substr(
                                                                 m_TextBeforeSelection.getString().getSize(),
                                                                 m_TextSelection.getString().getSize()
                                                                )
                                        );

                m_TextAfterSelection.setString(
                                               m_DisplayedText.substr(
                                                                      m_TextBeforeSelection.getString().getSize()
                                                                       + m_TextSelection.getString().getSize(),
                                                                      m_TextAfterSelection.getString().getSize()
                                                                     )
                                             );
            }
        }
        else // Scrolling is enabled
        {
            unsigned int length = m_DisplayedText.length();

            // Now check if the text fits into the EditBox
            while (m_TextBeforeSelection.findCharacterPos(length).x > width)
            {
                // The text is still too long
                --length;
            }

            // Set the text crop
            m_LeftTextCrop = 0;
            m_RightTextCrop = length;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::changeColors(const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& unfocusedSelectedBgrColor)
    {
        m_TextBeforeSelection.setColor(color);
        m_TextSelection.setColor(selectedColor);
        m_TextAfterSelection.setColor(color);

        m_SelectedTextBgrColor = selectedBgrColor;
        m_UnfocusedSelectedTextBgrColor = unfocusedSelectedBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getTextColor()
    {
        return m_TextBeforeSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectedTextColor()
    {
        return m_TextSelection.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectedTextBackgroundColor()
    {
        return m_SelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getUnfocusedSelectedTextBackgroundColor()
    {
        return m_UnfocusedSelectedTextBgrColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointPosition(const unsigned int charactersBeforeSelectionPoint)
    {
        // Set the selection point to the correct position
        m_SelChars = 0;
        m_SelStart = charactersBeforeSelectionPoint;
        m_SelEnd = charactersBeforeSelectionPoint;

        // Change our three texts
        m_TextBeforeSelection.setString(m_DisplayedText);
        m_TextSelection.setString("");
        m_TextAfterSelection.setString("");


        float position = m_TextBeforeSelection.findCharacterPos(m_SelEnd).x;

        // Check if scrolling is enabled, if so then adjust the position
        if (m_LimitTextWidth == false)
            position -= m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x;

        // Now change the position of the selection point
        if (m_SplitImage)
            m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().y) + position);
        else
            m_SelectionPointPosition = static_cast<unsigned int>((m_LeftBorder * getScale().x) + position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::findSelectionPointPosition(float posX)
    {
        // This code will crash when the editbox is empty. We need to avoid this.
        if (m_DisplayedText.length() == 0)
            return 0;

        // What part of the text is visible
        unsigned int leftBound, rightBound;

        // Check if there is a text width limit
        if (m_LimitTextWidth)
        {
            leftBound = 0;
            rightBound = m_DisplayedText.length();
        }
        else // Scrolling is enabled
        {
            leftBound = m_LeftTextCrop;
            rightBound = m_RightTextCrop;
        }

        // Create a temp SFML text and give it the correct size
        sf::Text tempText(m_TextBeforeSelection);
        tempText.setString(m_DisplayedText);

        // check if clicked on the first character.
        if (posX < ((tempText.findCharacterPos(leftBound + 1).x - tempText.findCharacterPos(leftBound).x) / 2.0f) + 0.5f)
            return leftBound;

        // This string will store pieces of the text
        std::string tempString;

        // Add the first character to our temp string
        tempString.push_back(m_DisplayedText.at(leftBound));

        // for all the other characters, check where you have clicked.
        for (unsigned int x = leftBound + 1; x < rightBound; ++x)
        {
            // Add the next character to the temp string
            tempString.push_back(m_DisplayedText.at(x));

            // Set the string
            tempText.setString(tempString);

            // Make some calculations
            float textWidthWithoutLastChar = tempText.findCharacterPos(x - leftBound).x;
            float fullTextWidth = tempText.findCharacterPos(x - leftBound + 1).x;
            float halfOfLastCharWidth = (fullTextWidth - textWidthWithoutLastChar) / 2.0f;

            // Check if you have clicked on the first halve of that character
            if (posX < textWidthWithoutLastChar + halfOfLastCharWidth + 0.5f)
                return x;
        }

        // If you pass here then you clicked behind all the characters
        return rightBound;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::mouseOnObject(float x, float y)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        float editBoxWidth;
        float editBoxHeight;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Calculate the edit box size
        if (m_SplitImage)
        {
            editBoxHeight = m_TextureNormal_M->getSize().y * curScale.y;

            // Check if the middle part will be drawn (this won't happen when the x scale is too small
            if ((curScale.y * (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x))
                < curScale.x * (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x))
            {
                editBoxWidth = (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x + m_TextureNormal_M->getSize().x) * getScale().x;
            }
            else // The middle part won't be drawn
            {
                editBoxWidth = (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * curScale.y;
            }
        }
        else
        {
            editBoxWidth = m_TextureNormal_M->getSize().x * curScale.x;
            editBoxHeight = m_TextureNormal_M->getSize().y * curScale.y;
        }

        // Check if the mouse is on top of the edit box
        if ((x > position.x) && (x < position.x + editBoxWidth)
            && (y > position.y) && (y < position.y + editBoxHeight))
        {
            return true;
        }
        else // When the mouse is not on top of the edit box
        {
            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMousePressed(float x, float y)
    {
        TGUI_UNUSED_PARAM(y);

        // Check if the checkbox was already focused
        if (m_Focused)
        {
            // Set the new selection point
            setSelectionPointPosition(static_cast<unsigned int>(findSelectionPointPosition(x - getPosition().x - m_LeftBorder)));

            // Set the mouse down flag
            m_MouseDown = true;
        }
        else // When the editbox was not focused
        {
            // Make sure that no text was selected
            if (m_SelChars == 0)
            {
                // When no text was selected then just do the same as when the edibox was already focused
                setSelectionPointPosition(static_cast<unsigned int>(findSelectionPointPosition(x - getPosition().x - m_LeftBorder)));
                m_MouseDown = true;
            }
        }

        // The selection point should be visible
        m_SelectionPointVisible = true;
        m_AnimationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::leftMouseReleased(float x, float y)
    {
        TGUI_UNUSED_PARAM(x);
        TGUI_UNUSED_PARAM(y);

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(float x, float y)
    {
        TGUI_UNUSED_PARAM(y);

        // Set the mouse move flag
        m_MouseHover = true;

        // Check if the mouse is hold down (we are selecting multiple characters)
        if (m_MouseDown)
        {
            // Check if there is a text width limit
            if (m_LimitTextWidth)
            {
                // Find out between which characters the mouse is standing
                m_SelEnd = findSelectionPointPosition(x - getPosition().x - m_LeftBorder);
            }
            else // Scrolling is enabled
            {
                float width, rightBorderWidth;

                // Calculate the width of the edit box and the width of the right border
                if (m_SplitImage)
                {
                    width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                             - ((m_LeftBorder + m_RightBorder) * getScale().y);
                    rightBorderWidth = m_RightBorder * getScale().y;
                }
                else
                {
                    width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;
                    rightBorderWidth = m_RightBorder * getScale().x;
                }

                // Create a temporary text object
                sf::Text tempText(m_TextBeforeSelection);
                tempText.setString(m_DisplayedText);

                // Check if the mouse is on the left of the text
                if (x - getPosition().x < m_LeftBorder)
                {
                    // Check if the left crop position can be decremented
                    if (m_LeftTextCrop > 0)
                    {
                        // Decrement the left crop position
                        --m_LeftTextCrop;

                        // Check if the right crop position should be lowered
                        while ((tempText.findCharacterPos(m_RightTextCrop).x - tempText.findCharacterPos(m_LeftTextCrop).x) > width)
                            --m_RightTextCrop;
                    }
                }
                // Check if the mouse is on the right of the text
                else if (x - getPosition().x > (width - rightBorderWidth))
                {
                    // Check if the right crop position can be incremented
                    if (m_RightTextCrop < m_DisplayedText.length())
                    {
                        // Increment the right crop position
                        ++m_RightTextCrop;

                        // Check if the left crop position should be raised
                        while ((tempText.findCharacterPos(m_RightTextCrop).x - tempText.findCharacterPos(m_LeftTextCrop).x) > width)
                            ++m_LeftTextCrop;
                    }
                }

                // Find out between which characters the mouse is standing
                m_SelEnd = findSelectionPointPosition(x - getPosition().x - m_LeftBorder);
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
                    m_TextBeforeSelection.setString(m_DisplayedText.substr(0, m_SelStart));
                    m_TextSelection.setString(m_DisplayedText.substr(m_SelStart, m_SelChars));
                    m_TextAfterSelection.setString(m_DisplayedText.substr(m_SelEnd));
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
                    m_TextBeforeSelection.setString(m_DisplayedText.substr(0, m_SelEnd));
                    m_TextSelection.setString(m_DisplayedText.substr(m_SelEnd, m_SelChars));
                    m_TextAfterSelection.setString(m_DisplayedText.substr(m_SelStart));
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

            // Create a temporary text object
            sf::Text tempText(m_TextBeforeSelection);
            tempText.setString(m_DisplayedText);

            // Check if there is a text width limit
            if (m_LimitTextWidth)
            {
                // Change the selection point
                if (m_SplitImage)
                    m_SelectionPointPosition = static_cast<unsigned int>(tempText.findCharacterPos(m_SelEnd).x + (m_LeftBorder * getScale().y));
                else
                    m_SelectionPointPosition = static_cast<unsigned int>(tempText.findCharacterPos(m_SelEnd).x + (m_LeftBorder * getScale().x));
            }
            else // Scrolling is enabled
            {
                // Change the selection point
                if (m_SplitImage)
                    m_SelectionPointPosition = static_cast<unsigned int>(tempText.findCharacterPos(m_SelEnd).x - tempText.findCharacterPos(m_LeftTextCrop).x + (m_LeftBorder * getScale().y));
                else
                    m_SelectionPointPosition = static_cast<unsigned int>(tempText.findCharacterPos(m_SelEnd).x - tempText.findCharacterPos(m_LeftTextCrop).x + (m_LeftBorder * getScale().x));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(sf::Keyboard::Key key)
    {
        // If the editbox is invisible then ignore that the key is pressed
        if (!m_Visible)
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
                // You don't have to do anything when the selection point is at the beginning of the text
                if (m_SelEnd > 0)
                {
                    // Check if scrolling is enabled
                    if (m_LimitTextWidth == false)
                    {
                        // Check if you should scroll left
                        if ((m_LeftTextCrop > 0) && ((m_LeftTextCrop == (m_SelEnd-1)) || (m_LeftTextCrop == m_SelEnd)))
                        {
                            float width;

                            // Calculate the width of the edit box
                            if (m_SplitImage)
                                width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                                         - ((m_LeftBorder + m_RightBorder) * getScale().y);
                            else
                                width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                            // Change the left crop position
                            --m_LeftTextCrop;

                            // Adjust the text before the selection, it has to be changed before the calculations
                            m_TextBeforeSelection.setString(m_DisplayedText);

                            // Change the right crop position (if necessary)
                            while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > width)
                                --m_RightTextCrop;
                        }
                    }

                    // Move the selection point to the left
                    setSelectionPointPosition(m_SelEnd - 1);
                }
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
                // You don't have to do anything when the selection point is at the end of the text
                if (m_SelEnd < m_DisplayedText.length())
                {
                    // Check if scrolling is enabled
                    if (m_LimitTextWidth == false)
                    {
                        // Check if you should scroll right
                        if ((m_RightTextCrop == m_SelEnd) || (m_RightTextCrop == (m_SelEnd + 1)))
                        {
                            float width;

                            // Calculate the width of the edit box
                            if (m_SplitImage)
                                width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                                         - ((m_LeftBorder + m_RightBorder) * getScale().y);
                            else
                                width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                            // Change the right crop position
                            ++m_RightTextCrop;

                            // Adjust the text before the selection, it has to be changed before the calculations
                            m_TextBeforeSelection.setString(m_DisplayedText);

                            // Change the left crop position (if necessary)
                            while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > width)
                                ++m_LeftTextCrop;
                        }
                    }

                    // Move the selection point to the left
                    setSelectionPointPosition(m_SelEnd + 1);
                }
            }

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Home)
        {
            // Check if scrolling is enabled
            if (m_LimitTextWidth == false)
            {
                float width;

                // Calculate the width of the edit box
                if (m_SplitImage)
                    width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                             - ((m_LeftBorder + m_RightBorder) * getScale().y);
                else
                    width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                // Reset the crop positions
                m_LeftTextCrop = 0;
                m_RightTextCrop = 0;

                // Adjust the text before the selection, it has to be changed before the calculations
                m_TextBeforeSelection.setString(m_DisplayedText);

                // Check if the right crop position can be raised
                while (m_RightTextCrop < m_DisplayedText.length())
                {
                    // Change the right crop position (if necessary)
                    if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop + 1).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) < width)
                        ++m_RightTextCrop;
                    else
                        break;
                }
            }

            // Set the selection point to the beginning of the text
            setSelectionPointPosition(0);

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::End)
        {
            // Check if scrolling is enabled
            if (m_LimitTextWidth == false)
            {
                float width;

                // Calculate the width of the edit box
                if (m_SplitImage)
                    width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                             - ((m_LeftBorder + m_RightBorder) * getScale().y);
                else
                    width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                // Reset the crop positions
                m_LeftTextCrop = m_DisplayedText.length();
                m_RightTextCrop = m_DisplayedText.length();

                // Adjust the text before the selection, it has to be changed before the calculations
                m_TextBeforeSelection.setString(m_DisplayedText);

                // Check if the right crop position can be raised
                while (m_LeftTextCrop > 0)
                {
                    // Change the right crop position (if necessary)
                    if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop - 1).x) < width)
                        --m_LeftTextCrop;
                    else
                        break;
                }
            }

            // Set the selection point behind the text
            setSelectionPointPosition(m_Text.length());

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (key == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::keyPress_Return;
                callback.text       = m_Text;
                m_Parent->addCallback(callback);
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

                // Check if scrolling is enabled
                if (m_LimitTextWidth == false)
                {
                    float width;

                    // Calculate the width of the edit box
                    if (m_SplitImage)
                        width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                        - ((m_LeftBorder + m_RightBorder) * getScale().y);
                    else
                        width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                    // Check if the right crop position was the end of the text
                    if (m_RightTextCrop == (m_DisplayedText.length() + 1))
                        --m_RightTextCrop;

                    // Adjust the text before the selection, it has to be changed before the calculations
                    m_TextBeforeSelection.setString(m_DisplayedText);

                    // Check if you can still scroll left
                    while (m_LeftTextCrop > 0)
                    {
                        // Check if a character has to be removed
                        if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop - 1).x) < width)
                        {
                            // Lower the left crop position
                            --m_LeftTextCrop;
                        }
                        else
                            break;
                    }
                }

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelEnd - 1);
            }
            else // When you did select some characters
            {
            deleteSelectedCharacters:

                // Check if they were selected from left to right
                if (m_SelStart < m_SelEnd)
                {
                    // Erase the characters
                    m_Text.erase(m_SelStart, m_SelChars);
                    m_DisplayedText.erase(m_SelStart, m_SelChars);

                    // Check if scrolling is enabled
                    if (m_LimitTextWidth == false)
                    {
                        float width;

                        // Calculate the width of the edit box
                        if (m_SplitImage)
                            width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                            - ((m_LeftBorder + m_RightBorder) * getScale().y);
                        else
                            width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                        // Check if the right crop position was the end of the text
                        if (m_RightTextCrop > m_DisplayedText.length())
                            m_RightTextCrop = m_DisplayedText.length();

                        // reset the left crop position
                        m_LeftTextCrop = m_RightTextCrop;

                        // Adjust the text before the selection, it has to be changed before the calculations
                        m_TextBeforeSelection.setString(m_DisplayedText);

                        // Check if you can still scroll left
                        while (m_LeftTextCrop > 0)
                        {
                            // Check if another character can be visible
                            if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop - 1).x) < width)
                            {
                                // Lower the left crop position
                                --m_LeftTextCrop;
                            }
                            else
                                break;
                        }
                    }

                    // Set the selection point back on the correct position
                    setSelectionPointPosition(m_SelStart);
                }
                else // When the text is selected from right to left
                {
                    // Erase the characters
                    m_Text.erase(m_SelEnd, m_SelChars);
                    m_DisplayedText.erase(m_SelEnd, m_SelChars);

                    // Check if scrolling is enabled
                    if (m_LimitTextWidth == false)
                    {
                        float width;

                        // Calculate the width of the edit box
                        if (m_SplitImage)
                            width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                            - ((m_LeftBorder + m_RightBorder) * getScale().y);
                        else
                            width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                        // reset the right crop position
                        m_RightTextCrop = m_LeftTextCrop;

                        // Adjust the text before the selection, it has to be changed before the calculations
                        m_TextBeforeSelection.setString(m_DisplayedText);

                        // Check if you can still scroll right
                        while (m_RightTextCrop < m_DisplayedText.length())
                        {
                            // Check if another character can be visible
                            if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop + 1).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) < width)
                            {
                                // Increment the right crop position
                                ++m_RightTextCrop;
                            }
                            else
                                break;
                        }
                    }

                    // Set the selection point back on the correct position
                    setSelectionPointPosition(m_SelEnd);
                }
            }

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::textChanged;
                callback.text       = m_Text;
                m_Parent->addCallback(callback);
            }
        }
        else if (key == sf::Keyboard::Delete)
        {
            // Make sure that no text is selected
            if (m_SelChars == 0)
            {
                // When the selection point is at the end of the line then you can't delete anything
                if (m_SelEnd == m_Text.length())
                    return;

                // Erase the character
                m_Text.erase(m_SelEnd, 1);
                m_DisplayedText.erase(m_SelEnd, 1);

                // Check if scrolling is enabled
                if (m_LimitTextWidth == false)
                {
                    float width;

                    // Calculate the width of the edit box
                    if (m_SplitImage)
                        width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                        - ((m_LeftBorder + m_RightBorder) * getScale().y);
                    else
                        width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

                    // Reset the right crop position
                    m_RightTextCrop = m_LeftTextCrop;

                    // Adjust the text before the selection, it has to be changed before the calculations
                    m_TextBeforeSelection.setString(m_DisplayedText);

                    // Make sure that there is still space after the right crop position
                    while (m_RightTextCrop < m_DisplayedText.length())
                    {
                        // Check if the next character may be visible
                        if ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop + 1).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) < width)
                        {
                            // The next character may be visible
                            ++m_RightTextCrop;
                        }
                        else // The next character may not become visible
                            break;
                    }
                }

                // Set the selection point back on the correct position
                setSelectionPointPosition(m_SelEnd);
            }
            else // You did select some characters
            {
                // This code is exactly the same as when pressing backspace
                goto deleteSelectedCharacters;
            }

            // The selection point should be visible again
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();

            // Add the callback (if the user requested it)
            if (callbackID > 0)
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::textChanged;
                callback.text       = m_Text;
                m_Parent->addCallback(callback);
            }
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::textEntered(char key)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        float width;

        // Calculate the width of the edit box
        if (m_SplitImage)
            width = ((m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x + m_TextureNormal_L->getSize().x) * getScale().x)
                     - ((m_LeftBorder + m_RightBorder) * getScale().y);
        else
            width = (m_TextureNormal_M->getSize().x - m_LeftBorder - m_RightBorder) * getScale().x;

        // Recalculate the text size when auto scaling
        if (m_TextSize == 0)
        {
            unsigned int size = static_cast<unsigned int>((m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * getScale().y);

            // Set the text size
            m_TextBeforeSelection.setCharacterSize(size);
            m_TextSelection.setCharacterSize(size);
            m_TextAfterSelection.setCharacterSize(size);
        }

        // Make sure we didn't select any characters
        if (m_SelChars == 0)
        {
          addCharacter:

            // Make sure we don't exceed our maximum characters limit
            if ((m_MaxChars > 0) && (m_Text.length() + 1 > m_MaxChars))
                return;

            // Insert our character
            m_Text.insert(m_Text.begin()+m_SelEnd, key);

            // Change the displayed text
            if (m_PasswordChar != '\0')
                m_DisplayedText.insert(m_DisplayedText.begin()+m_SelEnd, m_PasswordChar);
            else
                m_DisplayedText.insert(m_DisplayedText.begin()+m_SelEnd, key);

            // When there is a text width limit then reverse what we just did
            if (m_LimitTextWidth)
            {
                // Create a temporary SFML text object
                sf::Text tempText(m_TextBeforeSelection);
                tempText.setString(m_DisplayedText);

                // Now check if the text fits into the EditBox
//                    if (tempText.getGlobalBounds().width > width)
                if (tempText.findCharacterPos(tempText.getString().getSize()).x > width)
                {
                    // If the text does not fit in the EditBox then delete the added character
                    m_Text.erase(m_SelEnd, 1);
                    m_DisplayedText.erase(m_SelEnd, 1);
                    return;
                }
            }
            else // Scrolling is enabled
            {
                // Change the right crop position
                ++m_RightTextCrop;

                // Adjust the text before the selection, it has to be changed before the calculations
                m_TextBeforeSelection.setString(m_DisplayedText);

                // Check if you add a character at the end of the visible text
                if ((m_SelEnd+1) == m_RightTextCrop)
                {
                    // Change the left crop position (if necessary)
                    while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > width)
                        ++m_LeftTextCrop;
                }
                else // The character was inserted
                {
                    // Check if the last visible character may stay visible
                    while ((m_TextBeforeSelection.findCharacterPos(m_RightTextCrop).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x) > width)
                        --m_RightTextCrop;
                }
            }

            // Move our selection point forward
            setSelectionPointPosition(m_SelEnd + 1);
        }
        else // You did select some characters
        {
            // Fist we delete the selected characters
            keyPressed(sf::Keyboard::BackSpace);

            // And then we add our typed character
            goto addCharacter;
        }

        // The selection point should be visible again
        m_SelectionPointVisible = true;
        m_AnimationTimeElapsed = sf::Time();

        // Add the callback (if the user requested it)
        if (callbackID > 0)
        {
            Callback callback;
            callback.callbackID = callbackID;
            callback.trigger    = Callback::textChanged;
            callback.text       = m_Text;
            m_Parent->addCallback(callback);
        }
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
        m_SelectionPointVisible = m_SelectionPointVisible ? false : true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Drawing the edit box will be different when the image is split
        if (m_SplitImage)
        {
            // Set the position of the left image
            states.transform.translate(position);

            // Set the scale for the left image
            states.transform.scale(curScale.y, curScale.y);

            // Draw the left image
            {
                // When the normal edit box
                target.draw(m_SpriteNormal_L, states);

                // When the edit box is focused then draw an extra image
                if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                    target.draw(m_SpriteFocused_L, states);

                // When the mouse is on top of the edit box then draw an extra image
                if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                    target.draw(m_SpriteHover_L, states);
            }

            // Check if the middle image may be drawn
            if ((curScale.y * (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x))
                < curScale.x * (m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x))
            {
                // Calculate the scale for our middle image
                float ScaleX = (((m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x)
                                 / m_TextureNormal_M->getSize().x) * curScale.x) - (2 * curScale.y);

                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                // Set the scale for the middle image
                states.transform.scale(ScaleX / curScale.y, 1);

                // Draw the middle image
                {
                    // Draw the normal image
                    target.draw(m_SpriteNormal_M, states);

                    // When the edit box is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                        target.draw(m_SpriteFocused_M, states);

                    // When the mouse is on top of the edit box then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                        target.draw(m_SpriteHover_M, states);
                }

                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_M->getSize().x), 0);

                // Set the scale for the right image
                states.transform.scale(curScale.y / ScaleX, 1);

                // Draw the right image
                {
                    // Draw the normal image
                    target.draw(m_SpriteNormal_R, states);

                    // When the edit box is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                        target.draw(m_SpriteFocused_R, states);

                    // When the mouse is on top of the edit box then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                        target.draw(m_SpriteHover_R, states);
                }
            }
            else // The edit box isn't width enough, we will draw it at minimum size
            {
                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                // Draw the right image
                {
                    // Draw the normal image
                    target.draw(m_SpriteNormal_R, states);

                    // When the edit box is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                        target.draw(m_SpriteFocused_R, states);

                    // When the mouse is on top of the edit box then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                        target.draw(m_SpriteHover_R, states);
                }
            }
        }
        else // The image is not split
        {
            // Adjust the transformation
            states.transform *= getTransform();

            // Draw the edit box
            target.draw(m_SpriteNormal_M, states);

            // When the edit box is focused then draw an extra image
            if ((m_Focused) && (m_ObjectPhase & objectPhase::focused))
                target.draw(m_SpriteFocused_M, states);

            // When the mouse is on top of the edit boc then draw an extra image
            if ((m_MouseHover) && (m_ObjectPhase & objectPhase::hover))
                target.draw(m_SpriteHover_M, states);
        }

        // Reset the transformation to draw the text
        states.transform = oldTransform;

        // Set the translation
        states.transform.translate(position);

        if (m_SplitImage)
            states.transform.translate(static_cast<float>(m_LeftBorder * curScale.y), static_cast<float>(m_TopBorder * curScale.y));
        else
            states.transform.translate(static_cast<float>(m_LeftBorder * curScale.x), static_cast<float>(m_TopBorder * curScale.y));

        // Draw the background when a text is selected
        if (m_TextSelection.getString().getSize() > 0)
        {
            // Check if there is a text width limit
            if (m_LimitTextWidth)
            {
                // Create and draw the rectangle
//                sf::RectangleShape TextBgr(Vector2f(m_TextSelection.getGlobalBounds().width,
                sf::RectangleShape TextBgr(Vector2f(m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x,
                                                    ((m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * curScale.y)));

                if (m_Focused)
                    TextBgr.setFillColor(m_SelectedTextBgrColor);
                else
                    TextBgr.setFillColor(m_UnfocusedSelectedTextBgrColor);

//                TextBgr.setPosition(m_TextBeforeSelection.getGlobalBounds().width, 0);
                TextBgr.setPosition(m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x, 0);
                target.draw(TextBgr, states);
            }
            else // Scrolling is enabled
            {
                sf::RectangleShape TextBgr;
                Vector2f rectSize;

                // Create a temporary text object
                sf::Text tempText(m_TextBeforeSelection);
                tempText.setString(m_DisplayedText);

                // Check if the whole selected text is visible
                if ((m_TextBeforeSelection.getString().getSize() >= m_LeftTextCrop) && (tempText.getString().getSize() - m_TextAfterSelection.getString().getSize() <= m_RightTextCrop))
                {
//                    rectSize = Vector2f(m_TextSelection.getGlobalBounds().width,
                    rectSize = Vector2f(m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x,
                                        (m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * curScale.y);

//                    TextBgr.setPosition(m_TextBeforeSelection.getGlobalBounds().width - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x, 0);
                    TextBgr.setPosition(m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x - m_TextBeforeSelection.findCharacterPos(m_LeftTextCrop).x, 0);
                }
                else // Part of the selected text is invisible
                {
                    // Check if you are selecting from left to right
                    if (m_SelStart < m_SelEnd)
                    {
                        // Set the size of the rectangle
                        rectSize = Vector2f(tempText.findCharacterPos(m_SelEnd).x - tempText.findCharacterPos(m_LeftTextCrop).x,
                                                (m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * curScale.y);
                    }
                    else // You are selecting from right to left
                    {
                        // Set the size of the rectangle
                        rectSize = Vector2f(tempText.findCharacterPos(m_RightTextCrop).x - tempText.findCharacterPos(m_SelEnd).x,
                                                (m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * curScale.y);

                        // Set the position of the rectangle
                        TextBgr.setPosition(tempText.findCharacterPos(m_SelEnd).x - tempText.findCharacterPos(m_LeftTextCrop).x, 0);
                    }
                }

                if (m_Focused)
                    TextBgr.setFillColor(m_SelectedTextBgrColor);
                else
                    TextBgr.setFillColor(m_UnfocusedSelectedTextBgrColor);

                TextBgr.setSize(rectSize);
                target.draw(TextBgr, states);
            }
        }

        float textSize;

        // The text size will be different when auto scaling
        if (m_TextSize == 0)
            textSize = (m_TextureNormal_M->getSize().y - m_TopBorder - m_BottomBorder) * curScale.y;
        else
            textSize = static_cast<float>(m_TextSize);

        // Calculate the top position of the text
        float TopPosition = ((m_TextureNormal_M->getSize().y * curScale.y) * 0.33333f) - (textSize / 2.0f);

        // Set the position and scale
        states.transform.translate(0, TopPosition - m_TopBorder);

        // Check if there is atext width limit
        if (m_LimitTextWidth)
        {
            // Draw the text before the selection
            target.draw(m_TextBeforeSelection, states);

            // Check if there is a selection
            if (m_SelChars != 0)
            {
                // Draw the selected text
//                states.transform.translate(m_TextBeforeSelection.getGlobalBounds().width, 0);
                states.transform.translate(m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x, 0);
                target.draw(m_TextSelection, states);

                // Draw the text behind the selected text
//                states.transform.translate(m_TextSelection.getGlobalBounds().width, 0);
                states.transform.translate(m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x, 0);
                target.draw(m_TextAfterSelection, states);
            }
        }
        else // Scrolling is enabled
        {
            // Create a temporary text
            sf::Text tempTextBeforeSelection(m_TextBeforeSelection);

            // Check if the text before selection should be drawn
            if (m_LeftTextCrop < tempTextBeforeSelection.getString().getSize())
            {
                // Get the string
                std::string tempString = tempTextBeforeSelection.getString().toAnsiString();

                // Erase the invisible part of the text
                tempString.erase(0, m_LeftTextCrop);

                // Check if the text should be drawn completely or not
                if (m_RightTextCrop < tempTextBeforeSelection.getString().getSize())
                    tempString.erase(m_RightTextCrop - m_LeftTextCrop);

                // Make the changes in the text
                tempTextBeforeSelection.setString(tempString);

                // Draw the text before selection
                target.draw(tempTextBeforeSelection, states);
            }
            else // When the text before selection shouldn't be drawn
                tempTextBeforeSelection.setString("");

            // Check if there is a selection
            if (m_SelChars != 0)
            {
                // Create a temporary text
                sf::Text tempTextSelection(m_TextSelection);
                sf::Text tempTextAfterSelection(m_TextAfterSelection);

                // First store the size of the text before selection
                unsigned int charactersToSkip = m_TextBeforeSelection.getString().getSize();

                // Check if the selection should be drawn
                if (m_LeftTextCrop < charactersToSkip + m_TextSelection.getString().getSize())
                {
                    // Get the selected string
                    std::string tempString = m_TextSelection.getString().toAnsiString();

                    // Change the string of the temperary text
                    tempTextSelection.setString(tempString);

                    // Erase the invisible part of the text
                    if (m_LeftTextCrop > charactersToSkip)
                        tempString.erase(0, m_LeftTextCrop - charactersToSkip);

                    // Check if the text should be drawn completely or not
                    if (m_RightTextCrop < tempTextSelection.getString().getSize() + charactersToSkip)
                        tempString.erase(m_RightTextCrop - charactersToSkip);

                    // Make the changes in the text
                    tempTextSelection.setString(tempString);

                    // Draw the text before selection
//                    states.transform.translate(tempTextBeforeSelection.getGlobalBounds().width, 0);
                    states.transform.translate(tempTextBeforeSelection.findCharacterPos(tempTextBeforeSelection.getString().getSize()).x, 0);
                    target.draw(tempTextSelection, states);
                }
                else // The selected text shouldn't be drawn (should be impossible)
                    tempTextSelection.setString("");

                // Some more characters can be skipped now
                charactersToSkip += m_TextSelection.getString().getSize();

                // Check if the text after the selection should be drawn
                if (m_RightTextCrop > charactersToSkip)
                {
                    // Get the selected string
                    std::string tempString = m_TextAfterSelection.getString().toAnsiString();

                    // Change the string of the temperary text
                    tempTextAfterSelection.setString(tempString);

                    // Erase the invisible part of the text
                    if (m_LeftTextCrop > charactersToSkip)
                        tempString.erase(0, m_LeftTextCrop - charactersToSkip);

                    // Check if the text should be drawn completely or not
                    if (m_RightTextCrop < tempTextAfterSelection.getString().getSize() + charactersToSkip)
                        tempString.erase(m_RightTextCrop - charactersToSkip);

                    // Make the changes in the text
                    tempTextAfterSelection.setString(tempString);

                    // Draw the text before selection
//                    states.transform.translate(tempTextSelection.getGlobalBounds().width, 0);
                    states.transform.translate(tempTextSelection.findCharacterPos(tempTextSelection.getString().getSize()).x, 0);
                    target.draw(tempTextAfterSelection, states);
                }
            }
        }

        // Reset the transformation to draw the selection point
        states.transform = oldTransform;
        states.transform.translate(position);

        // Also draw the selection point (if needed)
        if ((m_Focused) && (m_SelectionPointVisible))
        {
            sf::RectangleShape SelectionPoint(Vector2f(static_cast<float>(selectionPointWidth),
                                                       (m_TextureNormal_M->getSize().y - m_BottomBorder - m_TopBorder) * curScale.y));
            SelectionPoint.setPosition(m_SelectionPointPosition - (selectionPointWidth*0.5f), static_cast<float>(m_TopBorder * curScale.y));
            SelectionPoint.setFillColor(selectionPointColor);

            // Draw the selection point
            target.draw(SelectionPoint, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
