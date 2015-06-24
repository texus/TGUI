/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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
    m_SelectionPointVisible (true),
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
    m_NumbersOnly           (false),
    m_SeparateHoverImage    (false)
    {
        m_Callback.widgetType = Type_EditBox;
        m_AnimatedWidget = true;
        m_DraggableWidget = true;
        m_AllowFocus = true;

        m_SelectionPoint.setSize(sf::Vector2f(1, 0));

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::EditBox(const EditBox& copy) :
    ClickableWidget         (copy),
    WidgetBorders           (copy),
    m_LoadedConfigFile      (copy.m_LoadedConfigFile),
    m_SelectionPointVisible (copy.m_SelectionPointVisible),
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
    m_SelectedTextBackground(copy.m_SelectedTextBackground),
    m_SelectionPoint        (copy.m_SelectionPoint),
    m_TextBeforeSelection   (copy.m_TextBeforeSelection),
    m_TextSelection         (copy.m_TextSelection),
    m_TextAfterSelection    (copy.m_TextAfterSelection),
    m_TextFull              (copy.m_TextFull),
    m_PossibleDoubleClick   (copy.m_PossibleDoubleClick),
    m_NumbersOnly           (copy.m_NumbersOnly),
    m_SeparateHoverImage    (copy.m_SeparateHoverImage)
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
        if (m_TextureNormal_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureHover_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureHover_R);

        if (m_TextureFocused_L.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox& EditBox::operator= (const EditBox& right)
    {
        if (this != &right)
        {
            EditBox temp(right);
            this->ClickableWidget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_LoadedConfigFile,       temp.m_LoadedConfigFile);
            std::swap(m_SelectionPointVisible,  temp.m_SelectionPointVisible);
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
            std::swap(m_SelectedTextBackground, temp.m_SelectedTextBackground);
            std::swap(m_SelectionPoint,         temp.m_SelectionPoint);
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
            std::swap(m_SeparateHoverImage,     temp.m_SeparateHoverImage);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox* EditBox::clone()
    {
        return new EditBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool EditBox::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;
        m_Size.x = 0;
        m_Size.y = 0;

        // Remove the textures when they were loaded before
        if (m_TextureNormal_L.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureHover_L.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureHover_L);
        if (m_TextureHover_M.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureHover_M);
        if (m_TextureHover_R.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureHover_R);
        if (m_TextureFocused_L.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureFocused_R);

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_LoadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read(sectionName, properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_LoadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = m_LoadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = m_LoadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "separatehoverimage")
            {
                m_SeparateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "textcolor")
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
                m_SelectedTextBackground.setFillColor(extractColor(value));
            }
            else if (property == "selectionpointcolor")
            {
                m_SelectionPoint.setFillColor(extractColor(value));
            }
            else if (property == "selectionpointwidth")
            {
                m_SelectionPoint.setSize(sf::Vector2f(static_cast<float>(atof(value.c_str())), m_SelectionPoint.getSize().y));
            }
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "normalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = false;
            }
            else if (property == "hoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_L in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "normalimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_M in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                m_SplitImage = true;
            }
            else if (property == "normalimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureNormal_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for NormalImage_R in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_L in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_M in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "hoverimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureHover_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for HoverImage_R in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_l")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_L))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_L in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_m")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_M))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_M in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "focusedimage_r")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureFocused_R))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for FocusedImage_R in section EditBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else
                TGUI_OUTPUT("TGUI error: Unrecognized property '" + property + "' in section EditBox in " + m_LoadedConfigFile + ".");
        }

        // Check if the image is split
        if (m_SplitImage)
        {
            // Make sure the required textures were loaded
            if ((m_TextureNormal_L.data != nullptr) && (m_TextureNormal_M.data != nullptr) && (m_TextureNormal_R.data != nullptr))
            {
                m_Loaded = true;
                setSize(static_cast<float>(m_TextureNormal_L.getSize().x + m_TextureNormal_M.getSize().x + m_TextureNormal_R.getSize().x),
                        static_cast<float>(m_TextureNormal_M.getSize().y));

                m_TextureNormal_M.data->texture.setRepeated(true);
            }
            else
            {
                TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the edit box. Is the EditBox section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if ((m_TextureFocused_L.data != nullptr) && (m_TextureFocused_M.data != nullptr) && (m_TextureFocused_R.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Focused;

                m_TextureFocused_M.data->texture.setRepeated(true);
            }
            if ((m_TextureHover_L.data != nullptr) && (m_TextureHover_M.data != nullptr) && (m_TextureHover_R.data != nullptr))
            {
                m_WidgetPhase |= WidgetPhase_Hover;

                m_TextureHover_M.data->texture.setRepeated(true);
            }
        }
        else // The image isn't split
        {
            // Make sure the required texture was loaded
            if (m_TextureNormal_M.data != nullptr)
            {
                m_Loaded = true;
                setSize(static_cast<float>(m_TextureNormal_M.getSize().x), static_cast<float>(m_TextureNormal_M.getSize().y));
            }
            else
            {
                TGUI_OUTPUT("TGUI error: NormalImage wasn't loaded. Is the EditBox section in " + m_LoadedConfigFile + " complete?");
                return false;
            }

            // Check if optional textures were loaded
            if (m_TextureFocused_M.data != nullptr)
            {
                m_WidgetPhase |= WidgetPhase_Focused;
            }
            if (m_TextureHover_M.data != nullptr)
            {
                m_WidgetPhase |= WidgetPhase_Hover;
            }
        }

        // Auto-size the text
        setTextSize(0);

        // When there is no error we will return true
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& EditBox::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        if (m_SplitImage)
        {
            m_TextureHover_L.sprite.setPosition(x, y);
            m_TextureNormal_L.sprite.setPosition(x, y);
            m_TextureFocused_L.sprite.setPosition(x, y);

            // Check if the middle image may be drawn
            if ((m_TextureNormal_M.sprite.getScale().y * (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x)) < m_Size.x)
            {
                float scalingY = m_Size.y / m_TextureNormal_M.getSize().y;

                m_TextureHover_M.sprite.setPosition(x + (m_TextureHover_L.getSize().x * m_TextureHover_L.sprite.getScale().x), y);
                m_TextureNormal_M.sprite.setPosition(x + (m_TextureNormal_L.getSize().x * m_TextureNormal_L.sprite.getScale().x), y);
                m_TextureFocused_M.sprite.setPosition(x + (m_TextureFocused_L.getSize().x * m_TextureFocused_L.sprite.getScale().x), y);

                m_TextureHover_R.sprite.setPosition(m_TextureHover_M.sprite.getPosition().x + (m_TextureHover_M.sprite.getTextureRect().width * scalingY), y);
                m_TextureNormal_R.sprite.setPosition(m_TextureNormal_M.sprite.getPosition().x + (m_TextureNormal_M.sprite.getTextureRect().width * scalingY), y);
                m_TextureFocused_R.sprite.setPosition(m_TextureFocused_M.sprite.getPosition().x + (m_TextureFocused_M.sprite.getTextureRect().width * scalingY), y);
            }
            else // The middle image isn't drawn
            {
                m_TextureHover_R.sprite.setPosition(x + (m_TextureHover_L.getSize().x * m_TextureHover_L.sprite.getScale().x), y);
                m_TextureNormal_R.sprite.setPosition(x + (m_TextureNormal_L.getSize().x * m_TextureNormal_L.sprite.getScale().x), y);
                m_TextureFocused_R.sprite.setPosition(x + (m_TextureFocused_L.getSize().x * m_TextureFocused_L.sprite.getScale().x), y);
            }
        }
        else // The images aren't split
        {
            m_TextureHover_M.sprite.setPosition(x, y);
            m_TextureNormal_M.sprite.setPosition(x, y);
            m_TextureFocused_M.sprite.setPosition(x, y);
        }

        recalculateTextPositions();
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

        // A negative size is not allowed for this widget
        if (m_Size.x < 0) m_Size.x = -m_Size.x;
        if (m_Size.y < 0) m_Size.y = -m_Size.y;

        // Recalculate the text size when auto scaling
        if (m_TextSize == 0)
            setText(m_Text);

        // Drawing the edit box image will be different when the image is split
        if (m_SplitImage)
        {
            float scalingY = m_Size.y / m_TextureNormal_M.getSize().y;
            float minimumWidth = (m_TextureNormal_L.getSize().x + m_TextureNormal_R.getSize().x) * scalingY;

            if (m_Size.x < minimumWidth)
                m_Size.x = minimumWidth;

            m_TextureHover_L.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_L.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_L.sprite.setScale(scalingY, scalingY);

            m_TextureHover_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureHover_M.getSize().y));
            m_TextureNormal_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureNormal_M.getSize().y));
            m_TextureFocused_M.sprite.setTextureRect(sf::IntRect(0, 0, static_cast<int>((m_Size.x - minimumWidth) / scalingY), m_TextureFocused_M.getSize().y));

            m_TextureHover_M.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_M.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_M.sprite.setScale(scalingY, scalingY);

            m_TextureHover_R.sprite.setScale(scalingY, scalingY);
            m_TextureNormal_R.sprite.setScale(scalingY, scalingY);
            m_TextureFocused_R.sprite.setScale(scalingY, scalingY);
        }
        else // The image is not split
        {
            m_TextureHover_M.sprite.setScale(m_Size.x / m_TextureHover_M.getSize().x, m_Size.y / m_TextureHover_M.getSize().y);
            m_TextureNormal_M.sprite.setScale(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);
            m_TextureFocused_M.sprite.setScale(m_Size.x / m_TextureFocused_M.getSize().x, m_Size.y / m_TextureFocused_M.getSize().y);
        }

        // Set the size of the selection point
        m_SelectionPoint.setSize(sf::Vector2f(static_cast<float>(m_SelectionPoint.getSize().x),
                                              m_Size.y - ((m_BottomBorder + m_TopBorder) * (m_Size.y / m_TextureNormal_M.getSize().y))));

        // Recalculate the position of the images and texts
        setPosition(getPosition());
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
            m_TextFull.setCharacterSize(static_cast<unsigned int>((m_Size.y - ((m_TopBorder + m_BottomBorder) * (m_Size.y / m_TextureNormal_M.getSize().y))) * 0.75f));
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

        // If the edit box only accepts numbers then remove all other characters
        if (m_NumbersOnly)
            setNumbersOnly(true);

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
            while (m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x - m_TextBeforeSelection.getPosition().x > width)
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

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* EditBox::getTextFont() const
    {
        return m_TextFull.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setPasswordCharacter(char passwordChar)
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

    char EditBox::getPasswordCharacter() const
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

    void EditBox::setAlignment(Alignment::Alignments alignment)
    {
        m_TextAlignment = alignment;
        setText(m_Text);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    EditBox::Alignment::Alignments EditBox::getAlignment()
    {
        return m_TextAlignment;
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

        // Set the size of the selection point
        m_SelectionPoint.setSize(sf::Vector2f(static_cast<float>(m_SelectionPoint.getSize().x),
                                          m_Size.y - ((m_BottomBorder + m_TopBorder) * (m_Size.y / m_TextureNormal_M.getSize().y))));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::changeColors(const sf::Color& color,
                               const sf::Color& selectedColor,
                               const sf::Color& selectedBgrColor,
                               const sf::Color& selectionPointColor)
    {
        m_TextBeforeSelection.setColor(color);
        m_TextSelection.setColor(selectedColor);
        m_TextAfterSelection.setColor(color);

        m_SelectionPoint.setFillColor(selectionPointColor);
        m_SelectedTextBackground.setFillColor(selectedBgrColor);
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
        m_SelectedTextBackground.setFillColor(selectedTextBackgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointColor(const sf::Color& selectionPointColor)
    {
        m_SelectionPoint.setFillColor(selectionPointColor);
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
        return m_SelectedTextBackground.getFillColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& EditBox::getSelectionPointColor() const
    {
        return m_SelectionPoint.getFillColor();
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
            while (m_TextBeforeSelection.findCharacterPos(m_DisplayedText.getSize()).x - m_TextBeforeSelection.getPosition().x > width)
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

        recalculateTextPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setSelectionPointWidth(unsigned int width)
    {
        m_SelectionPoint.setPosition(m_SelectionPoint.getPosition().x + ((m_SelectionPoint.getSize().x - width) / 2.0f), m_SelectionPoint.getPosition().y);
        m_SelectionPoint.setSize(sf::Vector2f(static_cast<float>(width),
                                          m_Size.y - ((m_BottomBorder + m_TopBorder) * (m_Size.y / m_TextureNormal_M.getSize().y))));

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int EditBox::getSelectionPointWidth() const
    {
        return static_cast<unsigned int>(m_SelectionPoint.getSize().x);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setNumbersOnly(bool numbersOnly)
    {
        m_NumbersOnly = numbersOnly;

        // Remove all letters from the edit box if needed
        if (numbersOnly && !m_Text.isEmpty())
        {
            sf::String newText;
            bool commaFound = false;

            if ((m_Text[0] == '+') || (m_Text[0] == '-'))
                newText += m_Text[0];

            for (unsigned int i = 0; i < m_Text.getSize(); ++i)
            {
                if (!commaFound)
                {
                    if ((m_Text[i] == ',') || (m_Text[i] == '.'))
                    {
                        newText += m_Text[i];
                        commaFound = true;
                    }
                }

                if ((m_Text[i] >= '0') && (m_Text[i] <= '9'))
                    newText += m_Text[i];
            }

            // When the text changed then reposition the text
            if (newText != m_Text)
                setText(newText);
        }
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        if (m_SplitImage)
        {
            m_TextureNormal_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureHover_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureFocused_L.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));

            m_TextureNormal_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureHover_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_TextureFocused_R.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        }

        m_TextureNormal_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureHover_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureFocused_M.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
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
            m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
            m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
            addCallback();
        }

        recalculateTextPositions();

        // The selection point should be visible
        m_SelectionPointVisible = true;
        m_AnimationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::mouseMoved(float x, float)
    {
        if (m_MouseHover == false)
            mouseEnteredWidget();

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
                float scalingX;
                if (m_SplitImage)
                    scalingX = m_Size.y / m_TextureNormal_M.getSize().y;
                else
                    scalingX = m_Size.x / m_TextureNormal_M.getSize().x;

                float width = m_Size.x - ((m_LeftBorder + m_RightBorder) * scalingX);

                // If the width is negative then the edit box is too small to be displayed
                if (width < 0)
                    width = 0;

                // Check if the mouse is on the left of the text
                if (x - getPosition().x < m_LeftBorder * scalingX)
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
                else if ((x - getPosition().x > (m_LeftBorder * scalingX) + width) && (m_TextFull.findCharacterPos(m_DisplayedText.getSize()).x > width))
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
                    m_TextBeforeSelection.setString(m_DisplayedText.substring(0, m_SelStart));
                    m_TextSelection.setString(m_DisplayedText.substring(m_SelStart, m_SelChars));
                    m_TextAfterSelection.setString(m_DisplayedText.substring(m_SelEnd));

                    recalculateTextPositions();
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
                    m_TextBeforeSelection.setString(m_DisplayedText.substring(0, m_SelEnd));
                    m_TextSelection.setString(m_DisplayedText.substring(m_SelEnd, m_SelChars));
                    m_TextAfterSelection.setString(m_DisplayedText.substring(m_SelStart));

                    recalculateTextPositions();
                }
            }
            else if (m_SelChars > 0)
            {
                // Adjust the number of characters that are selected
                m_SelChars = 0;

                // Change our three texts
                m_TextBeforeSelection.setString(m_DisplayedText);
                m_TextSelection.setString("");
                m_TextAfterSelection.setString("");

                recalculateTextPositions();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::keyPressed(const sf::Event::KeyEvent& event)
    {
        // Don't do anything when the edit box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Check if one of the correct keys was pressed
        if (event.code == sf::Keyboard::Left)
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
        else if (event.code == sf::Keyboard::Right)
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
        else if (event.code == sf::Keyboard::Home)
        {
            // Set the selection point to the beginning of the text
            setSelectionPointPosition(0);

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::End)
        {
            // Set the selection point behind the text
            setSelectionPointPosition(m_Text.getSize());

            // Our selection point has moved, it should be visible
            m_SelectionPointVisible = true;
            m_AnimationTimeElapsed = sf::Time();
        }
        else if (event.code == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_Callback.trigger = ReturnKeyPressed;
                m_Callback.text    = m_Text;
                addCallback();
            }
        }
        else if (event.code == sf::Keyboard::BackSpace)
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
        else if (event.code == sf::Keyboard::Delete)
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
        else
        {
            // Check if you are copying, pasting or cutting text
            if (event.control)
            {
                if (event.code == sf::Keyboard::C)
                {
                    TGUI_Clipboard.set(m_TextSelection.getString());
                }
                else if (event.code == sf::Keyboard::V)
                {
                    auto clipboardContents = TGUI_Clipboard.get();

                    // Only continue pasting if you actually have to do something
                    if ((m_SelChars > 0) || (clipboardContents.getSize() > 0))
                    {
                        deleteSelectedCharacters();

                        unsigned int oldCaretPos = m_SelEnd;

                        if (m_Text.getSize() > m_SelEnd)
                            setText(m_Text.substring(0, m_SelEnd) + TGUI_Clipboard.get() + m_Text.substring(m_SelEnd, m_Text.getSize() - m_SelEnd));
                        else
                            setText(m_Text + clipboardContents);

                        setSelectionPointPosition(oldCaretPos + clipboardContents.getSize());

                        // Add the callback (if the user requested it)
                        if (m_CallbackFunctions[TextChanged].empty() == false)
                        {
                            m_Callback.trigger = TextChanged;
                            m_Callback.text    = m_Text;
                            addCallback();
                        }
                    }
                }
                else if (event.code == sf::Keyboard::X)
                {
                    TGUI_Clipboard.set(m_TextSelection.getString());
                    deleteSelectedCharacters();
                }
                else if (event.code == sf::Keyboard::A)
                {
                    m_SelStart = 0;
                    m_SelEnd = m_Text.getSize();
                    m_SelChars = m_Text.getSize();

                    m_TextBeforeSelection.setString("");
                    m_TextSelection.setString(m_DisplayedText);
                    m_TextAfterSelection.setString("");

                    recalculateTextPositions();
                }
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
            if ((key < '0') || (key > '9'))
            {
                if ((key == '-') || (key == '+'))
                {
                    if ((m_SelStart == 0) || (m_SelEnd == 0))
                    {
                        if (!m_Text.isEmpty())
                        {
                            // You can't have multiple + and - characters after each other
                            if ((m_Text[0] == '-') || (m_Text[0] == '+'))
                                return;
                        }
                    }
                    else // + and - symbols are only allowed at the beginning of the line
                        return;
                }
                else if ((key == ',') || (key == '.'))
                {
                    // Only one comma is allowed
                    for (auto it = m_Text.begin(); it != m_Text.end(); ++it)
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

    void EditBox::widgetUnfocused()
    {
        // If there is a selection then undo it now
        if (m_SelChars)
            setSelectionPointPosition(m_SelEnd);

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
            setTextSize(atoi(value.c_str()));
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
            setMaximumCharacters(atoi(value.c_str()));
        }
        else if (property == "alignment")
        {
            if ((value == "left") || (value == "Left"))
                setAlignment(Alignment::Left);
            else if ((value == "center") || (value == "Center"))
                setAlignment(Alignment::Center);
            else if ((value == "right") || (value == "Right"))
                setAlignment(Alignment::Right);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Alignment' property.");
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
            setSelectionPointWidth(atoi(value.c_str()));
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
            value = to_string(getTextSize());
        else if (property == "passwordcharacter")
        {
            if (getPasswordCharacter())
                value = getPasswordCharacter();
            else
                value = "";
        }
        else if (property == "maximumcharacters")
            value = to_string(getMaximumCharacters());
        else if (property == "alignment")
        {
            if (m_TextAlignment == Alignment::Left)
                value = "Left";
            else if (m_TextAlignment == Alignment::Center)
                value = "Center";
            else
                value = "Right";
        }
        else if (property == "borders")
            value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + to_string(int(getSelectedTextColor().r)) + "," + to_string(int(getSelectedTextColor().g))
                    + "," + to_string(int(getSelectedTextColor().b)) + "," + to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "selectedtextbackgroundcolor")
            value = "(" + to_string(int(getSelectedTextBackgroundColor().r)) + "," + to_string(int(getSelectedTextBackgroundColor().g))
                    + "," + to_string(int(getSelectedTextBackgroundColor().b)) + "," + to_string(int(getSelectedTextBackgroundColor().a)) + ")";
        else if (property == "selectionpointcolor")
            value = "(" + to_string(int(getSelectionPointColor().r)) + "," + to_string(int(getSelectionPointColor().g))
                    + "," + to_string(int(getSelectionPointColor().b)) + "," + to_string(int(getSelectionPointColor().a)) + ")";
        else if (property == "limittextwidth")
            value = m_LimitTextWidth ? "true" : "false";
        else if (property == "selectionpointwidth")
            value = to_string(getSelectionPointWidth());
        else if (property == "numbersonly")
            value = m_NumbersOnly ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(TextChanged) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(TextChanged).size() == 1) && (m_CallbackFunctions.at(TextChanged).front() == nullptr))
                callbacks.push_back("TextChanged");
            if ((m_CallbackFunctions.find(ReturnKeyPressed) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ReturnKeyPressed).size() == 1) && (m_CallbackFunctions.at(ReturnKeyPressed).front() == nullptr))
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
        list.push_back(std::pair<std::string, std::string>("Alignment", "string"));
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
        tempString = m_DisplayedText.substring(0, firstVisibleChar);
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
        // Nothing to delete when no text was selected
        if (m_SelChars == 0)
            return;

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

    void EditBox::recalculateTextPositions()
    {
        float textX = getPosition().x;
        float textY = getPosition().y;

        // Calculate the scaling
        sf::Vector2f scaling(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);

        // Calculate the scale of the left and right border
        float borderScale;
        if (m_SplitImage)
            borderScale = scaling.y;
        else
            borderScale = scaling.x;

        textX += m_LeftBorder * borderScale - m_TextCropPosition;
        textY += m_TopBorder * scaling.y;

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
                    textX += (width - textWidth) / 2.f;
                else // if (textAlignment == Alignment::Right)
                    textX += width - textWidth;
            }
        }

        float selectionPointLeft = textX;

        // Set the position of the text
        sf::Text tempText(m_TextFull);
        tempText.setString("kg");
        textY += (((m_Size.y - ((m_TopBorder + m_BottomBorder) * scaling.y)) - tempText.getLocalBounds().height) * 0.5f) - tempText.getLocalBounds().top;

        // Set the text before the selection on the correct position
        m_TextBeforeSelection.setPosition(std::floor(textX + 0.5f), std::floor(textY + 0.5f));

        // Check if there is a selection
        if (m_SelChars != 0)
        {
            // Watch out for the kerning
            if (m_TextBeforeSelection.getString().getSize() > 0)
                textX += m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[m_TextBeforeSelection.getString().getSize() - 1], m_DisplayedText[m_TextBeforeSelection.getString().getSize()], m_TextBeforeSelection.getCharacterSize());

            textX += m_TextBeforeSelection.findCharacterPos(m_TextBeforeSelection.getString().getSize()).x - m_TextBeforeSelection.getPosition().x;

            // Set the position and size of the rectangle that gets drawn behind the selected text
            m_SelectedTextBackground.setSize(sf::Vector2f(m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x - m_TextSelection.getPosition().x,
                                                          (m_Size.y - ((m_TopBorder + m_BottomBorder) * scaling.y))));
            m_SelectedTextBackground.setPosition(std::floor(textX + 0.5f), std::floor(getPosition().y + (m_TopBorder * scaling.y) + 0.5f));

            // Set the text selected text on the correct position
            m_TextSelection.setPosition(std::floor(textX + 0.5f), std::floor(textY + 0.5f));

            // Watch out for kerning
            if (m_DisplayedText.getSize() > m_TextBeforeSelection.getString().getSize() + m_TextSelection.getString().getSize())
                textX += m_TextBeforeSelection.getFont()->getKerning(m_DisplayedText[m_TextBeforeSelection.getString().getSize() + m_TextSelection.getString().getSize() - 1], m_DisplayedText[m_TextBeforeSelection.getString().getSize() + m_TextSelection.getString().getSize()], m_TextBeforeSelection.getCharacterSize());

            // Set the text selected text on the correct position
            textX += m_TextSelection.findCharacterPos(m_TextSelection.getString().getSize()).x  - m_TextSelection.getPosition().x;
            m_TextAfterSelection.setPosition(std::floor(textX + 0.5f), std::floor(textY + 0.5f));
        }

        // Set the position of the selection point
        selectionPointLeft += m_TextFull.findCharacterPos(m_SelEnd).x - (m_SelectionPoint.getSize().x * 0.5f);
        m_SelectionPoint.setPosition(std::floor(selectionPointLeft + 0.5f), std::floor((m_TopBorder * scaling.y) + getPosition().y + 0.5f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void EditBox::initialize(Container *const parent)
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
        // Don't draw anything when the edit box was not loaded correctly
        if (m_Loaded == false)
            return;

        if (m_SplitImage)
        {
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureHover_L, states);
                    target.draw(m_TextureHover_M, states);
                    target.draw(m_TextureHover_R, states);
                }
                else
                {
                    target.draw(m_TextureNormal_L, states);
                    target.draw(m_TextureNormal_M, states);
                    target.draw(m_TextureNormal_R, states);
                }
            }
            else // The hover image is drawn on top of the normal one
            {
                target.draw(m_TextureNormal_L, states);
                target.draw(m_TextureNormal_M, states);
                target.draw(m_TextureNormal_R, states);

                // When the mouse is on top of the edit box then draw an extra image
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                {
                    target.draw(m_TextureHover_L, states);
                    target.draw(m_TextureHover_M, states);
                    target.draw(m_TextureHover_R, states);
                }
            }

            // When the edit box is focused then draw an extra image
            if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
            {
                target.draw(m_TextureFocused_L, states);
                target.draw(m_TextureFocused_M, states);
                target.draw(m_TextureFocused_R, states);
            }
        }
        else // The images aren't split
        {
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureHover_M, states);
                else
                    target.draw(m_TextureNormal_M, states);
            }
            else // The hover image is drawn on top of the normal one
            {
                target.draw(m_TextureNormal_M, states);

                // When the mouse is on top of the edit box then draw an extra image
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureHover_M, states);
            }

            // When the edit box is focused then draw an extra image
            if ((m_Focused) && (m_WidgetPhase & WidgetPhase_Focused))
                target.draw(m_TextureFocused_M, states);
        }

        // Calculate the scaling
        sf::Vector2f scaling(m_Size.x / m_TextureNormal_M.getSize().x, m_Size.y / m_TextureNormal_M.getSize().y);

        // Calculate the scale of the left and right border
        float borderScale;
        if (m_SplitImage)
            borderScale = scaling.y;
        else
            borderScale = scaling.x;

        // Calculate the scale factor of the view
        const sf::View& view = target.getView();
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x + (m_LeftBorder * borderScale) - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y + (m_TopBorder * scaling.y) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + (m_Size.x - (m_RightBorder * borderScale)) - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + (m_Size.y - (m_BottomBorder * scaling.y)) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

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

        target.draw(m_TextBeforeSelection, states);

        if (m_TextSelection.getString().isEmpty() == false)
        {
            target.draw(m_SelectedTextBackground, states);

            target.draw(m_TextSelection, states);
            target.draw(m_TextAfterSelection, states);
        }

        // Draw the selection point
        if ((m_Focused) && (m_SelectionPointVisible))
            target.draw(m_SelectionPoint, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
